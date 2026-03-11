#include "AppDispatcher.h"
#include "EthernetFrame_m.h"

Define_Module(AppDispatcher);

void AppDispatcher::initialize() {}

void AppDispatcher::handleMessage(cMessage *msg)
{
    if(msg->getArrivalGate() == gate("lowerLayerIn")) {
        for (int i = 0; i < gateSize("upperLayerOut"); i++) {
            send(msg->dup(), "upperLayerOut", i);
        }
        delete msg;
    }
    else if (strcmp(msg->getArrivalGate()->getBaseName(), "upperLayerIn")==0) {
        send(msg, "lowerLayerOut");
    }
}
