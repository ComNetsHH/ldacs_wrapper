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

#ifndef INTAIRNETRADIO_H_
#define INTAIRNETRADIO_H_

#include "inet/physicallayer/unitdisk/UnitDiskRadio.h"

using namespace inet;
using namespace inet::physicallayer;

class IntAirNetRadio: public UnitDiskRadio {
public:
    void initialize(int stage) override;

protected:
    void startReception(cMessage *timer, IRadioSignal::SignalPart part) override;
    void endReception(cMessage *timer) override;

    double tx_power = 0;
    double tx_antenna_gain = 0;
    double tx_loss = 0;
    double rx_antenna_gain = 0;
    double rx_loss = 0;
    double noise_figure = 0;
    double thermal_noise_density = 0;
    double receiver_bandwidth = 0;
    double snr_margin = 0;

};

#endif /* INTAIRNETRADIO_H_ */
