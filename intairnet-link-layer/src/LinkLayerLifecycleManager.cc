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

#include "LinkLayerLifecycleManager.h"
#include "MacLayer.h"
#include <RngProvider.hpp>

Define_Module(LinkLayerLifecycleManager);

LinkLayerLifecycleManager::LinkLayerLifecycleManager() {

}

LinkLayerLifecycleManager::~LinkLayerLifecycleManager() {
    cancelAndDelete(slotTimer);
}

void LinkLayerLifecycleManager::initialize(int stage) {
    // Set RNG Provider to OMNeT++.
    RngProvider::getInstance().setUseDefaultRngs(false);
    RngProvider::getInstance().setOmnetGetInt([=](int min, int max, int k) {return this->getRandomInt(min, max, k);});

    slotTimer = new cMessage("slotTimer");
    slotDuration = par("slotDuration");
    scheduleAt(slotDuration, slotTimer);
}

int LinkLayerLifecycleManager::getRandomInt(int min, int max, int k) {
    try {
        int rand_int = getRNG(k)->intRand(max - min);
        return rand_int + min;
    } catch (const std::exception& e) {
        EV << "LinkLayerLifecycleManager::getRandomInt error: " << e.what() << std::endl;
        throw e;
    }
}

void LinkLayerLifecycleManager::registerClient(LinkLayer *ll) {
    Enter_Method_Silent();
    linkLayers.push_back(ll);
}

void LinkLayerLifecycleManager::handleMessage(cMessage *message) {
    if(message == slotTimer) {

        if(!isFirstSlot) {
            for(auto const& ll: linkLayers) {
                ll->onSlotEnd();
            }
        }

        for(auto const& ll: linkLayers) {
            ll->beforeSlotStart();
        }
        for(auto const& ll: linkLayers) {
            ll->onSlotStart();
        }

        isFirstSlot = false;



//            ((MacLayer*)macSublayer)->update(1);
//        (macSublayer)->execute();
        // for all mac layers,
        // if its not the firstSlot, call on Slot end
        //call update, call execute
        scheduleAt(simTime() + slotDuration, slotTimer);
    }
}

size_t LinkLayerLifecycleManager::isGoingToTransmitDuringCurrentSlot(uint64_t center_frequecy) const {
    size_t num_transmissions_this_slot = 0;
    for (const auto *ll : linkLayers) {
        if (ll->isGoingToTransmitDuringCurrentSlot(center_frequecy))
            num_transmissions_this_slot++;
    }
    return num_transmissions_this_slot;
}


