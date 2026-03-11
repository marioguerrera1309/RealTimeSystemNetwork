package rtn_01;

simple PeriodicTrafficGen
{
    parameters:
        volatile double period @unit(s) = default(0s);
        double startTime @unit(s) = default(0s);
        string name;
        int payloadSize @unit(B) = default(0B);
        int burstSize = default(1);
        string destAddr = default("");
        string srcAddr;
        int vlanid = default(-1);
        int pcp = default(0);
        double deadlineRel @unit(s);
        @display("i=block/app2");
        @signal[E2EDelay];
        @statistic[E2EDelay](source=E2EDelay; record=vector, histogram, mean);
        @signal[minE2EDelay];
        @statistic[minE2EDelay](source=minE2EDelay; record=vector, histogram, mean);
        @signal[maxE2EDelay];
        @statistic[maxE2EDelay](source=maxE2EDelay; record=vector, histogram, mean);
        @signal[E2EBurstDelay];
        @statistic[E2EBurstDelay](source=E2EBurstDelay; record=vector, histogram, mean);
        @signal[frameInviate];
        @statistic[frameInviate](source=frameInviate; record=vector, histogram, mean);
        @signal[frameRicevute];
        @statistic[frameRicevute](source=frameRicevute; record=vector, histogram, mean);
        @signal[absJitter];
        @statistic[absJitter](source=absJitter; record=vector, histogram, mean);
    gates:
        input lowerLayerIn;
        output lowerLayerOut;
}
