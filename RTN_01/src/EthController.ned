package rtn_01;

import ned.IdealChannel;

module EthController
{
    @display("bgb=320,280");
    gates:
        output upperLayerOut;
        input upperLayerIn;
        output lowerLayerOut;
        input lowerLayerIn;
    submodules:
        ethEncap: EthEncap {
            @display("p=142,52");
        }
        etherMAC: EtherMAC {
            @display("p=142,165");
        }
    connections:
        ethEncap.upperLayerOut --> IdealChannel --> upperLayerOut;
        upperLayerIn --> IdealChannel --> ethEncap.upperLayerIn;
        etherMAC.upperLayerOut --> IdealChannel --> ethEncap.lowerLayerIn;
        ethEncap.lowerLayerOut --> IdealChannel --> etherMAC.upperLayerIn;
        etherMAC.channelOut --> IdealChannel --> lowerLayerOut;
        lowerLayerIn --> IdealChannel --> etherMAC.channelIn;
}