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