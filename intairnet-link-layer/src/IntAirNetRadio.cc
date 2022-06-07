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
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "IntAirNetRadio.h"
#include "inet/physicallayer/common/packetlevel/RadioMedium.h"
#include "IntAirNetLinkLayerPacket.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/INETMath.h"
#include <L2Packet.hpp>

Register_Class(IntAirNetRadio);

void IntAirNetRadio::initialize(int stage) {
    if(stage == INITSTAGE_LOCAL) {
        tx_power = par("tx_power").doubleValue();
        tx_antenna_gain = par("tx_antenna_gain").doubleValue();
        tx_loss = par("tx_loss").doubleValue();
        rx_antenna_gain = par("rx_antenna_gain").doubleValue();
        rx_loss = par("rx_loss").doubleValue();
        noise_figure = par("noise_figure").doubleValue();
        thermal_noise_density = par("thermal_noise_density").doubleValue();
        receiver_bandwidth = par("receiver_bandwidth").doubleValue();
        snr_margin = par("snr_margin").doubleValue();
    }
    UnitDiskRadio::initialize(stage);
}

void IntAirNetRadio::startReception(cMessage *timer, IRadioSignal::SignalPart part)
{
    EV << "Lets get this transmission" << endl;
    auto signal = static_cast<Signal *>(timer->getControlInfo());
    auto arrival = signal->getArrival();
    auto reception = signal->getReception();
// TODO: should be this, but it breaks fingerprints: if (receptionTimer == nullptr && isReceiverMode(radioMode) && arrival->getStartTime(part) == simTime()) {
    //if (isReceiverMode(radioMode) && arrival->getStartTime(part) == simTime()) {
        auto transmission = signal->getTransmission();
        auto isReceptionAttempted = true; //medium->isReceptionAttempted(this, transmission, part);
        EV_INFO << "Reception started: " << (isReceptionAttempted ? "\x1b[1mattempting\x1b[0m" : "\x1b[1mnot attempting\x1b[0m") << " " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
        if (isReceptionAttempted) {
            receptionTimer = timer;
            emit(receptionStartedSignal, check_and_cast<const cObject *>(reception));
        }
    //}
    //else
    //    EV_INFO << "Reception started: \x1b[1mignoring\x1b[0m " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;

        if(arrival->getEndTime(part) < simTime()) {
            return;
        }
    timer->setKind(part);

    scheduleAt(arrival->getEndTime(part), timer);
    updateTransceiverState();
    updateTransceiverPart();
    // TODO: move to radio medium
    check_and_cast<RadioMedium *>(medium)->emit(IRadioMedium::signalArrivalStartedSignal, check_and_cast<const cObject *>(reception));
}

void IntAirNetRadio::endReception(cMessage *timer)
{
    auto part = (IRadioSignal::SignalPart)timer->getKind();
    auto signal = static_cast<Signal *>(timer->getControlInfo());
    auto arrival = signal->getArrival();
    auto reception = signal->getReception();
    // if (timer == receptionTimer && isReceiverMode(radioMode) && arrival->getEndTime() == simTime()) {
        auto transmission = signal->getTransmission();
// TODO: this would draw twice from the random number generator in isReceptionSuccessful: auto isReceptionSuccessful = medium->isReceptionSuccessful(this, transmission, part);
        auto isReceptionSuccessful = medium->getReceptionDecision(this, signal->getListening(), transmission, part)->isReceptionSuccessful();
        EV_INFO << "Reception ended: " << (isReceptionSuccessful ? "\x1b[1msuccessfully\x1b[0m" : "\x1b[1munsuccessfully\x1b[0m") << " for " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
        auto macFrame = medium->receivePacket(this, signal);
        // TODO: FIXME: see handling packets with incorrect PHY headers in the TODO file
        decapsulate(macFrame);

        // TODO: get frequence from packet
        double frequency = 960; // MHz

        Coord rx_pos = check_and_cast<IMobility *>(getContainingNode(this)->getSubmodule("mobility"))->getCurrentPosition();
        Coord tx_pos = check_and_cast<const IReception *>(reception)->getTransmission()->getStartPosition();
        double dist = rx_pos.distance(tx_pos);
        double path_loss = 2 * inet::math::fraction2dB((dist * frequency / 1000)) + 32.4478;
        double rx_power = tx_power + tx_antenna_gain - tx_loss + rx_antenna_gain - rx_loss - path_loss;
        double snr = rx_power - (noise_figure + thermal_noise_density + 10 * log10(receiver_bandwidth)) - snr_margin;

        L2Packet* containedPkt =  ((IntAirNetLinkLayerPacket *)macFrame)->getContainedPacket();
        containedPkt->receptionDist = dist;
        containedPkt->snr = snr;

        if(uniform(0, 1.0) <= par("per").doubleValue()) {
            containedPkt->hasChannelError = true;
        }
        if (isReceptionSuccessful) {
            sendUp(macFrame);
        } 
        else {
            delete (IntAirNetLinkLayerPacket *)macFrame;
        }
        receptionTimer = nullptr;
        emit(receptionEndedSignal, check_and_cast<const cObject *>(reception));
    //}
    //else
    //    EV_INFO << "Reception ended: \x1b[1mignoring\x1b[0m " << (ISignal *)signal << " " << IRadioSignal::getSignalPartName(part) << " as " << reception << endl;
    updateTransceiverState();
    updateTransceiverPart();
    delete timer;
    // TODO: move to radio medium
    check_and_cast<RadioMedium *>(medium)->emit(IRadioMedium::signalArrivalEndedSignal, check_and_cast<const cObject *>(reception));
}


