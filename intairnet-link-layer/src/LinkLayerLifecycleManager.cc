
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


