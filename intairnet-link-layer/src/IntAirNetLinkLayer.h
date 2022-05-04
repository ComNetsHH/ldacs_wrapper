#ifndef __INET_INT_AIR_NET_LL_H
#define __INET_INT_AIR_NET_LL_H

#include "LinkLayer.h"
#include <IRlc.hpp>
#include <IMac.hpp>
#include <IArq.hpp>
#include <IPhy.hpp>
#include <L2Packet.hpp>
#include <IOmnetPluggable.hpp>
#include <map>
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/transportlayer/udp/UdpHeader_m.h"
#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/HopLimitTag_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"


using namespace inet;
using namespace std;
using namespace TUHH_INTAIRNET_MCSOTDMA;

class LinkLayerLifecycleManager;

/** 
 *    @author Konrad Fuger, TUHH ComNets
 *    @date August 2020
 */
class IntAirNetLinkLayer : public LinkLayer {   
public:
    ~IntAirNetLinkLayer();    

    void sendToChannel(L2Packet* data, uint64_t center_frequency) override;
    void receiveFromChannel(L2Packet *packet, uint64_t center_frequency) override { };    
    void receiveFromLower(L3Packet* packet) override;

    void beforeSlotStart();
    void onSlotStart();
    void onSlotEnd();

    bool isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const override;

protected:    
    void initialize(int stage) override;
    void finish() override;        
    void handleUpperPacket(Packet *packet) override;
    void handleLowerPacket(Packet *packet) override;    
    void addCallback(IOmnetPluggable *layer, double time);    
    void emitStatistic(string statistic_name, double value);
    void onPacketDelete(L2Packet* pkt);
    void onPayloadDelete(L2Packet::Payload* payload);
    L2Packet* copyL2Packet(L2Packet* original);
    L2Packet::Payload* copyL2PacketPayload(L2Packet::Payload* original);
    void onBeaconReceive(MacId origin_id, L2HeaderBeacon header);   

protected:    
    std::map<std::string, simsignal_t> mcsotdma_statistics_map;
    double slotDuration;    
    vector<pair<double, IOmnetPluggable*>> callbackTimes;
    IRlc* rlcSubLayer;
    IArq* arqSubLayer;
    IMac* macSubLayer;
    IPhy* phySubLayer;
    Packet *tmp;    
    bool gpsrIsUsed = false;
    bool arqIsUsed = false;    
    const std::vector<std::string> str_mcsotdma_statistics = {
            "rlc_bits_received_from_upper",
            "rlc_bits_received_from_lower",
            "rlc_bits_requested_from_lower",
            "rlc_packet_received_from_upper",
            "rlc_packet_sent_down",
            "rlc_packet_sent_up",
            "rlc_packets_to_send",
            "rlc_packets_injected",
            "rlc_awaiting_reassembly",
            "arq_bits_sent_down",
            "arq_bits_sent_up",
            "arq_num_rtx",
            "arq_sent_unacked",
            "arq_received_out_of_sequence",
            "arq_seq_no_received",
            "arq_seq_no_sent",
            "arq_rtx_list",
            "arq_srej",
            "arq_seqno_passed_up",
            "arq_bits_requested_from_lower",
            "arq_out_of_sequence_list",
            "arq_seq_no_passed_up",
            "mcsotdma_statistic_num_packets_received",
            "mcsotdma_statistic_num_broadcasts_received",
            "mcsotdma_statistic_num_broadcast_message_processed",
            "mcsotdma_statistic_num_unicasts_received",
            "mcsotdma_statistic_num_unicast_message_processed",
            "mcsotdma_statistic_num_link_requests_received",
            "mcsotdma_statistic_num_link_replies_received",
            "mcsotdma_statistic_num_links_closed_early",
            "mcsotdma_statistic_num_beacons_received",            
            "mcsotdma_statistic_num_packets_sent",
            "mcsotdma_statistic_num_broadcasts_sent",
            "mcsotdma_statistic_num_unicasts_sent",
            "mcsotdma_statistic_num_link_requests_sent",
            "mcsotdma_statistic_num_link_replies_sent",
            "mcsotdma_statistic_num_beacons_sent",            
            "mcsotdma_statistic_num_cancelled_link_requests",
            "mcsotdma_statistic_num_packet_collisions",                        
            "mcsotdma_statistic_num_channel_errors",        
            "mcsotdma_statistic_num_active_neighbors",
            "mcsotdma_statistic_min_beacon_offset",
            "mcsotdma_statistic_broadcast_candidate_slots",
            "mcsotdma_statistic_broadcast_mac_delay",
            "mcsotdma_statistic_broadcast_selected_candidate_slot",
            "mcsotdma_phy_statistic_num_missed_packets",            
            "phy_statistic_num_packets_received",
            "phy_statistic_num_packets_missed",
            "mcsotdma_statistic_broadcast_avg_neighbor_transmission_rate",
            "mcsotdma_statistic_broadcast_wasted_tx_opportunities",
            "mcsotdma_statistic_unicast_wasted_tx_opportunities",
            "mcsotdma_statistic_pp_link_missed_last_reply_opportunity",
            "mcsotdma_statistic_pp_link_establishment_time",
            "mcsotdma_statistic_num_pp_links_established",
            "mcsotdma_statistic_num_third_party_link_requests_received",
            "mcsotdma_statistic_num_third_party_replies_rcvd",
            "mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_reply_slot",
            "mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_pp_resource_proposals",
            "mcsotdma_statistic_pp_link_missed_last_reply_opportunity",
            "mcsotdma_statistic_pp_link_missed_first_data_tx",
            "mcsotdma_statistic_num_pp_links_expired",
            "mcsotdma_statistic_num_pp_requests_rejected_due_to_insufficient_tx_slots",
            "mcsotdma_statistic_num_num_dme_packets_rcvd",
            "mcsotdma_statistic_num_broadcast_collisions_detected",
            "mcsotdma_statistic_num_beacon_collisions_detected",
            "mcsotdma_statistic_num_pp_requests_canceled_due_to_insufficient_resources",
            "mcsotdma_statistic_unicast_mac_delay",
            "mcsotdma_statistic_burst_offset",
            "mcsotdma_statistic_duty_cycle"
    };
};

#endif //__INET_INT_AIR_NET_LL_H
