#include "DMELinkLayer.h"

using namespace TUHH_INTAIRNET_MCSOTDMA;
Define_Module(DMELinkLayer);

void DMELinkLayer::beforeSlotStart() {
	throw std::runtime_error("beforeSlotStart not implemented");
}

void DMELinkLayer::onSlotStart() {
	throw std::runtime_error("onSlotStart not implemented");
}

void DMELinkLayer::onSlotEnd() {
	throw std::runtime_error("onSlotStart not implemented");
}