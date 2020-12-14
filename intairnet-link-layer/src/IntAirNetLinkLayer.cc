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
#include "../../glue-lib-headers/DelayMac.hpp"
#include "../../glue-lib-headers/PassThroughArq.hpp"
#include "../../glue-lib-headers/L3Packet.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"

using namespace inet::physicallayer;
using namespace TUHH_INTAIRNET_RLC;

Define_Module(IntAirNetLinkLayer);

IntAirNetLinkLayer::~IntAirNetLinkLayer() {
}

void IntAirNetLinkLayer::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        subLayerTimerMessage = new cMessage("subLayerTimer");


        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");

        rlcSubLayer = new Rlc();
        arqSublayer = new PassThroughArq();
        macSublayer = new DelayMac(MacId(1));

        rlcSubLayer->setLowerLayer(arqSublayer);
        rlcSubLayer->setUpperLayer((INet*)this);
        arqSublayer->setUpperLayer(rlcSubLayer);
        arqSublayer->setLowerLayer(macSublayer);
        macSublayer->setUpperLayer(arqSublayer);
        macSublayer->setLowerLayer((IPhy*)this);

        /** Register callback functions **/
        // GetTime
        function<double()> getTimeFkt= [this]{
            return simTime().dbl();
        };
        ((Rlc*)rlcSubLayer)->registerGetTimeCallback(getTimeFkt);
        ((PassThroughArq*)arqSublayer)->registerGetTimeCallback(getTimeFkt);
        ((DelayMac*)macSublayer)->registerGetTimeCallback(getTimeFkt);

        // Debug Messages
        function<void(string)> debugFkt = [this](string message){
            EV << "DEBUG: " << message << endl;
        };
        ((Rlc*)rlcSubLayer)->registerDebugMessageCallback(debugFkt);
        ((PassThroughArq*)arqSublayer)->registerDebugMessageCallback(debugFkt);
        ((DelayMac*)macSublayer)->registerDebugMessageCallback(debugFkt);

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
        ((DelayMac*)macSublayer)->registerScheduleAtCallback([this](double time){
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
        ((DelayMac*)macSublayer)->registerEmitEventCallback(emitFkt);



    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        auto radio = check_and_cast<IRadio *>(radioModule);
        radio->setRadioMode(IRadio::RADIO_MODE_TRANSCEIVER);

    } else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        configureInterfaceEntry();

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

    auto tags = packet->getTags();
    for(int i = 0; i< packet->getNumTags(); i++) {
        EV << "TAG " << i<< ": "<< tags.getTag(i)->getClassName() << endl;
    }

    //TODO: resolve MAc ID
    macSublayer->receiveFromLower(containedPacket, MacId(2));
    delete packet;
}

void IntAirNetLinkLayer::handleSelfMessage(cMessage *message) {

    if(message == subLayerTimerMessage) {
        for(auto it = callbackTimes.begin(); it != callbackTimes.end(); it++) {
            double time = it->first;
            if(time == simTime().dbl()) {

                EV << "TIME" << endl;

                ((DelayMac*)macSublayer)->onEvent(time);
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
void IntAirNetLinkLayer::receiveFromUpper(L2Packet* data, unsigned int center_frequency)  {
    auto pkt = PacketFactory::fromL2Packet(data);
    pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);

    // TODO: remove quickfix for copying tags
    pkt->copyTags(*tmp);

    sendDown(pkt);

}

unsigned long IntAirNetLinkLayer::getCurrentDatarate() const {
    return 1;
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
}





