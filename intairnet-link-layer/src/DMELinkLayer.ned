import intairnet_link_layer.LinkLayer;

package intairnet_link_layer;

simple DMELinkLayer extends LinkLayer {
	parameters:
		@display("i=block/rxtx");
        @class(DMELinkLayer);
		bool is_ground_station = default(false);
		int center_frequency = 4000; // currently hardcoded in IntAirNetLinkLayer.cc:99 -> 4000 is the SHared channel
}