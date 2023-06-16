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

#ifndef __INET_INT_AIR_NET_DME_LL_H
#define __INET_INT_AIR_NET_DME_LL_H

#include "LinkLayer.h"

class DMEAirborneLinkLayer : public LinkLayer {
	public:
		/** Constructor. */
		DMEAirborneLinkLayer() = default;
		/** Destructor. */
		~DMEAirborneLinkLayer() = default;

		/** Called before the start of each time slot. */
		void beforeSlotStart();

		/** Called at the start of each time slot. */
		void onSlotStart();

		/** Called at the end of each time slot. */
		void onSlotEnd();

		bool isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const override;

	protected:
		/** Called during simulation setup several times with the different init-stages. */
		void initialize(int stage) override;

		/** How to handle a packet that comes in from the layer above. */
		void handleUpperPacket(inet::Packet *packet) override;

		/** How to handle a packet that comes in from the layer below. */
    	void handleLowerPacket(inet::Packet *packet) override;

	protected:
		unsigned long long current_time_slot = 0;					
};

inline std::ostream& operator<<(std::ostream& stream, const DMEAirborneLinkLayer& link_layer) {
	return stream << "DMEAirborne";
}

#endif //__INET_INT_AIR_NET_DME_LL_H