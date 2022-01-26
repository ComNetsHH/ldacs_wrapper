#ifndef __INET_INT_AIR_NET_DME_LL_H
#define __INET_INT_AIR_NET_DME_LL_H

#include "inet/common/INETDefs.h"
#include "inet/queueing/contract/IPacketQueue.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/LayeredProtocolBase.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/contract/IArp.h"
#include <IRadio.hpp>
#include <INet.hpp>
#include <L3Packet.hpp>
#include <L2Packet.hpp>
#include <IOmnetPluggable.hpp>

class DMELinkLayer : public inet::LayeredProtocolBase, public TUHH_INTAIRNET_MCSOTDMA::IRadio, public TUHH_INTAIRNET_MCSOTDMA::INet {
	public:
		/** Constructor. */
		DMELinkLayer() = default;
		/** Destructor. */
		~DMELinkLayer() = default;

		/** IRadio interface function to send a packet to a particular frequency subchannel. */
		void sendToChannel(TUHH_INTAIRNET_MCSOTDMA::L2Packet* data, uint64_t center_frequency) override;
		/** IRadio interface function to receive a packet on a particular frequency subchannel. */
		void receiveFromChannel(TUHH_INTAIRNET_MCSOTDMA::L2Packet *packet, uint64_t center_frequency) override;		
		/** INet interface function when a layer-3 packet was received and reassembled. */
		void receiveFromLower(L3Packet* packet) override;

		/** Called before the start of each time slot. */
		void beforeSlotStart();
		/** Called at the start of each time slot. */
		void onSlotStart();
		/** Called at the end of each time slot. */
		void onSlotEnd();

		/** TODO delete from interface */
		unsigned int getNumHopsToGroundStation() const override { return 0;};
		/** TODO delete from interface */
    	void reportNumHopsToGS(const MacId& id, unsigned int num_hops) override {};

	protected:
		void initialize(int stage) override;
		void finish() override;
		void sendUp(inet::cMessage *message);
		void sendDown(inet::cMessage *message);

		void handleMessageWhenDown(inet::cMessage *msg) override;
		void handleStartOperation(inet::LifecycleOperation *operation) override;
		void handleStopOperation(inet::LifecycleOperation *operation) override;
		void handleCrashOperation(inet::LifecycleOperation *operation) override;

		bool isInitializeStage(int stage) override;
		bool isModuleStartStage(int stage) override;
		bool isModuleStopStage(int stage) override;

		bool isUpperMessage(inet::cMessage *message) override;
		bool isLowerMessage(inet::cMessage *message) override;

		void handleUpperPacket(inet::Packet *packet) override;
		void handleLowerPacket(inet::Packet *packet) override;
		void handleSelfMessage(inet::cMessage *message) override;
		
};

#endif //__INET_INT_AIR_NET_DME_LL_H