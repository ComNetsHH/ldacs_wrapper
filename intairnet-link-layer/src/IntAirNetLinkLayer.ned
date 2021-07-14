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
        string lifecycleManager = "^.^.^.lifecycleManager";
        @display("i=block/rxtx");
        @class(IntAirNetLinkLayer);
        
        
        @signal[rlc_bits_received_from_upper](type=double);
        @signal[rlc_bits_received_from_lower](type=double);
        
        @statistic[rlc_bits_received_from_upper](title="Rlc:packet_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_received_from_lower](title="Rlc:packet_received_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        
        
        @signal[mcsotdma_statistic_num_packets_received](type=double);
        @signal[mcsotdma_statistic_num_broadcasts_received](type=double);
        @signal[mcsotdma_statistic_num_unicasts_received](type=double);
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
        @signal[mcsotdma_phy_statistic_num_missed_packets](type=double);        
        @signal[mcsotdma_statistic_contention](type=double);
        @signal[mcsotdma_statistic_num_active_neighbors](type=double);
        @signal[mcsotdma_statistic_min_beacon_offset](type=double);        
        @signal[mcsotdma_statistic_broadcast_candidate_slots](type=double);
                       
        @statistic[mcsotdma_statistic_num_packets_received](title="MCSOTDMA:num_packets_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_received](title="MCSOTDMA:num_broadcasts_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_received](title="MCSOTDMA:num_unicasts_received(num)"; record=vector,histogram,last; interpolationmode=none);               
        @statistic[mcsotdma_statistic_num_link_requests_received](title="MCSOTDMA:num_link_requests_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_received](title="MCSOTDMA:num_link_replies_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_beacons_received](title="MCSOTDMA:num_beacons_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_infos_received](title="MCSOTDMA:num_link_infos_received(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packets_sent](title="MCSOTDMA:num_packets_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_broadcasts_sent](title="MCSOTDMA:num_broadcasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_unicasts_sent](title="MCSOTDMA:num_unicasts_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_requests_sent](title="MCSOTDMA:num_link_requests_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_replies_sent](title="MCSOTDMA:num_link_replies_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_beacons_sent](title="MCSOTDMA:num_beacons_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_link_infos_sent](title="MCSOTDMA:num_link_infos_sent(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_cancelled_link_requests](title="MCSOTDMA:num_cancelled_link_requests(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_collisions](title="MCSOTDMA:num_packet_collisions(num)"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_decoded](title="MCSOTDMA:num_packet_decoded(num)"; record=vector,histogram,last; interpolationmode=none);                                                        
        @statistic[mcsotdma_phy_statistic_num_missed_packets](title="MCSOTDMA:Phy:statistic_num_missed_packets(num)"; record=vector,histogram,last; interpolationmode=none);        
        @statistic[mcsotdma_statistic_contention](title="MCSOTDMA:statistic_contention"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_active_neighbors](title="MCSOTDMA:statistic_num_active_neighbors"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_min_beacon_offset](title="MCSOTDMA:statistic_min_beacon_offset"; record=vector,histogram,last; interpolationmode=none);
        @statistic[mcsotdma_statistic_broadcast_candidate_slots](title="MCSOTDMA:statistic_broadcast_candidate_slots"; record=vector,histogram,last; interpolationmode=none);               
        
    gates:
        input upperLayerIn @labels(INetworkHeader/down);
        output upperLayerOut @labels(INetworkHeader/up);
        input lowerLayerIn @labels(ILinkLayerFrame/up);
        output lowerLayerOut @labels(ILinkLayerFrame/down);
}
