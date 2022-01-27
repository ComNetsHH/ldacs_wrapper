#include "LinkLayer.h"
#include "LinkLayerLifecycleManager.h"

using namespace TUHH_INTAIRNET_MCSOTDMA;

void LinkLayer::sendToChannel(L2Packet* data, uint64_t center_frequency) {
	throw std::runtime_error("sendToChannel not implemented");
}

void LinkLayer::receiveFromChannel(L2Packet *packet, uint64_t center_frequency) {
	throw std::runtime_error("receiveFromChannel not implemented");
}

void LinkLayer::receiveFromLower(L3Packet* packet) {
	throw std::runtime_error("receiveFromLower not implemented");
}

void LinkLayer::initialize(int stage) {
	if (stage == inet::INITSTAGE_LOCAL) {
		upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");
        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");
		host = inet::getContainingNode(this);
        mobility = omnetpp::check_and_cast<inet::IMobility*>(host->getSubmodule("mobility"));
        arp = inet::getModuleFromPar<inet::IArp>(par("arpModule"), this);
	} else if (stage == INITSTAGE_LINK_LAYER) {
		cModule *radioModule = gate("lowerLayerOut")->getPathEndGate()->getOwnerModule();
        auto radio = check_and_cast<inet::physicallayer::IRadio *>(radioModule);
        radio->setRadioMode(inet::physicallayer::IRadio::RADIO_MODE_TRANSCEIVER);
	} else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {		
		this->interfaceEntry = inet::getContainingNicModule(this);
		// configure interface entry
		inet::MacAddress address = inet::MacAddress::generateAutoAddress();
		// data rate
		this->interfaceEntry->setDatarate(10);
		// generate a link-layer address to be used as interface token for IPv6
		this->interfaceEntry->setMacAddress(address);
		this->interfaceEntry->setInterfaceToken(address.formInterfaceIdentifier());
		//TODO: set high enough so that IP does not fragment
		this->interfaceEntry->setMtu(1500);
		// capabilities
		this->interfaceEntry->setMulticast(true);
		this->interfaceEntry->setBroadcast(true);
		// register with manager
		this->lifecycleManager = getModuleFromPar<LinkLayerLifecycleManager>(par("lifecycleManager"), this);
		this->lifecycleManager->registerClient(this);
	}	
}

void LinkLayer::finish() {
	throw std::runtime_error("finish not implemented");
}

void LinkLayer::sendUp(inet::cMessage *message) {
	throw std::runtime_error("sendUp not implemented");
}

void LinkLayer::sendDown(inet::cMessage *message) {
	throw std::runtime_error("sendDown not implemented");
}

void LinkLayer::handleMessageWhenDown(inet::cMessage *msg) {
	throw std::runtime_error("handleMessageWhenDown not implemented");
}

void LinkLayer::handleStartOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleStartOperation not implemented");
}

void LinkLayer::handleStopOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleStopOperation not implemented");
}

void LinkLayer::handleCrashOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleCrashOperation not implemented");
}

bool LinkLayer::isInitializeStage(int stage) {
	throw std::runtime_error("isInitializeStage not implemented");
}

bool LinkLayer::isModuleStartStage(int stage) {
	throw std::runtime_error("isModuleStartStage not implemented");
}

bool LinkLayer::isModuleStopStage(int stage) {
	throw std::runtime_error("isModuleStopStage not implemented");
}

bool LinkLayer::isUpperMessage(inet::cMessage *message) {
	throw std::runtime_error("isUpperMessage not implemented");
}

bool LinkLayer::isLowerMessage(inet::cMessage *message) {
	throw std::runtime_error("isLowerMessage not implemented");
}

void LinkLayer::handleUpperPacket(inet::Packet *packet) {
	throw std::runtime_error("handleUpperPacket not implemented");
}

void LinkLayer::handleLowerPacket(inet::Packet *packet) {
	throw std::runtime_error("handleLowerPacket not implemented");
}

void LinkLayer::handleSelfMessage(inet::cMessage *message) {
	throw std::runtime_error("handleSelfMessage not implemented");
}
