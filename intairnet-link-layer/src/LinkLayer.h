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

#ifndef __INET_INT_AIR_NET_LINKLAYER_H
#define __INET_INT_AIR_NET_LINKLAYER_H

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

class LinkLayerLifecycleManager;

/**
 * Link Layer interface.
 */
class LinkLayer : public inet::LayeredProtocolBase, public TUHH_INTAIRNET_MCSOTDMA::IRadio, public TUHH_INTAIRNET_MCSOTDMA::INet {
	public:
		/** Called before the start of each time slot. */
		virtual void beforeSlotStart() = 0;
		/** Called at the start of each time slot. */
		virtual void onSlotStart() = 0;
		/** Called at the end of each time slot. */
		virtual void onSlotEnd() = 0;

		/** IRadio interface function to send a packet to a particular frequency subchannel. */
		virtual void sendToChannel(TUHH_INTAIRNET_MCSOTDMA::L2Packet* data, uint64_t center_frequency) override;
		/** IRadio interface function to receive a packet on a particular frequency subchannel. */
		virtual void receiveFromChannel(TUHH_INTAIRNET_MCSOTDMA::L2Packet *packet, uint64_t center_frequency) override;		
		/** INet interface function when a layer-3 packet was received and reassembled. */
		virtual void receiveFromLower(L3Packet* packet) override;		

		/** TODO delete from interface */
		unsigned int getNumHopsToGroundStation() const override { return 0;};
		/** TODO delete from interface */
    	void reportNumHopsToGS(const MacId& id, unsigned int num_hops) override {};
		virtual bool isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const = 0;

	protected:
		virtual void initialize(int stage) override;
		virtual void finish() override;
		virtual void sendUp(inet::cMessage *message);
		virtual void sendDown(inet::cMessage *message);

		virtual void handleMessageWhenDown(inet::cMessage *msg) override;
		virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
		virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
		virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;

		bool isInitializeStage(int stage) override;
		bool isModuleStartStage(int stage) override;
		bool isModuleStopStage(int stage) override;

		bool isUpperMessage(inet::cMessage *message) override;
		bool isLowerMessage(inet::cMessage *message) override;

		void handleUpperPacket(inet::Packet *packet) override;
		void handleLowerPacket(inet::Packet *packet) override;
		void handleSelfMessage(inet::cMessage *message) override;

	protected:
		int upperLayerInGateId = -1;
		int upperLayerOutGateId = -1;
		int lowerLayerInGateId = -1;
		int lowerLayerOutGateId = -1;	
		inet::cModule *host = nullptr;
		inet::IMobility *mobility = nullptr;
		inet::IArp *arp = nullptr;
		/** Pointer to the scheduler instance. */
    	LinkLayerLifecycleManager *lifecycleManager = nullptr;
		inet::InterfaceEntry *interfaceEntry = nullptr;
};

#endif // __INET_INT_AIR_NET_LINKLAYER_H