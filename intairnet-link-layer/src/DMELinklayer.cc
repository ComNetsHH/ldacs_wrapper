#include "DMELinkLayer.h"

using namespace TUHH_INTAIRNET_MCSOTDMA;
Define_Module(DMELinkLayer);

void DMELinkLayer::sendToChannel(L2Packet* data, uint64_t center_frequency) {
	throw std::runtime_error("sendToChannel not implemented");
}

void DMELinkLayer::receiveFromChannel(L2Packet *packet, uint64_t center_frequency) {
	throw std::runtime_error("receiveFromChannel not implemented");
}

void DMELinkLayer::receiveFromLower(L3Packet* packet) {
	throw std::runtime_error("receiveFromLower not implemented");
}

void DMELinkLayer::beforeSlotStart() {
	throw std::runtime_error("beforeSlotStart not implemented");
}

void DMELinkLayer::onSlotStart() {
	throw std::runtime_error("onSlotStart not implemented");
}

void DMELinkLayer::onSlotEnd() {
	throw std::runtime_error("onSlotStart not implemented");
}

void DMELinkLayer::initialize(int stage) {
	throw std::runtime_error("initialize not implemented");
}

void DMELinkLayer::finish() {
	throw std::runtime_error("finish not implemented");
}

void DMELinkLayer::sendUp(inet::cMessage *message) {
	throw std::runtime_error("sendUp not implemented");
}

void DMELinkLayer::sendDown(inet::cMessage *message) {
	throw std::runtime_error("sendDown not implemented");
}

void DMELinkLayer::handleMessageWhenDown(inet::cMessage *msg) {
	throw std::runtime_error("handleMessageWhenDown not implemented");
}

void DMELinkLayer::handleStartOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleStartOperation not implemented");
}

void DMELinkLayer::handleStopOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleStopOperation not implemented");
}

void DMELinkLayer::handleCrashOperation(inet::LifecycleOperation *operation) {
	throw std::runtime_error("handleCrashOperation not implemented");
}

bool DMELinkLayer::isInitializeStage(int stage) {
	throw std::runtime_error("isInitializeStage not implemented");
}

bool DMELinkLayer::isModuleStartStage(int stage) {
	throw std::runtime_error("isModuleStartStage not implemented");
}

bool DMELinkLayer::isModuleStopStage(int stage) {
	throw std::runtime_error("isModuleStopStage not implemented");
}

bool DMELinkLayer::isUpperMessage(inet::cMessage *message) {
	throw std::runtime_error("isUpperMessage not implemented");
}

bool DMELinkLayer::isLowerMessage(inet::cMessage *message) {
	throw std::runtime_error("isLowerMessage not implemented");
}

void DMELinkLayer::handleUpperPacket(inet::Packet *packet) {
	throw std::runtime_error("handleUpperPacket not implemented");
}

void DMELinkLayer::handleLowerPacket(inet::Packet *packet) {
	throw std::runtime_error("handleLowerPacket not implemented");
}

void DMELinkLayer::handleSelfMessage(inet::cMessage *message) {
	throw std::runtime_error("handleSelfMessage not implemented");
}
