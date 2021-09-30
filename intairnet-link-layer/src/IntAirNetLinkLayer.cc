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
#include <L3Packet.hpp>
#include <IOmnetPluggable.hpp>
#include <ContentionMethod.hpp>
#include <InetPacketPayload.hpp>
#include "MacLayer.h"
#include "PhyLayer.h"
#include "LinkLayerLifecycleManager.h"

using namespace inet::physicallayer;
using namespace TUHH_INTAIRNET_RLC;
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
        // target collision probability
        double bc_target_collision_prob = par("broadcastTargetCollisionRate");
        macSubLayer->setBroadcastTargetCollisionProb(bc_target_collision_prob);
        // minimum number of candidate slots during slot selection
        int min_bc_candidate_slots = par("broadcastSlotSelectionMinNumCandidateSlots");
        macSubLayer->setBcSlotSelectionMinNumCandidateSlots(min_bc_candidate_slots);
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

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        lifecycleManager = getModuleFromPar<LinkLayerLifecycleManager>(par("lifecycleManager"), this);
        configureInterfaceEntry();
        MacAddress address = interfaceEntry->getMacAddress();
        uint32_t planning_horizon = 256;
        uint64_t center_frequency1 = 1000, center_frequency2 = 2000, center_frequency3 = 3000, bc_frequency = 4000, bandwidth = 500;

        rlcSubLayer = new Rlc(1600);
        arqSubLayer = new PassThroughArq();
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

//                // Debug Messages
//                function<void(string)> debugFkt = [this](string message){
//                    cout << endl << message << endl;
//                    EV << "DEBUG: " << message << endl;
//                };
//                ((Rlc*)rlcSubLayer)->registerDebugMessageCallback(debugFkt);
//                ((PassThroughArq*)arqSublayer)->registerDebugMessageCallback(debugFkt);
//                ((MacLayer*)macSublayer)->registerDebugMessageCallback(debugFkt);
//                ((PhyLayer*)phySubLayer)->registerDebugMessageCallback(debugFkt);

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
        ((PassThroughArq*)arqSubLayer)->registerEmitEventCallback(emitFkt);
        ((MacLayer*)macSubLayer)->registerEmitEventCallback(emitFkt);
        ((PhyLayer*)phySubLayer)->registerEmitEventCallback(emitFkt);

        // Emit statistic
        function<void(L2Packet*)> deleteFkt = [this](L2Packet* pkt){
            this->onPacketDelete(pkt);
        };
        ((Rlc*)rlcSubLayer)->registerDeleteCallback(deleteFkt);
        //((PassThroughArq*)arqSubLayer)->registerDeleteCallback(deleteFkt);
        ((MacLayer*)macSubLayer)->registerDeleteCallback(deleteFkt);
        ((PhyLayer*)phySubLayer)->registerDeleteCallback(deleteFkt);

        lifecycleManager->registerClient(this);

        double bc_target_collision_prob = par("broadcastTargetCollisionRate");
        macSubLayer->setBroadcastTargetCollisionProb(bc_target_collision_prob);
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
    // TODO: handle tags better
    tmp = packet;

    L3Packet* int_air_net_packet = PacketFactory::fromInetPacket(packet);
    auto macAddressReq = packet->getTag<MacAddressReq>();
    MacAddress address = macAddressReq->getDestAddress();
    auto destination_mac_id = MacId(address.getInt());

    if(address.isBroadcast()) {
        destination_mac_id = SYMBOLIC_LINK_ID_BROADCAST;
    }

    rlcSubLayer->receiveFromUpper(int_air_net_packet, destination_mac_id);
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

    phySubLayer->onReception(containedPacket, center_frequency);
    pkt->attachPacket(nullptr);
    delete pkt;
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
    sendDown(pkt);

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
        sendUp(original);
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
    ((MacLayer*)macSubLayer)->update(1);
}

void IntAirNetLinkLayer::onSlotStart() {
    Enter_Method_Silent();
    ((MacLayer*)macSubLayer)->execute();
}

void IntAirNetLinkLayer::onSlotEnd() {
    Enter_Method_Silent();
    ((MacLayer*)macSubLayer)->onSlotEnd();
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






