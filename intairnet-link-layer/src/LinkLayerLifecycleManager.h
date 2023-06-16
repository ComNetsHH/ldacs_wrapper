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

#ifndef LINKLAYERLIFECYCLEMANAGER_H_
#define LINKLAYERLIFECYCLEMANAGER_H_

#include "inet/common/INETDefs.h"
#include "inet/queueing/contract/IPacketQueue.h"
#include "inet/linklayer/base/MacProtocolBase.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/acking/AckingMac.h"
#include <IMac.hpp>
#include "LinkLayer.h"

using namespace inet;
using namespace TUHH_INTAIRNET_MCSOTDMA;

class LinkLayerLifecycleManager: public cSimpleModule {
    protected:
        bool isFirstSlot = true;
        double slotDuration;
        /** LDACS users. */
        std::vector<LinkLayer*> linkLayers;                
        cMessage *slotTimer = nullptr;

        void initialize(int stage) override;
        virtual void handleMessage(cMessage *message) override;
        size_t isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const;


  public:
    LinkLayerLifecycleManager();
    void registerClient(LinkLayer *linkLayer);
    virtual ~LinkLayerLifecycleManager();

    int getRandomInt(int min, int max, int k);

};



#endif /* LINKLAYERLIFECYCLEMANAGER_H_ */
