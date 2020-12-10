/*
 * IntAirNetLinkLayerPacket.cc
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#include "IntAirNetLinkLayerPacket.h"

void IntAirNetLinkLayerPacket::attachPacket(L2Packet* packet) {
    containedPacket = packet;
    // we may set some field like size, attach a dummy header etc...
}

L2Packet* IntAirNetLinkLayerPacket::getContainedPacket() {
    return containedPacket;
}



