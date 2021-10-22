/*
 * PhyLayer.h
 *
 *  Created on: 15 Dec 2020
 *      Author: fu
 */

#ifndef PHYLAYER_H_
#define PHYLAYER_H_

#include <MCSOTDMA_Phy.hpp>
#include <IOmnetPluggable.hpp>

using namespace TUHH_INTAIRNET_MCSOTDMA;

class PhyLayer : public MCSOTDMA_Phy {
        public:
            explicit PhyLayer(uint32_t planning_horizon) : MCSOTDMA_Phy(planning_horizon) {}

            void receiveFromUpper(L2Packet* data, unsigned int center_frequency) override {
                this->radio->sendToChannel(data, center_frequency);
            }

            unsigned long getCurrentDatarate() const override {
                return 1600; // 200B/slot
            }

            ~PhyLayer() override {
            }

    };

#endif /* PHYLAYER_H_ */
