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

#ifndef PHYLAYER_H_
#define PHYLAYER_H_

#include <MCSOTDMA_Phy.hpp>
#include <IOmnetPluggable.hpp>

using namespace TUHH_INTAIRNET_MCSOTDMA;

class PhyLayer : public MCSOTDMA_Phy {
        public:
            explicit PhyLayer(uint32_t planning_horizon) : MCSOTDMA_Phy(planning_horizon) {}

            void receiveFromUpper(L2Packet* data, unsigned int center_frequency) override {
                this->radio->sendToChannel(data, center_frequency);
            }

            unsigned long getCurrentDatarate() const override {
                return 1600; // 200B/slot
            }

            ~PhyLayer() override {
            }

    };

#endif /* PHYLAYER_H_ */
