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

#include "PacketFactory.h"

#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/Packet.h"
#include <cmath>


using namespace TUHH_INTAIRNET_MCSOTDMA;

IntAirNetLinkLayerPacket* PacketFactory::fromL2Packet(L2Packet* source, uint64_t center_frequency) {
    float size_bits = (float)source->getBits();
    unsigned int size_bytes = std::ceil(size_bits / 8.0);
    auto data = makeShared<ByteCountChunk>(B(size_bytes));
    IntAirNetLinkLayerPacket* pkt = new IntAirNetLinkLayerPacket(PacketFactory::getPacketName(source).c_str(), data);
    pkt->attachPacket(source);
    pkt->center_frequency = center_frequency;
    pkt->destId = source->getDestination().getId();

    return pkt;
    // TODO: translate all relevant fields
}

L3Packet* PacketFactory::fromInetPacket(Packet* source) {
    L3Packet* pkt = new L3Packet();
    pkt->original = source;

    B size = source->getTotalLength();
    pkt->size = (size.get() * 8);

    return pkt;
}

string PacketFactory::getPacketName(L2Packet* packet) {
    auto headers = packet->getHeaders();
    if (headers.size() < 2) {
        return "IAN-Packet";

    }

    for(int i= 0; i< headers.size(); i++) {
        if(headers[i]->frame_type == L2Header::FrameType::link_establishment_request) {
            return "IAN-Link-Request";
        }
    }
    auto header = headers[1];
    
    if(header->frame_type == L2Header::FrameType::broadcast) {
        return "IAN-Broadcast";
    }
    if(header->frame_type == L2Header::FrameType::unicast) {
        return "IAN-Unicast";
    }
    if(header->frame_type == L2Header::FrameType::link_establishment_request) {
        return "IAN-Link-Request";
    }
    if(header->frame_type == L2Header::FrameType::link_establishment_reply) {
        return "IAN-Link-Reply";
    }    
    return "IAN-Packet";
}


