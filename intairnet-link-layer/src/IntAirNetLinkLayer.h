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
//#include "LinkLayerLifecycleManager.h"

#include "../../glue-lib-headers/IRlc.hpp"
#include "../../glue-lib-headers/IMac.hpp"
#include "../../glue-lib-headers/IArq.hpp"
#include "../../glue-lib-headers/IPhy.hpp"
#include "../../glue-lib-headers/INet.hpp"
#include "../../glue-lib-headers/IRadio.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"

#include <map>

using namespace inet;
using namespace std;

using namespace TUHH_INTAIRNET_MCSOTDMA;

class LinkLayerLifecycleManager;


/** @brief
 * Interface implementation of the RLC layer. It defines all common function needed to implement a RLC
 *
 *    @author Konrad Fuger, TUHH ComNets
 *    @date August 2020
 *
 */
// public INet
class IntAirNetLinkLayer: public LayeredProtocolBase, public TUHH_INTAIRNET_MCSOTDMA::IRadio, public INet {
protected:

    simsignal_t rlc_bits_received_from_upper_signal;
    simsignal_t rlc_bits_received_from_lower_signal;

    const std::vector<std::string> str_mcsotdma_statistics = {
            "mcsotdma_statistic_num_packets_received",
            "mcsotdma_statistic_num_broadcasts_received",
            "mcsotdma_statistic_num_unicasts_received",
            "mcsotdma_statistic_num_link_requests_received",
            "mcsotdma_statistic_num_link_replies_received",
            "mcsotdma_statistic_num_beacons_received",
            "mcsotdma_statistic_num_link_infos_received",
            "mcsotdma_statistic_num_packets_sent",
            "mcsotdma_statistic_num_broadcasts_sent",
            "mcsotdma_statistic_num_unicasts_sent",
            "mcsotdma_statistic_num_link_requests_sent",
            "mcsotdma_statistic_num_link_replies_sent",
            "mcsotdma_statistic_num_beacons_sent",
            "mcsotdma_statistic_num_link_infos_sent",
            "mcsotdma_statistic_num_cancelled_link_requests",
            "mcsotdma_statistic_num_packet_collisions",
            "mcsotdma_statistic_num_packet_decoded",
            "mcsotdma_statistic_contention",
            "mcsotdma_statistic_num_active_neighbors",
            "mcsotdma_statistic_min_beacon_offset",
            "mcsotdma_statistic_broadcast_candidate_slots",
            "mcsotdma_phy_statistic_num_missed_packets"
    };
    std::vector<simsignal_t> mcsotdma_statistics;

    double slotDuration;

    /** Reference to the scheduler instance */
    LinkLayerLifecycleManager* lifecycleManager = nullptr;

    vector<pair<double, IOmnetPluggable*>> callbackTimes;

    IRlc* rlcSubLayer;
    IArq* arqSublayer;
    IMac* macSublayer;
    IPhy* phySubLayer;

    Packet *tmp;

    InterfaceEntry *interfaceEntry = nullptr;

    ~IntAirNetLinkLayer();

    int upperLayerInGateId = -1;
    int upperLayerOutGateId = -1;
    int lowerLayerInGateId = -1;
    int lowerLayerOutGateId = -1;

    void initialize(int stage) override;
    void finish() override;
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

    void emitStatistic(string statistic_name, double value);

public:
    void sendToChannel(L2Packet* data, uint64_t center_frequency) override;
    void receiveFromChannel(L2Packet *packet, uint64_t center_frequency) override { };
    unsigned int getNumHopsToGroundStation() const override { return 0;};
    void reportNumHopsToGS(const MacId& id, unsigned int num_hops) override {};
    void receiveFromLower(L3Packet* packet) override;


    void beforeSlotStart();
    void onSlotStart();
    void onSlotEnd();



};

#endif //__INET_INT_AIR_NET_LL_H
