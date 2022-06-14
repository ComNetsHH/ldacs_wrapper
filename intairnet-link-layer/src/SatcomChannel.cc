#include <omnetpp.h>
#include "SatcomChannel.h"

using namespace omnetpp;

Define_Channel(SatcomChannel);

void SatcomChannel::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    cDatarateChannel::processMessage(msg, t, result);
    result.delay = par("delayDist");
}