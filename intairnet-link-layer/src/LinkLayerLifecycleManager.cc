
#include "LinkLayerLifecycleManager.h"
#include "MacLayer.h"

Define_Module(LinkLayerLifecycleManager);

LinkLayerLifecycleManager::LinkLayerLifecycleManager() {

}

LinkLayerLifecycleManager::~LinkLayerLifecycleManager() {
    cancelAndDelete(slotTimer);
}

void LinkLayerLifecycleManager::initialize(int stage) {
    slotTimer = new cMessage("slotTimer");
    slotDuration = par("slotDuration");
    scheduleAt(slotDuration, slotTimer);
}

void LinkLayerLifecycleManager::registerClient(IntAirNetLinkLayer *ll) {
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




