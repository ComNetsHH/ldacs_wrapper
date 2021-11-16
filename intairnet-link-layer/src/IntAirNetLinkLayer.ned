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
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

import inet.linklayer.contract.ILinkLayer;
import inet.common.LayeredProtocolBase;

package intairnet_link_layer;


simple IntAirNetLinkLayer extends LayeredProtocolBase like ILinkLayer {
    parameters:
        double slotDuration @unit(s) = default(12ms);
        double broadcastTargetCollisionRate = default(0.05);
        int broadcastSlotSelectionMinNumCandidateSlots = default(3);
        int broadcastSlotSelectionMaxNumCandidateSlots = default(1000);
        string contentionMethod = default("poisson_binomial_estimate"); // can be 'binomial_estimate', 'poisson_binomial_estimate', 'all_active_again_assumption', 'naive_random_access'        
        bool alwaysAdvertiseNextBroadcastSlot = default(true);
        int planningHorizon = default(1024); // number of slots both into past and future, e.g. 1024*12ms>12s
        int minBeaconInterval = default(80);
        int maxBeaconInterval = default(25000);
        bool forceBidirectionalP2PLinks = default(false);
        bool initializeBidirectionalP2PLinks = default(true);
        bool closeP2PLinksEarly = default(false);
        string lifecycleManager = "^.^.^.lifecycleManager";
        string arpModule = default("^.^.ipv4.arp");
        bool gpsrIsUsed = default(false);
        string gpsrModule = default("^.^.gpsr");
        
        @display("i=block/rxtx");
        @class(IntAirNetLinkLayer);
        
        
        @signal[rlc_bits_received_from_upper](type=double);
        @signal[rlc_bits_received_from_lower](type=double);
        @signal[rlc_bits_requested_from_lower](type=double);
        @signal[rlc_packet_received_from_upper](type=double);
        @signal[rlc_packet_sent_down](type=double);                        
        @signal[rlc_packet_sent_up](type=double);
        @statistic[rlc_bits_received_from_upper](title="rlc_bits_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_received_from_lower](title="rlc_bits_received_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_requested_from_lower](title="rlc_bits_requested_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_received_from_upper](title="rlc_packet_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_sent_down](title="rlc_packet_sent_down(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_sent_up](title="rlc_packet_sent_up(bits)"; record=vector,histogram; interpolationmode=none);               
        
        @signal[mcsotdma_statistic_num_packets_received](type=double);
        @signal[mcsotdma_statistic_num_broadcasts_received](type=double);
        @signal[mcsotdma_statistic_num_broadcast_message_decoded](type=double);        
        @signal[mcsotdma_statistic_num_unicasts_received](type=double);
        @signal[mcsotdma_statistic_num_unicast_message_decoded](type=double);        
        @signal[mcsotdma_statistic_num_link_requests_received](type=double);
        @signal[mcsotdma_statistic_num_link_replies_received](type=double);
        @signal[mcsotdma_statistic_num_beacons_received](type=double);
        @signal[mcsotdma_statistic_num_link_infos_received](type=double);
        @signal[mcsotdma_statistic_num_packets_sent](type=double);
        @signal[mcsotdma_statistic_num_broadcasts_sent](type=double);
        @signal[mcsotdma_statistic_num_unicasts_sent](type=double);
        @signal[mcsotdma_statistic_num_link_requests_sent](type=double);
        @signal[mcsotdma_statistic_num_link_replies_sent](type=double);
        @signal[mcsotdma_statistic_num_beacons_sent](type=double);
        @signal[mcsotdma_statistic_num_link_infos_sent](type=double);
        @signal[mcsotdma_statistic_num_cancelled_link_requests](type=double);
        @signal[mcsotdma_statistic_num_packet_collisions](type=double);
        @signal[mcsotdma_statistic_num_packet_decoded](type=double);                                      
        @signal[mcsotdma_statistic_contention](type=double);
        @signal[mcsotdma_statistic_congestion](type=double);
        @signal[mcsotdma_statistic_num_active_neighbors](type=double);
        @signal[mcsotdma_statistic_min_beacon_offset](type=double);        
        @signal[mcsotdma_statistic_broadcast_candidate_slots](type=double);                              
        @signal[mcsotdma_statistic_num_links_closed_early](type=double);   
        @signal[mcsotdma_statistic_broadcast_mac_delay](type=double);          
        @signal[mcsotdma_statistic_broadcast_selected_candidate_slot](type=double);                  
        @signal[mcsotdma_statistic_broadcast_avg_neighbor_transmission_rate](type=double);  
        @signal[mcsotdma_statistic_broadcast_wasted_tx_opportunities](type=double);  
        @signal[mcsotdma_statistic_unicast_wasted_tx_opportunities](type=double);    
        @signal[mcsotdma_statistic_pp_link_missed_last_reply_opportunity](type=double);            
        @statistic[mcsotdma_statistic_num_packets_received](title="mcsotdma_statistic_num_packets_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_received](title="mcsotdma_statistic_num_broadcasts_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcast_message_decoded](title="mcsotdma_statistic_num_broadcast_message_decoded(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_received](title="mcsotdma_statistic_num_unicasts_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicast_message_decoded](title="mcsotdma_statistic_num_unicast_message_decoded(num)"; record=vector,histogram,last; interpolationmode=none);               
        @statistic[mcsotdma_statistic_num_link_requests_received](title="mcsotdma_statistic_num_link_requests_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_received](title="mcsotdma_statistic_num_link_replies_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_beacons_received](title="mcsotdma_statistic_num_beacons_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_infos_received](title="mcsotdma_statistic_num_link_infos_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packets_sent](title="mcsotdma_statistic_num_packets_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_sent](title="mcsotdma_statistic_num_broadcasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_sent](title="mcsotdma_statistic_num_unicasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_requests_sent](title="mcsotdma_statistic_num_link_requests_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_sent](title="mcsotdma_statistic_num_link_replies_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_beacons_sent](title="mcsotdma_statistic_num_beacons_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_infos_sent](title="mcsotdma_statistic_num_link_infos_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_cancelled_link_requests](title="mcsotdma_statistic_num_cancelled_link_requests(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_collisions](title="mcsotdma_statistic_num_packet_collisions(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_decoded](title="mcsotdma_statistic_num_packet_decoded(num)"; record=vector,histogram,last; interpolationmode=none);                                                                       
        @statistic[mcsotdma_statistic_contention](title="mcsotdma_statistic_contention"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_congestion](title="mcsotdma_statistic_congestion"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_active_neighbors](title="mcsotdma_statistic_num_active_neighbors"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_min_beacon_offset](title="mcsotdma_statistic_min_beacon_offset"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_broadcast_candidate_slots](title="mcsotdma_statistic_broadcast_candidate_slots"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_num_links_closed_early](title="mcsotdma_statistic_num_links_closed_early"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_broadcast_mac_delay](title="mcsotdma_statistic_broadcast_mac_delay"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_broadcast_selected_candidate_slot](title="mcsotdma_statistic_broadcast_selected_candidate_slot"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_broadcast_wasted_tx_opportunities](title="mcsotdma_statistic_broadcast_wasted_tx_opportunities"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_unicast_wasted_tx_opportunities](title="mcsotdma_statistic_unicast_wasted_tx_opportunities"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_unicast_wasted_tx_opportunities](title="mcsotdma_statistic_pp_link_missed_last_reply_opportunity"; record=vector,histogram,last,mean; interpolationmode=none);                
                        
        @signal[phy_statistic_num_packets_received](type=double);
        @signal[phy_statistic_num_packets_missed](type=double);  
        @statistic[phy_statistic_num_packets_received](title="phy_statistic_num_packets_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[phy_statistic_num_packets_missed](title="phy_statistic_num_packets_missed(num)"; record=vector,histogram,last; interpolationmode=none);
        
    gates:
        input upperLayerIn @labels(INetworkHeader/down);
        output upperLayerOut @labels(INetworkHeader/up);
        input lowerLayerIn @labels(ILinkLayerFrame/up);
        output lowerLayerOut @labels(ILinkLayerFrame/down);
}
