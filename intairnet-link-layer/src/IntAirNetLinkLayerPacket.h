/*
 * IntAirNetLinkLayerPacket.h
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#ifndef INTAIRNETLINKLAYERPACKET_H_
#define INTAIRNETLINKLAYERPACKET_H_

#include "inet/common/packet/Packet.h"
#include <L2Packet.hpp>

using namespace inet;
using namespace TUHH_INTAIRNET_MCSOTDMA;

class IntAirNetLinkLayerPacket : public Packet {
private:
    L2Packet* containedPacket = nullptr;

public:
    IntAirNetLinkLayerPacket(const char *name, const Ptr<const Chunk>& content);
    void attachPacket(L2Packet* packet);
    L2Packet* getContainedPacket() const;
    int destId = 2;
    uint64_t center_frequency = 0;

    Packet *dup() const override;

    ~IntAirNetLinkLayerPacket();
};



#endif /* INTAIRNETLINKLAYERPACKET_H_ */
