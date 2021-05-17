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
        string lifecycleManager = "^.^.^.lifecycleManager";
        @display("i=block/rxtx");
        @class(IntAirNetLinkLayer);
        @signal[rlc_bits_received_from_upper](type=double);
        @signal[rlc_bits_received_from_lower](type=double);
        
        @signal[mcsotdma_statistic_num_packets_received](type=int);
        @signal[mcsotdma_statistic_num_packet_collisions](type=int);
        @signal[mcsotdma_statistic_num_packet_decoded](type=int);
        
        @statistic[rlc_bits_received_from_upper](title="Rlc:packet_received_from_upper(bits)"; record=vector,histogram; interpolationmode=none);
        @statistic[rlc_bits_received_from_lower](title="Rlc:packet_received_from_lower(bits)"; record=vector,histogram; interpolationmode=none);
        
        @statistic[mcsotdma_statistic_num_packets_received](title="MCSOTDMA:statistic_num_packets_received(num)"; record=vector,histogram; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_collisions](title="MCSOTDMA:statistic_num_packet_collisions(num)"; record=vector,histogram; interpolationmode=none);
        @statistic[mcsotdma_statistic_num_packet_decoded](title="MCSOTDMA:statistic_num_packet_decoded(num)"; record=vector,histogram; interpolationmode=none);
        
    gates:
        input upperLayerIn @labels(INetworkHeader/down);
        output upperLayerOut @labels(INetworkHeader/up);
        input lowerLayerIn @labels(ILinkLayerFrame/up);
        output lowerLayerOut @labels(ILinkLayerFrame/down);
}
