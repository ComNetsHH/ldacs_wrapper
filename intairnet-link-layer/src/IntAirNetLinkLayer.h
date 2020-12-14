#ifndef __INET_INT_AIR_NET_LL_H
#define __INET_INT_AIR_NET_LL_H

#include "inet/common/INETDefs.h"
#include "inet/queueing/contract/IPacketQueue.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/LayeredProtocolBase.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/InterfaceEntry.h"

#include "../../glue-lib-headers/IRlc.hpp"
#include "../../glue-lib-headers/IMac.hpp"
#include "../../glue-lib-headers/IArq.hpp"
#include "../../glue-lib-headers/IPhy.hpp"
#include "../../glue-lib-headers/INet.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"

using namespace inet;
using namespace std;

using namespace TUHH_INTAIRNET_MCSOTDMA;


/** @brief
 * Interface implementation of the RLC layer. It defines all common function needed to implement a RLC
 *
 *    @author Konrad Fuger, TUHH ComNets
 *    @date August 2020
 *
 */
// public INet
class IntAirNetLinkLayer: public LayeredProtocolBase, public IPhy, public INet {
protected:

    IRlc* rlcSubLayer;
    IArq* arqSublayer;
    IMac* macSublayer;

    cMessage* subLayerTimerMessage = nullptr;
    Packet *tmp;

    InterfaceEntry *interfaceEntry = nullptr;

    ~IntAirNetLinkLayer();

    int upperLayerInGateId = -1;
    int upperLayerOutGateId = -1;
    int lowerLayerInGateId = -1;
    int lowerLayerOutGateId = -1;

    void initialize(int stage) override;
    void sendUp(cMessage *message);
    void sendDown(cMessage *message);

    void handleMessageWhenDown(cMessage *msg) override;
    void handleStartOperation(LifecycleOperation *operation) override;
    void handleStopOperation(LifecycleOperation *operation) override;
    void handleCrashOperation(LifecycleOperation *operation) override;

    bool isInitializeStage(int stage) override { return stage == INITSTAGE_LINK_LAYER; }
    bool isModuleStartStage(int stage) override { return stage == ModuleStartOperation::STAGE_LINK_LAYER; }
    bool isModuleStopStage(int stage) override { return stage == ModuleStopOperation::STAGE_LINK_LAYER; }

    bool isUpperMessage(cMessage *message) override;
    bool isLowerMessage(cMessage *message) override;

    void handleUpperPacket(Packet *packet) override;
    void handleLowerPacket(Packet *packet) override;
    void handleSelfMessage(cMessage *message) override;

    void addCallback(IOmnetPluggable *layer, double time);
    void configureInterfaceEntry();


public:
    void receiveFromUpper(L2Packet* data, unsigned int center_frequency) override;
    unsigned long getCurrentDatarate() const override;
    unsigned int getNumHopsToGroundStation() const { return 0;};
    void reportNumHopsToGS(const MacId& id, unsigned int num_hops) {};
    void receiveFromLower(L3Packet* packet);


};

#endif //__INET_INT_AIR_NET_LL_H
