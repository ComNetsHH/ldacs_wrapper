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

#include "DMEAirborneLinkLayer.h"
#include "PacketFactory.h"

using namespace TUHH_INTAIRNET_MCSOTDMA;
Define_Module(DMEAirborneLinkLayer);

void DMEAirborneLinkLayer::initialize(int stage) {
	// call base initialize functions
	LayeredProtocolBase::initialize(stage);
	LinkLayer::initialize(stage);
	// now there's three typical init-stages
	if (stage == INITSTAGE_LOCAL) {
		
	} else if (stage == INITSTAGE_LINK_LAYER) {                

	} else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {                

	}
}

/**
 * this is called before the start of a slot
 * and may be used to update internal states if that is needed
 * it could also be that nothing is done here
 */
void DMEAirborneLinkLayer::beforeSlotStart() {	
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
}

/** 
 * at the beginning of a time slot, this is called
 * so you may transmit a packet, or schedule the next transmission, or some such
 */
void DMEAirborneLinkLayer::onSlotStart() {	
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
	// update current time slot
	current_time_slot++;	
}

/**
 * and this is finally called at the end of a time slot
 * so for example (to give you some inuition), since OMNeT++ is a discrete time simulator, during a time slot a number of packets may be received
 * these would be collected in some container, and this container is processed at the end of a time slot
 * so if that container contains exactly one packet, it is processed
 * and if it contains >1 packets, then a collision has occurred, the respective statistic is updated, and the packets are discarded
 */
void DMEAirborneLinkLayer::onSlotEnd() {
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
}

void DMEAirborneLinkLayer::handleUpperPacket(inet::Packet *packet) {
	throw std::runtime_error("handleUpperPacket not implemented");
}

void DMEAirborneLinkLayer::handleLowerPacket(inet::Packet *packet) {
	std::cout << "t=" << this->current_time_slot << " " << *this << " receives a packet." << std::endl;	
}	

bool DMEAirborneLinkLayer::isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const {
	return false;
}