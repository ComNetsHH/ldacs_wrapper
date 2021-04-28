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


IntAirNetLinkLayerPacket* PacketFactory::fromL2Packet(L2Packet* source, uint64_t center_frequency) {
    unsigned int size_bits = source->getBits();

    unsigned int size_bytes = std::ceil(size_bits / 8);
    auto data = makeShared<ByteCountChunk>(B(size_bytes));
    IntAirNetLinkLayerPacket* pkt = new IntAirNetLinkLayerPacket("IntAirNetLinkLayerPacket", data);
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
    // TODO: translate all relevant fields
}



