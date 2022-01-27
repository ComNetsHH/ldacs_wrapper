#ifndef __INET_INT_AIR_NET_DME_LL_H
#define __INET_INT_AIR_NET_DME_LL_H

#include "LinkLayer.h"

class DMELinkLayer : public LinkLayer {
	public:
		/** Constructor. */
		DMELinkLayer() = default;
		/** Destructor. */
		~DMELinkLayer() = default;

		/** Called before the start of each time slot. */
		void beforeSlotStart();
		/** Called at the start of each time slot. */
		void onSlotStart();
		/** Called at the end of each time slot. */
		void onSlotEnd();
};

#endif //__INET_INT_AIR_NET_DME_LL_H