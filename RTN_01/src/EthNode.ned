package rtn_01;

import ned.IdealChannel;

module EthNode
{
    parameters:
        int n;
        @display("bgb=470,359");
    gates:
        input channelIn;
        output channelOut;
    submodules:
        periodicTrafficGen[n]: PeriodicTrafficGen {
            @display("p=203,53");
        }
        appDispatcher: AppDispatcher {
            @display("p=203,155");
        }
        ethController: EthController {
            @display("p=203,263");
        }
    connections:
        for i=0..n-1 {
            periodicTrafficGen[i].lowerLayerOut --> IdealChannel --> appDispatcher.upperLayerIn++;
            appDispatcher.upperLayerOut++ --> IdealChannel --> periodicTrafficGen[i].lowerLayerIn;
        }
        appDispatcher.lowerLayerOut --> IdealChannel --> ethController.upperLayerIn;
        ethController.upperLayerOut --> IdealChannel --> appDispatcher.lowerLayerIn;
        channelIn --> IdealChannel --> ethController.lowerLayerIn;
        ethController.lowerLayerOut --> IdealChannel --> channelOut;
}
