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

#ifndef MACLAYER_H_
#define MACLAYER_H_



#include <MCSOTDMA_Mac.hpp>
#include <IOmnetPluggable.hpp>

using namespace TUHH_INTAIRNET_MCSOTDMA;

class MacLayer : public MCSOTDMA_Mac {

        friend class LinkManagerTests;
        friend class BCLinkManagerTests;
        friend class SystemTests;

    public:
        explicit MacLayer(const MacId& id, uint32_t planning_horizon) : MCSOTDMA_Mac(id, planning_horizon) {}

    protected:
        //void onReceptionSlot(const FrequencyChannel* channel) override {
            // do nothing.
        //}
};

#endif /* MACLAYER_H_ */
