import intairnet_link_layer.LinkLayer;

package intairnet_link_layer;

simple DMEGroundStationLinkLayer extends LinkLayer {
	parameters:
		@display("i=block/rxtx");
        @class(DMEGroundStationLinkLayer);		
		int center_frequency = 4000; // currently hardcoded in IntAirNetLinkLayer.cc:99 -> 4000 is the SHared channel
}