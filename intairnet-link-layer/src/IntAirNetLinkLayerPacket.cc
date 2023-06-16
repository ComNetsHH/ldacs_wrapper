// The L-Band Digital Aeronautical Communications System (LDACS) Wrapper Library integrates the LDACS Air-Air Medium Access Control simulator into OMNeT++.
// Copyright (C) 2023  Sebastian Lindner, Konrad Fuger, Musab Ahmed Eltayeb Ahmed, Andreas Timm-Giel, Institute of Communication Networks, Hamburg University of Technology, Hamburg, Germany
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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



