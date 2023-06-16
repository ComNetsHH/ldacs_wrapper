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

#include "DMEGroundStationLinkLayer.h"
#include "PacketFactory.h"

using namespace TUHH_INTAIRNET_MCSOTDMA;
Define_Module(DMEGroundStationLinkLayer);

void DMEGroundStationLinkLayer::initialize(int stage) {
	// call base initialize functions
	LayeredProtocolBase::initialize(stage);
	LinkLayer::initialize(stage);
	// now there's three typical init-stages
	if (stage == INITSTAGE_LOCAL) {
		// here we parse .ini parameters into member variables		
		this->center_frequency = par("center_frequency").intValue();
	} else if (stage == INITSTAGE_LINK_LAYER) {                

	} else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {                

	}
}

/**
 * this is called before the start of a slot
 * and may be used to update internal states if that is needed
 * it could also be that nothing is done here
 */
void DMEGroundStationLinkLayer::beforeSlotStart() {	
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
}

/** 
 * at the beginning of a time slot, this is called
 * so you may transmit a packet, or schedule the next transmission, or some such
 */
void DMEGroundStationLinkLayer::onSlotStart() {	
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
	// update current time slot
	current_time_slot++;
	// every ten slots
	if (current_time_slot % 10 == 0) {
		std::cout << "t=" << this->current_time_slot << " " << *this << " transmits a packet on f=" << this->center_frequency << " -> ";		
		// create our packet
		auto *packet = new L2Packet();		
		packet->addMessage(new L2HeaderDMEResponse(), nullptr);
		// wrap it into an OMNeT++-understandable class
		auto *inet_packet = PacketFactory::fromL2Packet(packet, this->center_frequency);
		inet_packet->addTag<inet::PacketProtocolTag>()->setProtocol(&inet::Protocol::ackingMac);		
		try {
			// send it
			sendDown(inet_packet);
			std::cout << "done." << std::endl;
		} catch (const std::exception& e) {
			// it's generally nice to print errors to stdout
			// so that you immediately see what's wrong
			// without having to go into a debugger
			std::cerr << "Exception in DMEGroundStationLinkLayer::sendToChannel: " << e.what() << std::endl;
			// after catching it, we throw it again, so that the program still terminates
			throw e;
		}
	}
}

/**
 * and this is finally called at the end of a time slot
 * so for example (to give you some inuition), since OMNeT++ is a discrete time simulator, during a time slot a number of packets may be received
 * these would be collected in some container, and this container is processed at the end of a time slot
 * so if that container contains exactly one packet, it is processed
 * and if it contains >1 packets, then a collision has occurred, the respective statistic is updated, and the packets are discarded
 */
void DMEGroundStationLinkLayer::onSlotEnd() {
	Enter_Method_Silent(); // just keep this, OMNeT++ needs this
}

void DMEGroundStationLinkLayer::handleUpperPacket(inet::Packet *packet) {
	throw std::runtime_error("handleUpperPacket not implemented @DME");
}

void DMEGroundStationLinkLayer::handleLowerPacket(inet::Packet *packet) {
	// We're sending only IntAirNetLinkLayerPacket* instances, so it's safe to cast.
	auto* casted_packet = (IntAirNetLinkLayerPacket*) packet;
	// These contain our own packet implementation, which you can get like so.
    L2Packet* data_packet = casted_packet->getContainedPacket();
	// The frequency that was used for transmission can then be parsed.
    auto center_frequency = casted_packet->center_frequency;
	std::cout << "t=" << this->current_time_slot << " " << *this << " received a packet on f=" << center_frequency << " -> ";
	if (center_frequency == this->center_frequency) {
		std::cout << "process -> ";
		for (L2Header* header : data_packet->getHeaders()) {
			std::cout << "header='" << header->frame_type << "' ";
		}		
		// do some processing
		std::cout << "-> done." << std::endl;
	} else {
		std::cout << "discard, as my frequency is " << this->center_frequency << "." << std::endl;
	}
}	

bool DMEGroundStationLinkLayer::isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const {
	return false;
}