/*
 * MacLayer.h
 *
 *  Created on: 15 Dec 2020
 *      Author: fu
 */

#ifndef MACLAYER_H_
#define MACLAYER_H_



#include "../../mc-sotdma-headers/MCSOTDMA_Mac.hpp"
#include "../../glue-lib-headers/IOmnetPluggable.hpp"

using namespace TUHH_INTAIRNET_MCSOTDMA;

class MacLayer : public MCSOTDMA_Mac {

        friend class LinkManagerTests;
        friend class BCLinkManagerTests;
        friend class SystemTests;

    public:
        explicit MacLayer(const MacId& id, uint32_t planning_horizon) : MCSOTDMA_Mac(id, planning_horizon) {}

    protected:
        //void onReceptionSlot(const FrequencyChannel* channel) override {
            // do nothing.
        //}
};

#endif /* MACLAYER_H_ */
