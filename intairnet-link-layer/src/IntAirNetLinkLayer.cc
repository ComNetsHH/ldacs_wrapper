/*
 * IntAirNetLinkLayer.cc
 *
 *  Created on: 5 Dec 2020
 *      Author: fu
 */

#include <functional>
#include "IntAirNetLinkLayer.h"
#include "../../glue-lib-headers/PassThroughRlc.h"
#include "../../glue-lib-headers/L3Packet.h"

Define_Module(IntAirNetLinkLayer);

IntAirNetLinkLayer::~IntAirNetLinkLayer() {
}

void IntAirNetLinkLayer::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");

        //std::function get_time_callback = std::bind(simTime, this);

        rlcSubLayer = new PassThroughRlc();
        ((PassThroughRlc*)rlcSubLayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });

        ((PassThroughRlc*)rlcSubLayer)->registerDebugMessageCallback([this](string message){
            EV << "TEST " << message << endl;
        });
        ((PassThroughRlc*)rlcSubLayer)->registerEmitEventCallback([this](string message, double value){
                    EV << "TEST " << message << " " << value << endl;
        });

        ((PassThroughRlc*)rlcSubLayer)->init();
        ((PassThroughRlc*)rlcSubLayer)->init();


    }
}

void IntAirNetLinkLayer::sendUp(cMessage *message)
{
    send(message, upperLayerOutGateId);
}

void IntAirNetLinkLayer::sendDown(cMessage *message)
{
    //send(message, lowerLayerOutGateId);

}

void IntAirNetLinkLayer::handleUpperPacket(Packet *packet) {
    auto int_air_net_packet = new L3Packet();
    EV <<  "HELLO" << endl;
    rlcSubLayer->receiveFromUpper(int_air_net_packet, MacId(10));
}

void IntAirNetLinkLayer::handleLowerPacket(Packet *packet) {
    sendUp(packet);
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





