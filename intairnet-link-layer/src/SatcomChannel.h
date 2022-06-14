//==========================================================================
//   CDATARATECHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SATCOMCHANNEL_H
#define __OMNETPP_SATCOMCHANNEL_H

namespace omnetpp {

class SatcomChannel : public cDatarateChannel 
{
    void processMessage(cMessage *msg, simtime_t t, result_t& result) override;
 
};

}

#endif 


