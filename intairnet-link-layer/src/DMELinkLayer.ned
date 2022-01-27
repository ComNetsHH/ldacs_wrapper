import intairnet_link_layer.LinkLayer;

package intairnet_link_layer;

simple IntAirNetLinkLayer extends LinkLayer {
	parameters:
		@display("i=block/rxtx");
        @class(DMELinkLayer);
}