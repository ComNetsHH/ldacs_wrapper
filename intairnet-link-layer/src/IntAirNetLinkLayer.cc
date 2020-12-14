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

        //std::function get_time_callback = std::bind(simTime, this);

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
        ((Rlc*)rlcSubLayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });
        ((PassThroughArq*)arqSublayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });
        ((DelayMac*)macSublayer)->registerGetTimeCallback([this]{
            return simTime().dbl();
        });

        // Debug Messages
        ((Rlc*)rlcSubLayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });
        ((PassThroughArq*)arqSublayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });
        ((DelayMac*)macSublayer)->registerDebugMessageCallback([this](string message){
            EV << "DEBUG: " << message << endl;
        });

        // Debug Messages
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
        ((Rlc*)rlcSubLayer)->registerEmitEventCallback([this](string message, double value){
                    EV << "TEST " << message << " " << value << endl;
        });

        //((PassThroughRlc*)rlcSubLayer)->init();
        //((PassThroughRlc*)rlcSubLayer)->init();



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

    // MTU: typical values are 576 (Internet de facto), 1500 (Ethernet-friendly),
    // 4000 (on some point-to-point links), 4470 (Cisco routers default, FDDI compatible)
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

    tmp = packet;


    L3Packet* int_air_net_packet = PacketFactory::fromInetPacket(packet);
    auto macAddressReq = packet->getTag<MacAddressReq>();
    MacAddress address = macAddressReq->getDestAddress();
    rlcSubLayer->receiveFromUpper(int_air_net_packet, MacId(address.getInt()));

    // DONE
    //auto pkt = new IntAirNetLinkLayerPacket();
    //pkt->copyTags(*packet);

    //auto macAddressInd = packet->addTagIfAbsent<MacAddressInd>();
    //macAddressInd->setSrcAddress(Mac);
    //macAddressInd->setDestAddress(macHeader->getDest());

    //pkt->getTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);

    //pkt->addTag<PacketProtocolTag>()->setProtocol(protocol);
    //packet->addTag<DispatchProtocolReq>()->setProtocol(addressType->getNetworkProtocol());
    //packet->addTag<L3AddressReq>()->setDestAddress(destAddr);

    // referr from sending down now
    //sendDown(pkt);
}

void IntAirNetLinkLayer::handleLowerPacket(Packet *packet) {
    EV << "GOT IT, THANKS " << *packet << endl;
    EV << "SIZE " << packet->getTotalLength() << endl;
    IntAirNetLinkLayerPacket* pkt = (IntAirNetLinkLayerPacket*)packet;
    L2Packet* containedPacket = pkt->getContainedPacket();

    EV << "### "<< containedPacket << endl;
    EV << "### "<< pkt->x << endl;

    // EV << containedPacket->getBits() << endl;
    EV << containedPacket->print() << endl;

    auto tags = packet->getTags();
    for(int i = 0; i< packet->getNumTags(); i++) {
        EV << "TAG " << i<< ": "<< tags.getTag(i)->getClassName() << endl;
    }

    // resolve MAc ID
    macSublayer->receiveFromLower(containedPacket, MacId(2));
    //sendUp(packet);
}

void IntAirNetLinkLayer::handleSelfMessage(cMessage *message) {

    if(message == subLayerTimerMessage) {
        EV <<  "My time has come" << endl;
        ((DelayMac*)macSublayer)->onEvent(simTime().dbl());
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


// Pretend to be PHY and get packet from MAC
void IntAirNetLinkLayer::receiveFromUpper(L2Packet* data, unsigned int center_frequency)  {
    auto pkt = PacketFactory::fromL2Packet(data);
    // SET TAGS
    pkt->addTag<PacketProtocolTag>()->setProtocol(&Protocol::ackingMac);

    pkt->copyTags(*tmp);
    auto tst = pkt->getContainedPacket();
    EV << tst->print() << endl;
    EV << "send down" << endl;

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





