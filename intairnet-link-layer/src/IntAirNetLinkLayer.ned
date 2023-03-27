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

import intairnet_link_layer.LinkLayer;

package intairnet_link_layer;

simple IntAirNetLinkLayer extends LinkLayer {
    parameters:
        @display("i=block/rxtx");
        @class(IntAirNetLinkLayer);
        
        int numPPChannels = default(3); // The number of PP channels that are locally available.        
        double broadcastTargetCollisionRate = default(0.626);
        double per = default(0);
        int numRtx = default(3);
        int broadcastSlotSelectionMinNumCandidateSlots = default(9);
        int broadcastSlotSelectionMaxNumCandidateSlots = default(10000); 
        string contentionMethod = default("randomized_slotted_aloha"); 
        bool advertiseNextBroadcastSlotInCurrentHeader = default(true);        
        int planningHorizon = default(8192); // number of slots both into past and future                                
        bool arqIsUsed = default(false);
        bool gpsrIsUsed = default(false);
        string gpsrModule = default("^.^.routing");
        // A 'silent' user cannot transmit packets at all.
        bool silent = default(false);
        // Duty cycle parameters.
        int duty_cycle_period = default(100); // no. of slots to consider when computing the duty cycle
        double max_duty_cycle = default(0.1); // max. percentage of time spent transmitting
        int duty_cycle_min_num_supported_pp_links = default(4); // used to leave duty cycle budget when computing the minimum slot offset 
        bool consider_duty_cycle = default(true);        
        int duty_cycle_budget_computation_strategy = default(0); // 0=Statically assigns a fraction of the maximum duty cycle to each link. 1=Dynamically computes the available duty cycle based on the maximum duty cycle and the number of already established links.
        // Usually the period in-between PP communication bursts is computed from the number of established links and the duty cycle,
        // but if you'd like to force it (for demonstrative purposes), here's how:
        bool should_force_pp_period = default(false); 
        int forced_pp_period = default(0);
        // In one offshoot project, we have investigated how the coexisting DME system's channel access patterns
        // could be learned through means of Machine Learning. This parameter stems from that. 
        // Setting it to 'true' will cause undefined behavior because the implementation should probably adapt to the newest version first.
        bool learnDMEActivity = default(false); // whether to save observations of DME packets for to learn the pattern
        bool shouldCapturePerSlotStatistics = default(false); // Per-slot statistics can take up a lot of memory. So enable these only if explicitly required by your evaluation.
        
        @signal[rlc_bits_received_from_upper](type=double);
        @signal[rlc_bits_received_from_lower](type=double);
        @signal[rlc_bits_requested_from_lower](type=double);
        @signal[rlc_packet_received_from_upper](type=double);
        @signal[rlc_packet_sent_down](type=double);                        
        @signal[rlc_packet_sent_up](type=double);
        @signal[rlc_bits_to_send](type=double);
        @signal[rlc_packets_to_send](type=double);
        @signal[rlc_packets_injected](type=double);
        @signal[rlc_awaiting_reassembly](type=double);
        @signal[rlc_packets_to_send](type=double);
        @statistic[rlc_bits_received_from_upper](title="rlc_bits_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_received_from_lower](title="rlc_bits_received_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_requested_from_lower](title="rlc_bits_requested_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_received_from_upper](title="rlc_packet_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_sent_down](title="rlc_packet_sent_down(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_packet_sent_up](title="rlc_packet_sent_up(bits)"; record=vector,histogram; interpolationmode=none);   
        @statistic[rlc_bits_to_send](title="rlc_bits_to_send"; record=vector,histogram; interpolationmode=none);    
        @statistic[rlc_packets_to_send](title="rlc_packets_to_send"; record=vector,histogram; interpolationmode=none);  
        @statistic[rlc_packets_injected](title="rlc_packets_injected"; record=vector,histogram; interpolationmode=none); 
        @statistic[rlc_awaiting_reassembly](title="rlc_awaiting_reassembly"; record=vector,histogram; interpolationmode=none); 
        @statistic[rlc_packets_to_send](title="rlc_packets_to_send"; record=vector,histogram; interpolationmode=none);  
        
        
        @signal[arq_bits_received_from_lower](type=double);   
        @signal[arq_bits_requested_from_lower](type=double); 
        @signal[arq_bits_sent_down](type=double);                        
        @signal[arq_bits_sent_up](type=double);                          
        @signal[arq_num_rtx](type=double);                          
        @signal[arq_sent_unacked](type=double);                          
        @signal[arq_received_out_of_sequence](type=double);                            
        @signal[arq_seq_no_received](type=double);   
        @signal[arq_seq_no_sent](type=double);
        @signal[arq_rtx_list](type=double);
        @signal[arq_srej](type=double);
        @signal[arq_seqno_passed_up](type=double);
        @signal[arq_out_of_sequence_list](type=double);
        @signal[arq_seq_no_passed_up](type=double);
        @statistic[arq_bits_requested_from_lower](title="arq_bits_requested_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[arq_bits_sent_down](title="arq_packet_sent_down(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[arq_bits_sent_up](title="arq_packet_sent_up(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[arq_num_rtx](title="retransmissions(num)"; record=vector,histogram; interpolationmode=none); 
        @statistic[arq_sent_unacked](title="sent_unacked(num)"; record=vector,histogram; interpolationmode=none); 
        @statistic[arq_received_out_of_sequence](title="received_out_of_sequence(num)"; record=vector,histogram; interpolationmode=none); 
        @statistic[arq_seq_no_received](title="arq_seq_no_received"; record=vector,histogram; interpolationmode=none);  
        @statistic[arq_seq_no_sent](title="arq_seq_no_sent"; record=vector,histogram; interpolationmode=none);   
        @statistic[arq_rtx_list](title="arq_rtx_list(num)"; record=vector,histogram; interpolationmode=none);   
        @statistic[arq_srej](title="arq_srej(seqNo)"; record=vector,histogram; interpolationmode=none);   
        @statistic[arq_seqno_passed_up](title="arq_seqno_passed_up(seqNo)"; record=vector,histogram; interpolationmode=none);    
        @statistic[arq_out_of_sequence_list](title="arq_out_of_sequence_list(seqNo)"; record=vector,histogram; interpolationmode=none);  
        @statistic[arq_seq_no_passed_up](title="arq_seq_no_passed_up(seqNo)"; record=vector,histogram; interpolationmode=none);                  
              
        
        @signal[mcsotdma_statistic_num_packets_received](type=double);
        @signal[mcsotdma_statistic_num_broadcasts_received](type=double);
        @signal[mcsotdma_statistic_num_broadcast_message_processed](type=double);        
        @signal[mcsotdma_statistic_num_unicasts_received](type=double);
        @signal[mcsotdma_statistic_num_unicast_message_processed](type=double);        
        @signal[mcsotdma_statistic_num_link_requests_received](type=double);
        @signal[mcsotdma_statistic_num_link_replies_received](type=double);        
        @signal[mcsotdma_statistic_num_packets_sent](type=double);
        @signal[mcsotdma_statistic_num_broadcasts_sent](type=double);
        @signal[mcsotdma_statistic_num_unicasts_sent](type=double);
        @signal[mcsotdma_statistic_num_link_requests_sent](type=double);
        @signal[mcsotdma_statistic_num_link_replies_sent](type=double);        
        @signal[mcsotdma_statistic_num_cancelled_link_requests](type=double);
        @signal[mcsotdma_statistic_num_packet_collisions](type=double);        
        @signal[mcsotdma_statistic_num_channel_errors](type=double);                
        @signal[mcsotdma_statistic_num_active_neighbors](type=double);        
        @signal[mcsotdma_statistic_broadcast_candidate_slots](type=double);                                      
        @signal[mcsotdma_statistic_broadcast_mac_delay](type=double);          
        @signal[mcsotdma_statistic_avg_beacon_rx_delay](type=double);          
        @signal[mcsotdma_statistic_first_neighbor_beacon_rx_delay](type=double);                  
        @signal[mcsotdma_statistic_broadcast_selected_candidate_slot](type=double);                                                              
        @signal[mcsotdma_statistic_pp_link_missed_last_reply_opportunity](type=double);     
        @signal[mcsotdma_statistic_pp_link_establishment_time](type=double);
        @signal[mcsotdma_statistic_num_pp_links_established](type=double);        
        @signal[mcsotdma_statistic_num_third_party_link_requests_received](type=double);
        @signal[mcsotdma_statistic_num_third_party_replies_rcvd](type=double);
        @signal[mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_reply_slot](type=double);
        @signal[mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_pp_resource_proposals](type=double);        
        @signal[mcsotdma_statistic_num_pp_links_expired](type=double);           
        @signal[mcsotdma_statistic_num_num_dme_packets_rcvd](type=double);                   
        @signal[mcsotdma_statistic_num_broadcast_collisions_detected](type=double);                           
        @signal[mcsotdma_statistic_num_pp_requests_canceled_due_to_insufficient_resources](type=double);   
        @signal[mcsotdma_statistic_unicast_mac_delay](type=double);        
        @signal[mcsotdma_statistic_duty_cycle](type=double);
        @signal[mcsotdma_statistic_num_own_proposals_sent](type=double);
        @signal[mcsotdma_statistic_num_saved_proposals_sent](type=double);
        @signal[mcsotdma_statistic_num_link_utils_rcvd](type=double);
        @signal[mcsotdma_statistic_pp_link_requests_accepted](type=double);        
        @signal[mcsotdma_statistic_pp_period](type=double);
        @signal[mcsotdma_statistic_dropped_packets_this_slot](type=double);
        @signal[mcsotdma_statistic_sent_packets_this_slot](type=double);
        @signal[mcsotdma_statistic_rcvd_packets_this_slot](type=double);        
                
        @statistic[mcsotdma_statistic_num_packets_received](title="mcsotdma_statistic_num_packets_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_received](title="mcsotdma_statistic_num_broadcasts_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcast_message_processed](title="mcsotdma_statistic_num_broadcast_message_processed(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_received](title="mcsotdma_statistic_num_unicasts_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicast_message_processed](title="mcsotdma_statistic_num_unicast_message_processed(num)"; record=vector,histogram,last; interpolationmode=none);               
        @statistic[mcsotdma_statistic_num_link_requests_received](title="mcsotdma_statistic_num_link_requests_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_received](title="mcsotdma_statistic_num_link_replies_received(num)"; record=vector,histogram,last; interpolationmode=none);        
        @statistic[mcsotdma_statistic_num_packets_sent](title="mcsotdma_statistic_num_packets_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_sent](title="mcsotdma_statistic_num_broadcasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_sent](title="mcsotdma_statistic_num_unicasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_requests_sent](title="mcsotdma_statistic_num_link_requests_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_sent](title="mcsotdma_statistic_num_link_replies_sent(num)"; record=vector,histogram,last; interpolationmode=none);        
        @statistic[mcsotdma_statistic_num_cancelled_link_requests](title="mcsotdma_statistic_num_cancelled_link_requests(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_collisions](title="mcsotdma_statistic_num_packet_collisions(num)"; record=vector,histogram,last; interpolationmode=none);                
        @statistic[mcsotdma_statistic_num_channel_errors](title="mcsotdma_statistic_num_channel_errors(num)"; record=vector,histogram,last; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_num_active_neighbors](title="mcsotdma_statistic_num_active_neighbors"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_broadcast_candidate_slots](title="mcsotdma_statistic_broadcast_candidate_slots"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_broadcast_mac_delay](title="mcsotdma_statistic_broadcast_mac_delay"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_avg_beacon_rx_delay](title="mcsotdma_statistic_avg_beacon_rx_delay"; record=vector,histogram,last,mean; interpolationmode=none);        
        @statistic[mcsotdma_statistic_first_neighbor_beacon_rx_delay](title="mcsotdma_statistic_first_neighbor_beacon_rx_delay"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_broadcast_selected_candidate_slot](title="mcsotdma_statistic_broadcast_selected_candidate_slot"; record=vector,histogram,last,mean; interpolationmode=none);                                    
        @statistic[mcsotdma_statistic_pp_link_establishment_time](title="mcsotdma_statistic_pp_link_establishment_time"; record=vector,histogram,last,mean; interpolationmode=none);                                
        @statistic[mcsotdma_statistic_num_pp_links_established](title="mcsotdma_statistic_num_pp_links_established"; record=vector,histogram,last,mean; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_num_third_party_link_requests_received](title="mcsotdma_statistic_num_third_party_link_requests_received"; record=vector,histogram,last,mean; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_num_third_party_replies_rcvd](title="mcsotdma_statistic_num_third_party_replies_rcvd"; record=vector,histogram,last,mean; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_reply_slot](title="mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_reply_slot"; record=vector,histogram,last,mean; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_pp_resource_proposals](title="mcsotdma_statistic_num_pp_requests_rejected_due_to_unacceptable_pp_resource_proposals"; record=vector,histogram,last,mean; interpolationmode=none);                        
        @statistic[mcsotdma_statistic_pp_link_missed_last_reply_opportunity](title="mcsotdma_statistic_pp_link_missed_last_reply_opportunity"; record=vector,histogram,last,mean; interpolationmode=none);                                
        @statistic[mcsotdma_statistic_num_pp_links_expired](title="mcsotdma_statistic_num_pp_links_expired"; record=vector,histogram,last,mean; interpolationmode=none);                                                
        @statistic[mcsotdma_statistic_num_num_dme_packets_rcvd](title="mcsotdma_statistic_num_num_dme_packets_rcvd"; record=vector,histogram,last,mean; interpolationmode=none);                                                
        @statistic[mcsotdma_statistic_num_broadcast_collisions_detected](title="mcsotdma_statistic_num_broadcast_collisions_detected"; record=vector,histogram,last,mean; interpolationmode=none);                                                                
        @statistic[mcsotdma_statistic_num_pp_requests_canceled_due_to_insufficient_resources](title="mcsotdma_statistic_num_pp_requests_canceled_due_to_insufficient_resources"; record=vector,histogram,last,mean; interpolationmode=none);                                                                        
        @statistic[mcsotdma_statistic_unicast_mac_delay](title="mcsotdma_statistic_unicast_mac_delay"; record=vector,histogram,last,mean; interpolationmode=none);                                                                                        
        @statistic[mcsotdma_statistic_duty_cycle](title="mcsotdma_statistic_duty_cycle"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_num_own_proposals_sent](title="mcsotdma_statistic_num_own_proposals_sent"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_num_saved_proposals_sent](title="mcsotdma_statistic_num_saved_proposals_sent"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_num_link_utils_rcvd](title="mcsotdma_statistic_num_link_utils_rcvd"; record=vector,histogram,last,mean; interpolationmode=none);                
        @statistic[mcsotdma_statistic_pp_link_requests_accepted](title="mcsotdma_statistic_pp_link_requests_accepted"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_pp_period](title="mcsotdma_statistic_pp_period"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_dropped_packets_this_slot](title="mcsotdma_statistic_dropped_packets_this_slot"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_sent_packets_this_slot](title="mcsotdma_statistic_sent_packets_this_slot"; record=vector,histogram,last,mean; interpolationmode=none);
        @statistic[mcsotdma_statistic_rcvd_packets_this_slot](title="mcsotdma_statistic_rcvd_packets_this_slot"; record=vector,histogram,last,mean; interpolationmode=none);        
                        
        @signal[phy_statistic_num_packets_received](type=double);
        @signal[phy_statistic_num_packets_missed](type=double);  
        @statistic[phy_statistic_num_packets_received](title="phy_statistic_num_packets_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[phy_statistic_num_packets_missed](title="phy_statistic_num_packets_missed(num)"; record=vector,histogram,last; interpolationmode=none);            
}
