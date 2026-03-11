#include "PeriodicTrafficGen.h"
#include "AppPackets_m.h"
#include "EthernetFrame_m.h"

Define_Module(PeriodicTrafficGen);

void PeriodicTrafficGen::initialize()
{
    period = par("period");
    startTime = par("startTime");
    name = par("name").str();
    payloadSize = par("payloadSize");
    burstSize = par("burstSize");
    destAddr = par("destAddr").str();
    srcAddr = par("srcAddr").str();
    vlanid = par("vlanid");
    pcp = par("pcp");
    if(startTime > 0) {
        cMessage *timer = new cMessage("TxTimer");
        scheduleAt(startTime, timer);
    }
    frameRicevute = 0;
    frameInviate = 0;
    pacchettiInRitardo = 0;
    pacchettiInOrario = 0;
    maxDelay = SIMTIME_ZERO;
    minDelay = SIMTIME_MAX;
    flag = true;
    bitRx = 0;
    bitTx = 0;
}

void PeriodicTrafficGen::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        if(strcmp(msg->getName(), "TxTimer") == 0) {
            transmitPacket();
            scheduleAt(simTime()+par("period"), msg);
            return;
        }
        error("E' arrivato un self message non previsto");
    }
    DataPacket *pkt = check_and_cast<DataPacket *>(msg);
    if(strcmp(pkt->getName(), name.c_str()) != 0) {
        delete pkt;
        return;
    }
    simtime_t delay = simTime() - pkt->getGenTime();
    if(simTime() > pkt->getDeadlineAbs()) {
        EV << "EndNode: " << name.c_str() <<" Pacchetto no. " << pkt->getPktNumber() << ", di " << pkt->getBurstSize() << " , deadlineAbs " << pkt->getDeadlineAbs() << " è arrivato a " << simTime() << " in ritardo di " << delay << endl;
        pacchettiInRitardo++;
    } else {
        EV << "EndNode: " << name.c_str() <<" Pacchetto no. " << pkt->getPktNumber() << ", di " << pkt->getBurstSize() << " , deadlineAbs " << pkt->getDeadlineAbs() << " è arrivato a " << simTime() << " in orario con ritardo di " << delay << endl;
        pacchettiInOrario++;
    }
    frameRicevute++;
    bitRx += pkt->getByteLength() * 8;
    simsignal_t sig = registerSignal("E2EDelay");
    emit(sig, delay);
    if(delay > maxDelay) {
        maxDelay = delay;
    } else if(delay < minDelay) {
        minDelay = delay;
    }
    if(pkt->getBurstSize() > 1) {
        flag = false;
    }
    if(pkt->getPktNumber() == pkt->getBurstSize()) {
        sig = registerSignal("E2EBurstDelay");
        emit(sig, delay);
    }
    delete pkt;
}

void PeriodicTrafficGen::transmitPacket() {
    DataPacket *pkt = new DataPacket(name.c_str());
    pkt->setByteLength(payloadSize);
    pkt->setGenTime(simTime());
    pkt->setBurstSize(burstSize);
    pkt->setDeadlineRel(par("deadlineRel"));
    pkt->setDeadlineAbs(simTime() + pkt->getDeadlineRel());
    for(int i=0; i<burstSize; i++) {
        DataPacket *toSend = pkt->dup();
        EthTransmitReq *req = new EthTransmitReq();
        req->setSrc(srcAddr.c_str());
        req->setDst(destAddr.c_str());
        req->setVlanid(vlanid);
        req->setPcp(pcp);
        toSend->setControlInfo(req);

        toSend->setPktNumber(i+1);
        frameInviate++;
        bitTx += toSend->getByteLength() * 8;
        send(toSend, "lowerLayerOut");
    }
    delete pkt;
}

void PeriodicTrafficGen::finish()
{
    simsignal_t sig = registerSignal("frameRicevute");
    emit(sig, frameRicevute);
    EV << "EndNode: " << getParentModule()->getName() << " APP: "<< name.c_str() << "[" << getIndex() << "]" << " ha ricevuto " << frameRicevute << " frame, bitRx: " << bitRx << endl;
    sig = registerSignal("frameInviate");
    emit(sig, frameInviate);
    EV << "EndNode: " << getParentModule()->getName() << " APP: " << name.c_str() << "[" << getIndex() << "]" <<" ha inviato " << frameInviate << " frame, bitTx: " << bitTx << endl;
    if(frameRicevute > 0) {
        EV << "EndNode: " << getParentModule()->getName() << " APP: " << name.c_str() << "[" << getIndex() << "]" <<" ha ricevuto " << pacchettiInRitardo << " pacchetti in ritardo e " << pacchettiInOrario << " pacchetti in orario" << endl;
    }
    if(frameRicevute > 0 && flag) {
        sig = registerSignal("absJitter");
        emit(sig, maxDelay - minDelay);
        sig = registerSignal("maxE2EDelay");
        emit(sig, maxDelay);
        sig = registerSignal("minE2EDelay");
        emit(sig, minDelay);
        EV << "EndNode: " << getParentModule()->getName() << " APP: " << name.c_str() << "[" << getIndex() << "] ha un jitter assoluto di " << maxDelay - minDelay << endl;

    }
    recordScalar("TotBitTx", bitTx);
    recordScalar("TotBitRx", bitRx);
}