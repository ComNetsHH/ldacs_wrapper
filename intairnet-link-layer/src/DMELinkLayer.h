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

class DMELinkLayer : public LayeredProtocolBase, public TUHH_INTAIRNET_MCSOTDMA::IRadio, public INet {
	public:
		DMELinkLayer() = default;
		~DMELinkLayer() = default;

	protected:
		void initialize(int stage) override;
		void finish() override;
		void sendUp(cMessage *message);
		void sendDown(cMessage *message);

		void handleMessageWhenDown(cMessage *msg) override;
		void handleStartOperation(LifecycleOperation *operation) override;
		void handleStopOperation(LifecycleOperation *operation) override;
		void handleCrashOperation(LifecycleOperation *operation) override;

		bool isInitializeStage(int stage) override { return stage == INITSTAGE_LINK_LAYER; }
		bool isModuleStartStage(int stage) override { return stage == ModuleStartOperation::STAGE_LINK_LAYER; }
		bool isModuleStopStage(int stage) override { return stage == ModuleStopOperation::STAGE_LINK_LAYER; }

		bool isUpperMessage(cMessage *message) override;
		bool isLowerMessage(cMessage *message) override;

		void handleUpperPacket(Packet *packet) override;
		void handleLowerPacket(Packet *packet) override;
		void handleSelfMessage(cMessage *message) override;
		
};

#endif //__INET_INT_AIR_NET_DME_LL_H