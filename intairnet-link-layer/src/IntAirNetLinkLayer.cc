/*
 * IntAirNetLinkLayer.cc
 *
 *  Created on: 5 Dec 2020
 *      Author: fu
 */

#include <functional>
#include "IntAirNetLinkLayer.h"
#include "IntAirNetLinkLayerPacket.h"
#include "PacketFactory.h"
#include "inet/common/ModuleAccess.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/common/ProtocolGroup.h"
#include "../../avionic-rlc-headers/Rlc.hpp"
#include "../../glue-lib-headers/PassThroughArq.hpp"
#include "../../glue-lib-headers/L3Packet.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"
#include "MacLayer.h"
#include "PhyLayer.h"

using namespace inet::physicallayer;
using namespace TUHH_INTAIRNET_RLC;

Define_Module(IntAirNetLinkLayer);


IntAirNetLinkLayer::~IntAirNetLinkLayer() {
}

void IntAirNetLinkLayer::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        slotDuration = par("slotDuration");

        rlc_bits_received_from_upper_signal = registerSignal("rlc_bits_received_from_upper");
        rlc_bits_received_from_lower_signal = registerSignal("rlc_bits_received_from_lower");

        subLayerTimerMessage = new cMessage("subLayerTimer");
        slotTimerMessage = new cMessage("slotTimer");
        scheduleAt(slotDuration, slotTimerMessage);


        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");

    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        auto radio = check_and_cast<inet::physicallayer::IRadio *>(radioModule);
        radio->setRadioMode(inet::physicallayer::IRadio::RADIO_MODE_TRANSCEIVER);

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        configureInterfaceEntry();
        MacAddress address = interfaceEntry->getMacAddress();
        uint32_t planning_horizon = 256;
        uint64_t center_frequency1 = 1000, center_frequency2 = 2000, center_frequency3 = 3000, bc_frequency = 4000, bandwidth = 500;

                rlcSubLayer = new Rlc();
                arqSublayer = new PassThroughArq();
                macSublayer = new MacLayer(MacId(address.getInt()), planning_horizon);
                phySubLayer = new PhyLayer(planning_horizon);


                ((MacLayer*)macSublayer)->getReservationManager()->setPhyTransmitterTable(((PhyLayer*)phySubLayer)->getTransmitterReservationTable());
                ((MacLayer*)macSublayer)->getReservationManager()->addFrequencyChannel(false, bc_frequency, bandwidth);
                ((MacLayer*)macSublayer)->getReservationManager()->addFrequencyChannel(true, center_frequency1, bandwidth);
                ((MacLayer*)macSublayer)->getReservationManager()->addFrequencyChannel(true, center_frequency2, bandwidth);
                ((MacLayer*)macSublayer)->getReservationManager()->addFrequencyChannel(true, center_frequency3, bandwidth);


                rlcSubLayer->setLowerLayer(arqSublayer);
                rlcSubLayer->setUpperLayer((INet*)this);
                arqSublayer->setUpperLayer(rlcSubLayer);
                arqSublayer->setLowerLayer(macSublayer);
                macSublayer->setUpperLayer(arqSublayer);
                macSublayer->setLowerLayer(phySubLayer);
                phySubLayer->setUpperLayer(macSublayer);
                phySubLayer->setRadio((IRadio*)this);

                /** Register callback functions **/
                // GetTime
                function<double()> getTimeFkt= [this]{
                    return simTime().dbl();
                };
                ((Rlc*)rlcSubLayer)->registerGetTimeCallback(getTimeFkt);
                ((PassThroughArq*)arqSublayer)->registerGetTimeCallback(getTimeFkt);
                ((MacLayer*)macSublayer)->registerGetTimeCallback(getTimeFkt);

                // Debug Messages
                function<void(string)> debugFkt = [this](string message){
                    EV << "DEBUG: " << message << endl;
                };
                ((Rlc*)rlcSubLayer)->registerDebugMessageCallback(debugFkt);
                ((PassThroughArq*)arqSublayer)->registerDebugMessageCallback(debugFkt);
                ((MacLayer*)macSublayer)->registerDebugMessageCallback(debugFkt);
                ((PhyLayer*)phySubLayer)->registerDebugMessageCallback(debugFkt);

                // Schedule At
                ((Rlc*)rlcSubLayer)->registerScheduleAtCallback([this](double time){
                    EV << "Schedule AT: " << time << endl;
                    this->addCallback((IOmnetPluggable*)this->rlcSubLayer, time);
                });
                ((PassThroughArq*)arqSublayer)->registerScheduleAtCallback([this](double time){
                    EV << "Schedule AT: " << time << endl;
                    this->addCallback((IOmnetPluggable*)this->arqSublayer, time);
                    //EV << "DEBUG: " << message << endl;
                });
                ((MacLayer*)macSublayer)->registerScheduleAtCallback([this](double time){
                    EV << "Schedule AT: " << time << endl;
                    this->addCallback((IOmnetPluggable*)this->macSublayer, time);
                    //EV << "DEBUG: " << message << endl;
                });


                // Emit statistic
                function<void(string, double)> emitFkt = [this](string message, double value){
                    this->emitStatistic(message, value);
                };
                ((Rlc*)rlcSubLayer)->registerEmitEventCallback(emitFkt);
                ((PassThroughArq*)arqSublayer)->registerEmitEventCallback(emitFkt);
                ((MacLayer*)macSublayer)->registerEmitEventCallback(emitFkt);

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

    // LOG ALL TAGS
    auto tags = packet->getTags();
    for(int i = 0; i< packet->getNumTags(); i++) {
        EV << "TAG " << i<< ": "<< tags.getTag(i)->getClassName() << endl;
    }

    // TODO: handle tags better
    tmp = packet;


    L3Packet* int_air_net_packet = PacketFactory::fromInetPacket(packet);
    auto macAddressReq = packet->getTag<MacAddressReq>();
    MacAddress address = macAddressReq->getDestAddress();
    rlcSubLayer->receiveFromUpper(int_air_net_packet, MacId(address.getInt()));
}

void IntAirNetLinkLayer::handleLowerPacket(Packet *packet) {
    IntAirNetLinkLayerPacket* pkt = (IntAirNetLinkLayerPacket*)packet;
    L2Packet* containedPacket = pkt->getContainedPacket();
    auto center_frequency = pkt->center_frequency;

    auto tags = packet->getTags();
    for(int i = 0; i< packet->getNumTags(); i++) {
        EV << "TAG " << i<< ": "<< tags.getTag(i)->getClassName() << endl;
    }

    //TODO: resolve MAc ID
    //phySubLayer->receive(containedPacket, MacId(2));
    phySubLayer->onReception(containedPacket, center_frequency);
    delete packet;
}

void IntAirNetLinkLayer::handleSelfMessage(cMessage *message) {

    if(message == slotTimerMessage) {
        ((MacLayer*)macSublayer)->update(1);
        ((MacLayer*)macSublayer)->execute();
        scheduleAt(simTime() + slotDuration, slotTimerMessage);
    }

    if(message == subLayerTimerMessage) {
        for(auto it = callbackTimes.begin(); it != callbackTimes.end(); it++) {
            double time = it->first;
            if(time == simTime().dbl()) {

                EV << "TIME" << endl;

                ((MacLayer*)macSublayer)->onEvent(time);
                callbackTimes.erase(it);
            }
        }
        double min_time = -1;
        for(auto it = callbackTimes.begin(); it != callbackTimes.end(); it++) {
            double time = it->first;
            if(min_time < 0) {
                min_time = time;
            } else if(time < min_time) {
                min_time = time;
            }
        }
        if(!subLayerTimerMessage->isScheduled()) {
            scheduleAt(min_time, subLayerTimerMessage);
        }
    }

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
    callbackTimes.push_back(make_pair(time, layer));
    double min_time = -1;
    for(auto it = callbackTimes.begin(); it != callbackTimes.end(); it++) {
        double time = it->first;
        if(min_time < 0) {
            min_time = time;
        } else if(time < min_time) {
            min_time = time;
        }
    }
    if(!subLayerTimerMessage->isScheduled()) {
        scheduleAt(min_time, subLayerTimerMessage);
    }
}


// Pretend to be PHY and get packet from MAC
void IntAirNetLinkLayer::sendToChannel(L2Packet* data, uint64_t center_frequency)  {
    auto pkt = PacketFactory::fromL2Packet(data, center_frequency);
    pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);

    // TODO: remove quickfix for copying tags
    if(tmp != nullptr) {
        // pkt->copyTags(*tmp);
    }

    sendDown(pkt);

}

void IntAirNetLinkLayer::receiveFromLower(L3Packet* packet) {
    Packet* original = packet->original;
    if(original) {
        take(original);

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
    EV << statistic_name << ": " << value << endl;
    if(statistic_name == "Rlc:packet_received_from_upper(bits)") {
        emit(rlc_bits_received_from_upper_signal, value);
    }
    if(statistic_name == "Rlc:packet_received_from_lower(bits)") {
        emit(rlc_bits_received_from_lower_signal, value);
    }
}





