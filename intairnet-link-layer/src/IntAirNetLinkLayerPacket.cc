/*
 * IntAirNetLinkLayerPacket.cc
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#include "IntAirNetLinkLayerPacket.h"

IntAirNetLinkLayerPacket::IntAirNetLinkLayerPacket(const char *name, const Ptr<const Chunk>& content) :Packet(name, content) {

}

void IntAirNetLinkLayerPacket::attachPacket(L2Packet* packet) {
    containedPacket = packet;
    // we may set some field like size, attach a dummy header etc...
}

L2Packet* IntAirNetLinkLayerPacket::getContainedPacket() {
    return containedPacket;
}



