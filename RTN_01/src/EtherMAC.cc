#include "EtherMAC.h"
#include "EthernetFrame_m.h"
#include "AppPackets_m.h"

Define_Module(EtherMAC);

int EDFCompare(cObject *a, cObject *b) {
    EthernetFrame *fa= check_and_cast<EthernetFrame *>(a);
    EthernetFrame *fb= check_and_cast<EthernetFrame *>(b);
    DataPacket *pa = check_and_cast<DataPacket *>(fa->getEncapsulatedPacket());
    DataPacket *pb = check_and_cast<DataPacket *>(fb->getEncapsulatedPacket());
    simtime_t ta = pa->getDeadlineAbs();
    simtime_t tb = pb->getDeadlineAbs();
    if (ta < tb) return -1;
    if (ta > tb) return 1;
    return 0;
}

void EtherMAC::initialize()
{
    txstate = TX_STATE_IDLE;
    rxbuf = nullptr;
    datarate = par("datarate");
    for(int i=0; i<8; i++) {
        if(strcmp(getParentModule()->getName(),"ethController")==0) {
            sprintf(nomeCoda, "txqueue_%s_[%d]", getParentModule()->getParentModule()->getName(), i);
        } else {
            sprintf(nomeCoda, "txqueue_%s_%d_[%d]", getParentModule()->getName(), getIndex(), i);
        }
        txqueue[i] = cPacketQueue(nomeCoda, &EDFCompare);
        max[i] = 0;
    }
    ifgdur = 96.0/(double)datarate;
    cValueArray *vlanArray = check_and_cast<cValueArray*>(par("vlans").objectValue());
    for (int i = 0; i < vlanArray->size(); ++i) {
        vlans.push_back((int)vlanArray->get(i).intValue());
    }
}

void EtherMAC::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        if(strcmp(msg->getName(), "TxTimer") == 0) {
            delete msg;
            cMessage *ifgtim = new cMessage("IFGTimer");
            scheduleAt(simTime()+ifgdur, ifgtim);
            txstate = TX_STATE_IFG;
        } else if(strcmp(msg->getName(), "IFGTimer") == 0) {
            delete msg;
            startTransmission();
        } else if(strcmp(msg->getName(), "RxTimer") == 0) {
            delete msg;
            if(rxbuf->hasBitError()) {
                EV_DEBUG << "Ricevuta frame errata la rimuovo!" << endl;
                delete rxbuf;
                rxbuf = nullptr;
                return;
            }
            send(rxbuf, "upperLayerOut");
            rxbuf = nullptr;
        }
        return;
    }

    cPacket *pkt = check_and_cast<cPacket *>(msg);

    if(pkt->getArrivalGate() == gate("upperLayerIn")) {
        if(vlanFilter(pkt)) {

            delete msg;
            return;
        }

        EthernetQFrame *qf = dynamic_cast<EthernetQFrame *>(pkt);
        txqueue[qf->getPcp()].insert(pkt);
        if(txqueue[qf->getPcp()].getLength() > max[qf->getPcp()]) {
            max[qf->getPcp()] = txqueue[qf->getPcp()].getLength();
        }
        char queue[50];
        sprintf(queue, "lenghtQueue%d", qf->getPcp());
        simsignal_t sig = registerSignal(queue);
        emit(sig, txqueue[qf->getPcp()].getLength());
        if(txstate == TX_STATE_IDLE) {
            startTransmission();
        }
        return;
    }

    if(rxbuf != nullptr) {
        error("Non possono esserci due ricezioni contemporanee!");
    }
    rxbuf = pkt;
    simtime_t rxdur = (double)pkt->getBitLength()/(double)datarate;
    cMessage *rxtim = new cMessage("RxTimer");
    scheduleAt(simTime()+rxdur, rxtim);
}

bool EtherMAC::vlanFilter(cPacket *pkt) {
    if(vlans.size() == 0) {
        return false;
    }

    EthernetQFrame *qf = dynamic_cast<EthernetQFrame *>(pkt);
    if(qf == nullptr) {
        return false;
    }

    for(int i=0; i<vlans.size(); i++) {
        if(vlans[i] == qf->getVlanid()) {
            return false;
        }
    }
    return true;
}

void EtherMAC::startTransmission() {
    int selezione = -1;
    for(int i=7; i>=0; --i) {
        if(txqueue[i].getLength() > 0) {
            selezione = i;
            break;
        }
    }
    if(selezione == -1) {
        txstate = TX_STATE_IDLE;
        return;
    }
    cPacket *pkt = txqueue[selezione].pop();
    char queue[50];
    sprintf(queue, "lenghtQueue%d", selezione);
    simsignal_t sig = registerSignal(queue);
    emit(sig, txqueue[selezione].getLength());
    simtime_t txdur = (double)pkt->getBitLength()/(double)datarate;
    send(pkt->dup(), "channelOut");
    delete pkt;
    cMessage *txtim = new cMessage("TxTimer");
    scheduleAt(simTime()+txdur, txtim);
    txstate = TX_STATE_TX;
}