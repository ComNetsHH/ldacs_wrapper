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
