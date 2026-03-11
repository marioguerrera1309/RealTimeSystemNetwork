#include "EthEncap.h"
#include "EthernetFrame_m.h"

Define_Module(EthEncap);

void EthEncap::initialize()
{
    address = par("address").str();
}

void EthEncap::handleMessage(cMessage *msg)
{
    if(msg->getArrivalGate() == gate("upperLayerIn")) {
        cPacket *payload = check_and_cast<cPacket *>(msg);
        EthTransmitReq *req = check_and_cast<EthTransmitReq *>(msg->getControlInfo());
        EthernetFrame *frame = new EthernetFrame("EthernetFrame");
        if(req->getVlanid() != -1) {
            EthernetQFrame *qframe = new EthernetQFrame("EthernetQFrame");
            qframe->setVlanid(req->getVlanid());
            qframe->setSrc(req->getSrc());
            qframe->setDst(req->getDst());
            qframe->setPcp(req->getPcp());
            qframe->encapsulate(payload);
            frame = qframe;
        } else {
            frame->setSrc(req->getSrc());
            frame->setDst(req->getDst());
            frame->encapsulate(payload);
        }
        send(frame->dup(), "lowerLayerOut");
        delete frame;
    }
    else if(msg->getArrivalGate() == gate("lowerLayerIn")) {
        EthernetFrame *frame = dynamic_cast<EthernetFrame *>(msg);
        if(strcmp(frame->getDst(), address.c_str()) == 0) {
            cPacket *payload = frame->decapsulate();
            send(payload, "upperLayerOut");
            delete frame;
        } else {
            delete frame;
        }
    }
}
