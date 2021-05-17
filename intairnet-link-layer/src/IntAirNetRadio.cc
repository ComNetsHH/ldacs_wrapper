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

Register_Class(IntAirNetRadio);

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
        sendUp(macFrame);
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


