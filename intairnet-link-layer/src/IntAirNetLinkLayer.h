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
    std::vector<L3Address> establishedLinks();   

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
    void onBeaconReceive(MacId origin_id, CPRPosition position);   

protected:    
    std::map<std::string, simsignal_t> mcsotdma_statistics_map;    
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
            "rlc_bits_to_send",
            "rlc_packets_to_send",
            "rlc_packets_injected",
            "rlc_awaiting_reassembly",
            "rlc_packets_to_send",
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
            "phy_statistic_num_packets_received",
            "phy_statistic_num_packets_missed",                    
            "mcsotdma_phy_statistic_num_missed_packets",                                        
            "mcsotdma_statistic_num_packets_received",
            "mcsotdma_statistic_num_broadcasts_received",
            "mcsotdma_statistic_num_broadcast_message_processed",
            "mcsotdma_statistic_num_unicasts_received",
            "mcsotdma_statistic_num_unicast_message_processed",
            "mcsotdma_statistic_num_link_requests_received",
            "mcsotdma_statistic_num_link_replies_received",            
            "mcsotdma_statistic_num_packets_sent",
            "mcsotdma_statistic_num_broadcasts_sent",
            "mcsotdma_statistic_num_unicasts_sent",
            "mcsotdma_statistic_num_link_requests_sent",
            "mcsotdma_statistic_num_link_replies_sent",                  
            "mcsotdma_statistic_num_cancelled_link_requests",
            "mcsotdma_statistic_num_packet_collisions",                        
            "mcsotdma_statistic_num_channel_errors",        
            "mcsotdma_statistic_num_active_neighbors",            
            "mcsotdma_statistic_broadcast_candidate_slots",
            "mcsotdma_statistic_broadcast_mac_delay",
            "mcsotdma_statistic_avg_beacon_rx_delay",
            "mcsotdma_statistic_first_neighbor_beacon_rx_delay",
            "mcsotdma_statistic_broadcast_selected_candidate_slot",            
            "mcsotdma_statistic_pp_link_missed_last_reply_opportunity",
            "mcsotdma_statistic_pp_link_establishment_time",
            "mcsotdma_statistic_num_pp_links_established",
            "mcsotdma_statistic_num_third_party_link_requests_received",
            "mcsotdma_statistic_num_third_party_replies_rcvd",
            "mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_reply_slot",
            "mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_pp_resource_proposals",
            "mcsotdma_statistic_pp_link_missed_last_reply_opportunity",            
            "mcsotdma_statistic_num_pp_links_expired",            
            "mcsotdma_statistic_num_num_dme_packets_rcvd",
            "mcsotdma_statistic_num_broadcast_collisions_detected",            
            "mcsotdma_statistic_num_pp_requests_canceled_due_to_insufficient_resources",
            "mcsotdma_statistic_unicast_mac_delay",            
            "mcsotdma_statistic_duty_cycle",
            "mcsotdma_statistic_num_own_proposals_sent",
            "mcsotdma_statistic_num_saved_proposals_sent",
            "mcsotdma_statistic_num_link_utils_rcvd",
            "mcsotdma_statistic_pp_link_requests_accepted",
            "mcsotdma_statistic_pp_period",
            "mcsotdma_statistic_dropped_packets_this_slot",
            "mcsotdma_statistic_sent_packets_this_slot",
            "mcsotdma_statistic_rcvd_packets_this_slot"
    };
};

#endif //__INET_INT_AIR_NET_LL_H
