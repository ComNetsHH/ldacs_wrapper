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