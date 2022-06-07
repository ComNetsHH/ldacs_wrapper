/*
 * IntAirNetLinkLayerPacket.cc
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#include "IntAirNetLinkLayerPacket.h"
#include <InetPacketPayload.hpp>

using namespace TUHH_INTAIRNET_MCSOTDMA;

IntAirNetLinkLayerPacket::IntAirNetLinkLayerPacket(const char *name, const Ptr<const Chunk>& content) :Packet(name, content) {

}

void IntAirNetLinkLayerPacket::attachPacket(L2Packet* packet) {
    containedPacket = packet;
    // we may set some field like size, attach a dummy header etc...
}

L2Packet* IntAirNetLinkLayerPacket::getContainedPacket() const {
    return containedPacket;
}

Packet* IntAirNetLinkLayerPacket::dup() const {
    auto pkt = new IntAirNetLinkLayerPacket(*this);
    pkt->center_frequency = this->center_frequency;
    auto containedPkt = this->getContainedPacket()->copy();

    auto originalPayloads = this->getContainedPacket()->getPayloads();
    auto newPayloads = containedPkt->getPayloads();
    auto newHeaders = containedPkt->getHeaders();
    for(int i = 0; i< newHeaders.size(); i++) {
        if(newHeaders[i]->frame_type == L2Header::FrameType::broadcast || newHeaders[i]->frame_type == L2Header::FrameType::unicast) {
            if(originalPayloads[i] != nullptr) {
                if(((InetPacketPayload*)originalPayloads[i])->original) {
                    ((InetPacketPayload*)newPayloads[i])->original = ((InetPacketPayload*)(originalPayloads[i]))->original->dup();
                }

            }
        }
    }


    pkt->attachPacket(containedPkt);
    return (Packet*)pkt;
}

IntAirNetLinkLayerPacket::~IntAirNetLinkLayerPacket() {
    if(!containedPacket) {
        return;
    }
    auto headers = containedPacket->getHeaders();
    auto payloads = containedPacket->getPayloads();
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
    delete containedPacket;

}



