package rtn_01;

simple EtherMAC {
    parameters:
        @display("i=block/mac");
        int datarate @unit(bps) = default(1Gbps);
        object vlans = default([]);
        @signal[lenghtQueue0];
        @statistic[lenghtQueue0](source=lenghtQueue0; record=vector, histogram, mean);
        @signal[lenghtQueue1];
        @statistic[lenghtQueue1](source=lenghtQueue1; record=vector, histogram, mean);
        @signal[lenghtQueue2];
        @statistic[lenghtQueue2](source=lenghtQueue2; record=vector, histogram, mean);
        @signal[lenghtQueue3];
        @statistic[lenghtQueue3](source=lenghtQueue3; record=vector, histogram, mean);
        @signal[lenghtQueue4];
        @statistic[lenghtQueue4](source=lenghtQueue4; record=vector, histogram, mean);
        @signal[lenghtQueue5];
        @statistic[lenghtQueue5](source=lenghtQueue5; record=vector, histogram, mean);
        @signal[lenghtQueue6];
        @statistic[lenghtQueue6](source=lenghtQueue6; record=vector, histogram, mean);
        @signal[lenghtQueue7];
        @statistic[lenghtQueue7](source=lenghtQueue7; record=vector, histogram, mean);
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input channelIn;
        output channelOut;
}
