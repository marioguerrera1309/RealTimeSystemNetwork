#ifndef __RTN_01_RELAYUNIT_H_
#define __RTN_01_RELAYUNIT_H_

#include <omnetpp.h>

using namespace omnetpp;

class RelayUnit : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif