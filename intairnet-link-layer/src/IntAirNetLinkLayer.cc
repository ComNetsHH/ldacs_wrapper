/*
 * IntAirNetLinkLayer.cc
 *
 *  Created on: 5 Dec 2020
 *      Author: fu
 */

#include <functional>
#include <cxxabi.h>
#include "IntAirNetLinkLayer.h"
#include "IntAirNetLinkLayerPacket.h"
#include "PacketFactory.h"
#include "inet/common/ModuleAccess.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/common/ProtocolGroup.h"
#include <GpsrModified.h>
#include <GpsrModified_m.h>
#include <Rlc.hpp>
#include <PassThroughArq.hpp>
#include <SelectiveRepeatArq.hpp>
#include <L3Packet.hpp>
#include <IOmnetPluggable.hpp>
#include <ContentionMethod.hpp>
#include <InetPacketPayload.hpp>
#include <SimulatorPosition.hpp>
#include "MacLayer.h"
#include "PhyLayer.h"
#include "LinkLayerLifecycleManager.h"

using namespace inet::physicallayer;
using namespace TUHH_INTAIRNET_RLC;
using namespace TUHH_INTAIRNET_ARQ;

Define_Module(IntAirNetLinkLayer);

IntAirNetLinkLayer::~IntAirNetLinkLayer() {}

void IntAirNetLinkLayer::finish() {
    LinkLayer::finish();
    delete ((Rlc*)this->rlcSubLayer);
    delete this->macSubLayer;
    delete this->arqSubLayer;
    delete this->phySubLayer;
    //cancelAndDelete(subLayerTimerMessage);
}

void IntAirNetLinkLayer::initialize(int stage) {    
    LayeredProtocolBase::initialize(stage);
    LinkLayer::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        slotDuration = par("slotDuration");
        gpsrIsUsed = par("gpsrIsUsed").boolValue();
        arqIsUsed = par("arqIsUsed").boolValue();        
	
        mcsotdma_statistics_map.clear();
        for (size_t i = 0; i < str_mcsotdma_statistics.size(); i++) {
            const std::string& s = str_mcsotdma_statistics.at(i);
            mcsotdma_statistics_map[s] = registerSignal(s.c_str());
        }        
    }
    else if (stage == INITSTAGE_LINK_LAYER) {                
        // which contention method to use
        std::string contention_method = par("contentionMethod");
        ContentionMethod method;
        if (contention_method.compare("binomial_estimate") == 0)
            method = ContentionMethod::binomial_estimate;
        else if (contention_method.compare("poisson_binomial_estimate") == 0)
            method = ContentionMethod::poisson_binomial_estimate;
        else if (contention_method.compare("randomized_slotted_aloha") == 0)
            method = ContentionMethod::randomized_slotted_aloha;
        else if (contention_method.compare("naive_random_access") == 0)
            method = ContentionMethod::naive_random_access;
        else
            throw std::invalid_argument("contentionMethod is invalid, it should be one of 'binomial_estimate', 'poisson_binomial_estimate', 'randomized_slotted_aloha', 'naive_random_access'.");
        macSubLayer->setContentionMethod(method);        
        macSubLayer->setBcSlotSelectionMinNumCandidateSlots(par("broadcastSlotSelectionMinNumCandidateSlots"));
        macSubLayer->setBcSlotSelectionMaxNumCandidateSlots(par("broadcastSlotSelectionMaxNumCandidateSlots"));
        macSubLayer->setAlwaysScheduleNextBroadcastSlot(par("alwaysScheduleNextBroadcastSlot"));        
        macSubLayer->setAdvertiseNextBroadcastSlotInCurrentHeader(par("advertiseNextBroadcastSlotInCurrentHeader"));                
        macSubLayer->setEnableBeacons(par("enableBeacons"));
        macSubLayer->setWriteResourceUtilizationIntoBeacon(par("writeResourceUtilizationToBeaconPayload"));        
        macSubLayer->setMinBeaconOffset(par("minBeaconInterval"));
        macSubLayer->setMaxBeaconOffset(par("maxBeaconInterval"));
        macSubLayer->setBroadcastTargetCollisionProb(par("broadcastTargetCollisionRate"));
        macSubLayer->setForceBidirectionalLinks(par("forceBidirectionalP2PLinks"));        
        macSubLayer->setPPLinkBurstOffset(par("ppBurstOffset"));       
        macSubLayer->setPPLinkBurstOffsetAdaptive(par("adaptivePPBurstOffset"));               
        
        // Report Beacon Callback
        function<void (MacId, L2HeaderBeacon)> reportBeaconCallback = [this](MacId origin_id, L2HeaderBeacon header){
            return this->onBeaconReceive(origin_id, header);
        };

        macSubLayer->setOmnetPassUpBeaconFct(reportBeaconCallback);

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {                
        MacAddress address = interfaceEntry->getMacAddress();
        uint32_t planning_horizon = par("planningHorizon");        


        rlcSubLayer = new Rlc(1600);
        if(arqIsUsed) {
            arqSubLayer = new SelectiveRepeatArq(MacId(address.getInt()), 100, 100, 1 + par("numRtx").intValue(), par("per").doubleValue());
        } else {
            arqSubLayer = new PassThroughArq();
        }
        
        macSubLayer = new MacLayer(MacId(address.getInt()), planning_horizon);
        phySubLayer = new PhyLayer(planning_horizon);

        auto reservation_manager = ((MacLayer*)macSubLayer)->getReservationManager();


        reservation_manager->setTransmitterReservationTable(((PhyLayer*)phySubLayer)->getTransmitterReservationTable());
        for (ReservationTable*& table : ((PhyLayer*)phySubLayer)->getReceiverReservationTables()) {
            reservation_manager->addReceiverReservationTable(table);
        }

        uint64_t bc_frequency = 960, bandwidth = 500;
        reservation_manager->addFrequencyChannel(false, bc_frequency, bandwidth);
        int numPPChannels = par("numPPChannels");
        for (int i = 0; i < numPPChannels; i++) {
            uint64_t frequency = bc_frequency + (i+1);
            reservation_manager->addFrequencyChannel(true, frequency, bandwidth);
        }                

        rlcSubLayer->setLowerLayer(arqSubLayer);
        rlcSubLayer->setUpperLayer((INet*)this);
        arqSubLayer->setUpperLayer(rlcSubLayer);
        arqSubLayer->setLowerLayer(macSubLayer);
        macSubLayer->setUpperLayer(arqSubLayer);
        macSubLayer->setLowerLayer(phySubLayer);
        phySubLayer->setUpperLayer(macSubLayer);
        phySubLayer->setRadio((IRadio*)this);

        /** Register callback functions **/
        // GetTime
        function<double()> getTimeFkt= [this]{
            return simTime().dbl();
        };
        ((Rlc*)rlcSubLayer)->registerGetTimeCallback(getTimeFkt);
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerGetTimeCallback(getTimeFkt);
        }else {
            ((PassThroughArq*)arqSubLayer)->registerGetTimeCallback(getTimeFkt);
        }
        ((MacLayer*)macSubLayer)->registerGetTimeCallback(getTimeFkt);

        // Schedule At
        ((Rlc*)rlcSubLayer)->registerScheduleAtCallback([this](double time){
            this->addCallback((IOmnetPluggable*)this->rlcSubLayer, time);
        });
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerScheduleAtCallback([this](double time){
                this->addCallback((IOmnetPluggable*)this->arqSubLayer, time);
            });
        } else {
            ((PassThroughArq*)arqSubLayer)->registerScheduleAtCallback([this](double time){
                this->addCallback((IOmnetPluggable*)this->arqSubLayer, time);
            });
        }
        ((MacLayer*)macSubLayer)->registerScheduleAtCallback([this](double time){
            this->addCallback((IOmnetPluggable*)this->macSubLayer, time);
        });

        // Emit statistic
        function<void(string, double)> emitFkt = [this](string message, double value){
            this->emitStatistic(message, value);
        };
        ((Rlc*)rlcSubLayer)->registerEmitEventCallback(emitFkt);
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerEmitEventCallback(emitFkt);
        } else {
            ((PassThroughArq*)arqSubLayer)->registerEmitEventCallback(emitFkt);
        }
        ((MacLayer*)macSubLayer)->registerEmitEventCallback(emitFkt);
        ((PhyLayer*)phySubLayer)->registerEmitEventCallback(emitFkt);

        // Delete packets
        function<void(L2Packet*)> deleteFkt = [this](L2Packet* pkt){
            this->onPacketDelete(pkt);
        };
        ((Rlc*)rlcSubLayer)->registerDeleteL2Callback(deleteFkt);
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerDeleteL2Callback(deleteFkt);
        } else {
            ((PassThroughArq*)arqSubLayer)->registerDeleteL2Callback(deleteFkt);
        }
        ((MacLayer*)macSubLayer)->registerDeleteL2Callback(deleteFkt);
        ((PhyLayer*)phySubLayer)->registerDeleteL2Callback(deleteFkt);


        // Delete Payloads
        function<void(L2Packet::Payload*)> deletePayloadFkt = [this](L2Packet::Payload* payload){
            this->onPayloadDelete(payload);
        };
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerDeleteL2PayloadCallback(deletePayloadFkt);
        } else {
            ((PassThroughArq*)arqSubLayer)->registerDeleteL2PayloadCallback(deletePayloadFkt);
        }

        // Deep Copy Packets
        function<L2Packet*(L2Packet*)> copyFkt = [this](L2Packet* pkt) {
            return this->copyL2Packet(pkt);
        };
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerCopyL2Callback(copyFkt);
        } else {
            ((PassThroughArq*)arqSubLayer)->registerCopyL2Callback(copyFkt);
        }

        // Deep Copy Packet Payloads
        function<L2Packet::Payload*(L2Packet::Payload*)> copyPayloadFkt = [this](L2Packet::Payload* payload) {
            return this->copyL2PacketPayload(payload);
        };
        if(arqIsUsed) {
            ((SelectiveRepeatArq*)arqSubLayer)->registerCopyL2PayloadCallback(copyPayloadFkt);
        } else {
            ((PassThroughArq*)arqSubLayer)->registerCopyL2PayloadCallback(copyPayloadFkt);
        }
        
        // getPosition
        function<SimulatorPosition()> getPositionFkt = [this](){
            auto pos = this->mobility->getCurrentPosition();
            return SimulatorPosition(pos.x, pos.y, pos.z);
        };
        ((MacLayer*)macSubLayer)->registerGetPositionCallback(getPositionFkt);                        
    }

}

void IntAirNetLinkLayer::handleUpperPacket(Packet *packet) {
    L3Packet* int_air_net_packet = PacketFactory::fromInetPacket(packet);
    auto macAddressReq = packet->getTag<MacAddressReq>();
    MacAddress address = macAddressReq->getDestAddress();
    auto destination_mac_id = MacId(address.getInt());

    if(address.isBroadcast()) {
        destination_mac_id = SYMBOLIC_LINK_ID_BROADCAST;
    }

    try {
        rlcSubLayer->receiveFromUpper(int_air_net_packet, destination_mac_id);
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::handleUpperPacket: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::handleLowerPacket(Packet *packet) {
    auto* pkt = (IntAirNetLinkLayerPacket*) packet;
    L2Packet* containedPacket = pkt->getContainedPacket();
    auto center_frequency = pkt->center_frequency;
    MacAddress address = interfaceEntry->getMacAddress();
    auto id = pkt->destId;
    if(id > 0 && id != MacId(address.getInt()).getId()) {
        return;
    }

    auto headers = containedPacket->getHeaders();
    auto payloads = containedPacket->getPayloads();
    int i = 0;
    for (const auto* payload : payloads){
        if(headers[i]->frame_type == L2Header::FrameType::broadcast || headers[i]->frame_type == L2Header::FrameType::unicast) {
            if(payload != nullptr) {
                if(((InetPacketPayload*)payload)->original) {
                    take(((InetPacketPayload*)payload)->original);
                }
            }
        }
        i++;
    }

    try {
        phySubLayer->onReception(containedPacket, center_frequency);
        pkt->attachPacket(nullptr);
        delete pkt;
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::handleLowerPacket: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::addCallback(IOmnetPluggable *layer, double time) {

}


// Pretend to be PHY and get packet from MAC
void IntAirNetLinkLayer::sendToChannel(L2Packet* data, uint64_t center_frequency)  {
    auto pkt = PacketFactory::fromL2Packet(data, center_frequency);
    pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);
    try {
        sendDown(pkt);
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::sendToChannel: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::receiveFromLower(L3Packet* packet) {
    if(packet == nullptr) {
        return;
    }

    Packet* original = (Packet*)packet->original;

    if(original == nullptr) {
        delete packet;
        return;
    }

    auto macAddressReq = original->getTag<MacAddressReq>();
    auto macAddressInd = original->addTagIfAbsent<MacAddressInd>();
    macAddressInd->setSrcAddress(macAddressReq->getSrcAddress());
    macAddressInd->setDestAddress(macAddressReq->getDestAddress());
    original->addTagIfAbsent<InterfaceInd>()->setInterfaceId(interfaceEntry->getInterfaceId());
    auto payloadProtocol = ProtocolGroup::ethertype.getProtocol(ProtocolGroup::ethertype.getProtocolNumber(original->getTag<PacketProtocolTag>()->getProtocol()));
    original->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(payloadProtocol);
    original->addTagIfAbsent<PacketProtocolTag>()->setProtocol(payloadProtocol);
    try {
        sendUp(original);
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::receiveFromLower: " << e.what() << std::endl;
        throw e;
    }
    delete packet;
}

void IntAirNetLinkLayer::emitStatistic(string statistic_name, double value) {
    const auto it = mcsotdma_statistics_map.find(statistic_name);
    if (it != mcsotdma_statistics_map.end())
        emit((*it).second, value);
    else
        throw std::invalid_argument("Emitted statistic not registered: '" + statistic_name + "'.");
}

void IntAirNetLinkLayer::beforeSlotStart() {
    Enter_Method_Silent();

    if(operationalState == State::NOT_OPERATING) {
        return;
    }

    try {
        ((MacLayer*)macSubLayer)->update(1);
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::beforeSlotStart: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::onSlotStart() {
    Enter_Method_Silent();

    if(operationalState == State::NOT_OPERATING) {
        return;
    }

    try {
        ((MacLayer*)macSubLayer)->execute();
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::onSlotStart: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::onSlotEnd() {
    Enter_Method_Silent();

    if(operationalState == State::NOT_OPERATING) {
        return;
    }

    try {
        ((MacLayer*)macSubLayer)->onSlotEnd();
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::onSlotEnd: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::onPacketDelete(L2Packet* pkt) {
    auto headers = pkt->getHeaders();
    auto payloads = pkt->getPayloads();
    int i = 0;
    for (const auto* payload : payloads){
        if(headers[i]->frame_type == L2Header::FrameType::broadcast || headers[i]->frame_type == L2Header::FrameType::unicast) {
            if(payload != nullptr) {
                if(((InetPacketPayload*)payload)->original) {
                    delete ((InetPacketPayload*)payload)->original;
                    ((InetPacketPayload*)payload)->original = nullptr;
                }
            }
        }
        i++;
    }
}

void IntAirNetLinkLayer::onPayloadDelete(L2Packet::Payload* payload) {
    if(!payload) {
        return;
    }
    auto inetPayload = (InetPacketPayload*)payload;
    if(inetPayload->original){
       delete inetPayload->original;
    }

    delete payload;
}

L2Packet* IntAirNetLinkLayer::copyL2Packet(L2Packet* original) {
    auto packet = original->copy();
    auto originalPayloads = original->getPayloads();
    auto newHeaders = original->getHeaders();
    auto newPayloads = packet->getPayloads();
    for(int i = 0; i< newHeaders.size(); i++) {
        if(newHeaders[i]->frame_type == L2Header::FrameType::broadcast || newHeaders[i]->frame_type == L2Header::FrameType::unicast) {
            if(originalPayloads[i] != nullptr) {
                if(((InetPacketPayload*)originalPayloads[i])->original) {
                    ((InetPacketPayload*)newPayloads[i])->original = ((InetPacketPayload*)(originalPayloads[i]))->original->dup();
                }
            }
        }
    }
    return packet;
}


L2Packet::Payload* IntAirNetLinkLayer::copyL2PacketPayload(L2Packet::Payload* originalPayload) {
    auto newPayload = originalPayload->copy();

    if(((InetPacketPayload*)originalPayload)->original){
        ((InetPacketPayload*)newPayload)->original = ((InetPacketPayload*)originalPayload)->original->dup();
    }

    return newPayload;

}

void IntAirNetLinkLayer::onBeaconReceive(MacId origin_id, L2HeaderBeacon header) {
    if(!gpsrIsUsed) {
        return;
    }
    auto encodedPosition = header.position.encodedPosition;
    Coord rcvdPosition = Coord(encodedPosition.x, encodedPosition.y, encodedPosition.z);
    auto rcvdMacAddress = MacAddress(0x0AAA00000000ULL + (origin_id.getId() & 0xffffffffUL));
    L3Address rcvdIpAddress = arp->getL3AddressFor(rcvdMacAddress);

    // @Musab, this code snippet will pass the beacon up, replace "MyNewGpsr" with you actual class name and make processBeacon a public function :)
    // Pass up beacon directly to gpsr (skipping NW layer)
    //GpsrModified* gpsr = getModuleFromPar<GpsrModified>(par("gpsrModule"), this);
    GpsrModified* gpsr = getModuleFromPar<GpsrModified>(par("gpsrModule"), this);
    gpsr->processBeaconMCSOTDMA(rcvdIpAddress, rcvdPosition);
   
}

bool IntAirNetLinkLayer::isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const {
    return macSubLayer->isGoingToTransmitDuringCurrentSlot(center_frequecy);
}




