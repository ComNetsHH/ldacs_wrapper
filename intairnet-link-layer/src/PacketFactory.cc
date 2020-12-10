/*
 * PacketFactory.cc
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#include "PacketFactory.h"


IntAirNetLinkLayerPacket* PacketFactory::fromL2Packet(L2Packet* source) {
    IntAirNetLinkLayerPacket* pkt = new IntAirNetLinkLayerPacket();
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



