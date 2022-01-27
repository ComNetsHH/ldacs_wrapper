#ifndef __INET_INT_AIR_NET_DME_LL_H
#define __INET_INT_AIR_NET_DME_LL_H

#include "LinkLayer.h"

class DMEGroundStationLinkLayer : public LinkLayer {
	public:
		/** Constructor. */
		DMEGroundStationLinkLayer() = default;
		/** Destructor. */
		~DMEGroundStationLinkLayer() = default;

		/** Called before the start of each time slot. */
		void beforeSlotStart();

		/** Called at the start of each time slot. */
		void onSlotStart();

		/** Called at the end of each time slot. */
		void onSlotEnd();

	protected:
		/** Called during simulation setup several times with the different init-stages. */
		void initialize(int stage) override;

		/** How to handle a packet that comes in from the layer above. */
		void handleUpperPacket(inet::Packet *packet) override;

		/** How to handle a packet that comes in from the layer below. */
    	void handleLowerPacket(inet::Packet *packet) override;

	protected:
		unsigned long long current_time_slot = 0;				

	public:
		// I made it public s.t. the << operator below can use it.
		uint64_t center_frequency = 1000;
};

inline std::ostream& operator<<(std::ostream& stream, const DMEGroundStationLinkLayer& link_layer) {
	return stream << "DMEGroundStation(f=" << link_layer.center_frequency << ")";
}

#endif //__INET_INT_AIR_NET_DME_LL_H