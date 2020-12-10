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
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "../../glue-lib-headers/PassThroughRlc.hpp"
#include "../../glue-lib-headers/DelayMac.hpp"
#include "../../glue-lib-headers/PassThroughArq.hpp"
#include "../../glue-lib-headers/L3Packet.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"

using namespace inet::physicallayer;

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

        //std::function get_time_callback = std::bind(simTime, this);

        rlcSubLayer = new PassThroughRlc();
        arqSublayer = new PassThroughArq();
        macSublayer = new DelayMac(MacId(1));

        rlcSubLayer->setLowerLayer(arqSublayer);
        arqSublayer->setUpperLayer(rlcSubLayer);
        arqSublayer->setLowerLayer(macSublayer);
        macSublayer->setUpperLayer(arqSublayer);
        macSublayer->setLowerLayer((IPhy*)this);





        /** Register callback functions **/
        // GetTime
        ((PassThroughRlc*)rlcSubLayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });
        ((PassThroughArq*)arqSublayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });
        ((DelayMac*)macSublayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });

        // Debug Messages
        ((PassThroughRlc*)rlcSubLayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });
        ((PassThroughArq*)arqSublayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });
        ((DelayMac*)macSublayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });

        // Debug Messages
        ((PassThroughRlc*)rlcSubLayer)->registerScheduleAtCallback([this](double time){
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
        ((PassThroughRlc*)rlcSubLayer)->registerEmitEventCallback([this](string message, double value){
                    EV << "TEST " << message << " " << value << endl;
        });

        //((PassThroughRlc*)rlcSubLayer)->init();
        //((PassThroughRlc*)rlcSubLayer)->init();



    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        auto radio = check_and_cast<IRadio *>(radioModule);
        radio->setRadioMode(IRadio::RADIO_MODE_TRANSCEIVER);
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

void IntAirNetLinkLayer::handleUpperPacket(Packet *packet) {
    L3Packet* int_air_net_packet = PacketFactory::fromInetPacket(packet);
    rlcSubLayer->receiveFromUpper(int_air_net_packet, MacId(10));
    auto pkt = new IntAirNetLinkLayerPacket();
    auto tags = packet->getTags();

    for(int i = 0; i< packet->getNumTags(); i++) {
        EV << "TAG " << i<< ": "<< tags.getTag(i)->getClassName() << endl;
    }
    pkt->copyTags(*packet);

    //auto macAddressInd = packet->addTagIfAbsent<MacAddressInd>();
    //macAddressInd->setSrcAddress(Mac);
    //macAddressInd->setDestAddress(macHeader->getDest());

    pkt->getTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);

    //pkt->addTag<PacketProtocolTag>()->setProtocol(protocol);
    //packet->addTag<DispatchProtocolReq>()->setProtocol(addressType->getNetworkProtocol());
    //packet->addTag<L3AddressReq>()->setDestAddress(destAddr);

    // referr from sending down now
    //sendDown(pkt);
}

void IntAirNetLinkLayer::handleLowerPacket(Packet *packet) {
    sendUp(packet);
}

void IntAirNetLinkLayer::handleSelfMessage(cMessage *message) {

    if(message == subLayerTimerMessage) {
        EV <<  "My time has come" << endl;
        ((DelayMac*)macSublayer)->onEvent(0);
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

void IntAirNetLinkLayer::addCallback(IOmnetPluggable * layer, double time) {
    EV << "HELLO" << time;
    if(!subLayerTimerMessage->isScheduled()) {
        scheduleAt(SimTime(time), subLayerTimerMessage);
    }
}


void IntAirNetLinkLayer::receiveFromUpper(L2Packet* data, unsigned int center_frequency)  {
    auto pkt = PacketFactory::fromL2Packet(data);
    // SET TAGS
    EV << "send down" << endl;

    sendDown(pkt);

}

unsigned long IntAirNetLinkLayer::getCurrentDatarate() const {
    return 1;
}





