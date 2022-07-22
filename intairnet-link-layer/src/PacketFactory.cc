/*
 * PacketFactory.cc
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#include "PacketFactory.h"

#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/Packet.h"
#include <cmath>


using namespace TUHH_INTAIRNET_MCSOTDMA;

IntAirNetLinkLayerPacket* PacketFactory::fromL2Packet(L2Packet* source, uint64_t center_frequency) {
    unsigned int size_bits = source->getBits();

    unsigned int size_bytes = std::ceil(size_bits / 8);
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


