/*
 * LinkLayerLifecycleManager.h
 *
 *  Created on: 5 Feb 2021
 *      Author: root
 */

#ifndef LINKLAYERLIFECYCLEMANAGER_H_
#define LINKLAYERLIFECYCLEMANAGER_H_

#include "inet/common/INETDefs.h"
#include "inet/queueing/contract/IPacketQueue.h"
#include "inet/linklayer/base/MacProtocolBase.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/linklayer/acking/AckingMac.h"
#include <IMac.hpp>
#include "IntAirNetLinkLayer.h"

using namespace inet;
using namespace TUHH_INTAIRNET_MCSOTDMA;

class LinkLayerLifecycleManager: public cSimpleModule {
    protected:
        bool isFirstSlot = true;
        double slotDuration;
        /** LDACS users. */
        std::vector<IntAirNetLinkLayer *> linkLayers;                
        cMessage *slotTimer = nullptr;

        void initialize(int stage) override;
        virtual void handleMessage(cMessage *message) override;



  public:
    LinkLayerLifecycleManager();
    void registerClient(IntAirNetLinkLayer *linkLayer);
    virtual ~LinkLayerLifecycleManager();

    int getRandomInt(int min, int max, int k);

};



#endif /* LINKLAYERLIFECYCLEMANAGER_H_ */
