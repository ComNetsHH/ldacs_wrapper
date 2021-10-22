/*
 * PacketFactory.h
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#ifndef PACKETFACTORY_H_
#define PACKETFACTORY_H_

#include "IntAirNetLinkLayerPacket.h"
#include <L2Packet.hpp>
#include <L3Packet.hpp>

using namespace inet;
using namespace TUHH_INTAIRNET_MCSOTDMA;

class PacketFactory {
public:
    static IntAirNetLinkLayerPacket* fromL2Packet(L2Packet* source, uint64_t center_frequency);
    static L3Packet* fromInetPacket(Packet* source);
};



#endif /* PACKETFACTORY_H_ */
