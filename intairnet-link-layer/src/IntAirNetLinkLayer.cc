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
#include <Rlc.hpp>
#include <PassThroughArq.hpp>
#include <SelectiveRepeatArq.hpp>
#include <L3Packet.hpp>
#include <IOmnetPluggable.hpp>
#include <ContentionMethod.hpp>
#include <InetPacketPayload.hpp>
#include "MacLayer.h"
#include "PhyLayer.h"
#include "LinkLayerLifecycleManager.h"

using namespace inet::physicallayer;
using namespace TUHH_INTAIRNET_RLC;
using namespace TUHH_INTAIRNET_ARQ;
//coutd.setVerbose(false);

Define_Module(IntAirNetLinkLayer);


IntAirNetLinkLayer::~IntAirNetLinkLayer() {
}

void IntAirNetLinkLayer::finish() {
    delete ((Rlc*)this->rlcSubLayer);
    //delete this->macSubLayer;
    //cancelAndDelete(subLayerTimerMessage);
}

void IntAirNetLinkLayer::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        slotDuration = par("slotDuration");

        mcsotdma_statistics_map.clear();
        for (size_t i = 0; i < str_mcsotdma_statistics.size(); i++) {
            const std::string& s = str_mcsotdma_statistics.at(i);
            mcsotdma_statistics_map[s] = registerSignal(s.c_str());
        }

        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");

    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        auto radio = check_and_cast<inet::physicallayer::IRadio *>(radioModule);
        radio->setRadioMode(inet::physicallayer::IRadio::RADIO_MODE_TRANSCEIVER);

        // Configure layer:                
        // which contention method to use
        std::string contention_method = par("contentionMethod");
        ContentionMethod method;
        if (contention_method.compare("binomial_estimate") == 0)
            method = ContentionMethod::binomial_estimate;
        else if (contention_method.compare("poisson_binomial_estimate") == 0)
            method = ContentionMethod::poisson_binomial_estimate;
        else if (contention_method.compare("all_active_again_assumption") == 0)
            method = ContentionMethod::all_active_again_assumption;
        else if (contention_method.compare("naive_random_access") == 0)
            method = ContentionMethod::naive_random_access;
        else
            throw std::invalid_argument("contentionMethod is invalid, it should be one of 'binomial_estimate', 'poisson_binomial_estimate', 'all_active_again_assumption'.");
        macSubLayer->setContentionMethod(method);
        macSubLayer->setBcSlotSelectionMinNumCandidateSlots(par("broadcastSlotSelectionMinNumCandidateSlots"));
        macSubLayer->setAlwaysScheduleNextBroadcastSlot(par("alwaysAdvertiseNextBroadcastSlot"));
        macSubLayer->setMinBeaconOffset(par("minBeaconInterval"));
        macSubLayer->setMaxBeaconOffset(par("maxBeaconInterval"));
        macSubLayer->setBroadcastTargetCollisionProb(par("broadcastTargetCollisionRate"));
        // for Konrad :*
        // macSubLayer->setOmnetPassUpBeaconFct(std::function<void (MacId origin_id, L2HeaderBeacon header)> func)

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        lifecycleManager = getModuleFromPar<LinkLayerLifecycleManager>(par("lifecycleManager"), this);
        configureInterfaceEntry();
        MacAddress address = interfaceEntry->getMacAddress();
        uint32_t planning_horizon = par("planningHorizon");
        uint64_t center_frequency1 = 1000, center_frequency2 = 2000, center_frequency3 = 3000, bc_frequency = 4000, bandwidth = 500;


        rlcSubLayer = new Rlc(1600);
        arqSubLayer = new SelectiveRepeatArq(100, 100, par("data_per"));
        macSubLayer = new MacLayer(MacId(address.getInt()), planning_horizon);
        phySubLayer = new PhyLayer(planning_horizon);

        auto reservation_manager = ((MacLayer*)macSubLayer)->getReservationManager();


        reservation_manager->setTransmitterReservationTable(((PhyLayer*)phySubLayer)->getTransmitterReservationTable());
        for (ReservationTable*& table : ((PhyLayer*)phySubLayer)->getReceiverReservationTables()) {
            reservation_manager->addReceiverReservationTable(table);
        }
        reservation_manager->addFrequencyChannel(false, bc_frequency, bandwidth);
        reservation_manager->addFrequencyChannel(true, center_frequency1, bandwidth);
        reservation_manager->addFrequencyChannel(true, center_frequency2, bandwidth);
        reservation_manager->addFrequencyChannel(true, center_frequency3, bandwidth);


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
        ((PassThroughArq*)arqSubLayer)->registerGetTimeCallback(getTimeFkt);
        ((MacLayer*)macSubLayer)->registerGetTimeCallback(getTimeFkt);

        // Schedule At
        ((Rlc*)rlcSubLayer)->registerScheduleAtCallback([this](double time){
            this->addCallback((IOmnetPluggable*)this->rlcSubLayer, time);
        });
        ((PassThroughArq*)arqSubLayer)->registerScheduleAtCallback([this](double time){
            this->addCallback((IOmnetPluggable*)this->arqSubLayer, time);
        });
        ((MacLayer*)macSubLayer)->registerScheduleAtCallback([this](double time){
            this->addCallback((IOmnetPluggable*)this->macSubLayer, time);
        });

        // Emit statistic
        function<void(string, double)> emitFkt = [this](string message, double value){
            this->emitStatistic(message, value);
        };
        ((Rlc*)rlcSubLayer)->registerEmitEventCallback(emitFkt);
        ((SelectiveRepeatArq*)arqSubLayer)->registerEmitEventCallback(emitFkt);
        ((MacLayer*)macSubLayer)->registerEmitEventCallback(emitFkt);
        ((PhyLayer*)phySubLayer)->registerEmitEventCallback(emitFkt);

        // Delete packets
        function<void(L2Packet*)> deleteFkt = [this](L2Packet* pkt){
            this->onPacketDelete(pkt);
        };
        ((Rlc*)rlcSubLayer)->registerDeleteL2Callback(deleteFkt);
        ((SelectiveRepeatArq*)arqSubLayer)->registerDeleteL2Callback(deleteFkt);
        ((MacLayer*)macSubLayer)->registerDeleteL2Callback(deleteFkt);
        ((PhyLayer*)phySubLayer)->registerDeleteL2Callback(deleteFkt);


        // Deep Copy Packets
        function<L2Packet*(L2Packet*)> copyFkt = [this](L2Packet* pkt) {
            return this->copyL2Packet(pkt);
        };
        ((SelectiveRepeatArq*)arqSubLayer)->registerCopyL2Callback(copyFkt);

        // Deep Copy Packet Payloads
        function<L2Packet::Payload*(L2Packet::Payload*)> copyPayloadFkt = [this](L2Packet::Payload* payload) {
            return this->copyL2PacketPayload(payload);
        };
        ((SelectiveRepeatArq*)arqSubLayer)->registerCopyL2PayloadCallback(copyPayloadFkt);



        lifecycleManager->registerClient(this);
    }

}

void IntAirNetLinkLayer::sendUp(cMessage *message)
{
    send(message, upperLayerOutGateId);
}

void IntAirNetLinkLayer::sendDown(cMessage *message)
{
    send(message, lowerLayerOutGateId);

}

void IntAirNetLinkLayer::configureInterfaceEntry()
{
    interfaceEntry = getContainingNicModule(this);
    MacAddress address = MacAddress::generateAutoAddress();

    // data rate
    interfaceEntry->setDatarate(10);

    // generate a link-layer address to be used as interface token for IPv6
    interfaceEntry->setMacAddress(address);
    interfaceEntry->setInterfaceToken(address.formInterfaceIdentifier());

    //TODO: set high enough so that IP does not fragment
    interfaceEntry->setMtu(1500);

    // capabilities
    interfaceEntry->setMulticast(true);
    interfaceEntry->setBroadcast(true);
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
    IntAirNetLinkLayerPacket* pkt = (IntAirNetLinkLayerPacket*)packet;
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

void IntAirNetLinkLayer::handleSelfMessage(cMessage *message) {
}

void IntAirNetLinkLayer::handleMessageWhenDown(cMessage *msg) {

}

void IntAirNetLinkLayer::handleStartOperation(LifecycleOperation *operation){

}

void IntAirNetLinkLayer::handleStopOperation(LifecycleOperation *operation) {

}

void IntAirNetLinkLayer::handleCrashOperation(LifecycleOperation *operation) {

}

bool IntAirNetLinkLayer::isUpperMessage(cMessage *message)
{
    return message->getArrivalGateId() == upperLayerInGateId;
}

bool IntAirNetLinkLayer::isLowerMessage(cMessage *message)
{
    return message->getArrivalGateId() == lowerLayerInGateId;
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
    if(packet->original == nullptr) {
        return;
    }
    Packet* original = (Packet*)packet->original;

    //delete packet->original;
    if(original) {
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
    }
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
    try {
        ((MacLayer*)macSubLayer)->update(1);
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::beforeSlotStart: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::onSlotStart() {
    Enter_Method_Silent();
    try {
        ((MacLayer*)macSubLayer)->execute();
    } catch (const std::exception& e) {
        std::cerr << "Exception in IntAirNetLinkLayer::onSlotStart: " << e.what() << std::endl;
        throw e;
    }
}

void IntAirNetLinkLayer::onSlotEnd() {
    Enter_Method_Silent();
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

L2Packet* IntAirNetLinkLayer::copyL2Packet(L2Packet* original) {
    auto packet = original->copy();
    auto originalPayloads = original->getPayloads();
    auto newHeaders = original->getHeaders();
    auto newPayloads = packet->getPayloads();
    int i = 0;
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





