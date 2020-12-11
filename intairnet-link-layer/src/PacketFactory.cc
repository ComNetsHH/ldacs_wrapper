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


IntAirNetLinkLayerPacket* PacketFactory::fromL2Packet(L2Packet* source) {
    unsigned int size_bits = source->getBits();
    unsigned int size_bytes = std::ceil(size_bits / 8) + 100;
    auto data = makeShared<ByteCountChunk>(B(size_bytes));
    IntAirNetLinkLayerPacket* pkt = new IntAirNetLinkLayerPacket("IntAirNetLinkLayerPacket", data);
    pkt->attachPacket(source);

    return pkt;
    // TODO: translate all relevant fields
}

L3Packet* PacketFactory::fromInetPacket(Packet* source) {
    L3Packet* pkt = new L3Packet();
    pkt->original = source;

    return pkt;
    // TODO: translate all relevant fields
}



