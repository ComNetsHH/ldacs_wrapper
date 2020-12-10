/*
 * InetPacketPayload.h
 *
 *  Created on: 9 Dec 2020
 *      Author: fu
 */

#ifndef INETPACKETPAYLOAD_H_
#define INETPACKETPAYLOAD_H_

#include "../../glue-lib-headers/L2Packet.hpp"
#include "inet/common/packet/Packet.h"


using namespace TUHH_INTAIRNET_MCSOTDMA;
using namespace inet;

class InetPacketPayload : public L2Packet::Payload {
public:
    Packet* original = nullptr;
    int size = 0;
    int offset = 0;
    unsigned int getBits();
};





#endif /* INETPACKETPAYLOAD_H_ */
