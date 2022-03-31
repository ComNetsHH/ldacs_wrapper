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

import inet.physicallayer.unitdisk.UnitDiskRadio;

package intairnet_link_layer;

module IntAirNetRadio extends UnitDiskRadio {
	parameters:
	    double per = default(0);
		double receiver_bandwidth = default(500); 
        double tx_power = default(42); 
        double tx_antenna_gain = default(3); 
        double rx_antenna_gain = default(3); 
        double tx_loss = default(4); 
        double rx_loss = default(4); 
        double noise_figure = default(6); 
        double thermal_noise_density = default(-174);
        double snr_margin= default(10);
	    @class(IntAirNetRadio);
}
