//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <string.h>

#include "loopback.h"

#include "opp_utils.h"
#include "IInterfaceTable.h"
#include "InterfaceTableAccess.h"
#include "IPassiveQueue.h"
#include "NotificationBoard.h"
#include "NotifierConsts.h"


Define_Module(LoopbackMac);

simsignal_t LoopbackMac::dropPkBitErrorSignal = SIMSIGNAL_NULL;
simsignal_t LoopbackMac::packetSentToUpperSignal = SIMSIGNAL_NULL;
simsignal_t LoopbackMac::packetReceivedFromUpperSignal = SIMSIGNAL_NULL;

LoopbackMac::LoopbackMac()
{
}

LoopbackMac::~LoopbackMac()
{
}

void LoopbackMac::initialize(int stage)
{
    // all initialization is done in the first stage
    if (stage == 0)
    {
        interfaceEntry = NULL;

        numSent = numRcvdOK = numBitErr = 0;
        WATCH(numSent);
        WATCH(numRcvdOK);
        WATCH(numBitErr);

        dropPkBitErrorSignal = registerSignal("dropPkBitError");
        packetSentToUpperSignal = registerSignal("packetSentToUpper");
        packetReceivedFromUpperSignal = registerSignal("packetReceivedFromUpper");

        // register our interface entry in IInterfaceTable
        interfaceEntry = registerInterface();
    }

    // update display string when addresses have been autoconfigured etc.
    if (stage == 3)
    {
        updateDisplayString();
    }
}

InterfaceEntry *LoopbackMac::registerInterface()
{
    InterfaceEntry *ie = new InterfaceEntry(this);

    // interface name: NIC module's name without special characters ([])
    ie->setName(OPP_Global::stripnonalnum(getParentModule()->getFullName()).c_str());

//    // generate a link-layer address to be used as interface token for IPv6
//    InterfaceToken token(0, simulation.getUniqueNumber(), 64);
//    e->setInterfaceToken(token);

    // capabilities
    ie->setMtu(par("mtu").longValue());
    ie->setLoopback(true);

    // add
    IInterfaceTable *ift = InterfaceTableAccess().getIfExists();
    if (ift)
        ift->addInterface(ie);

    return ie;
}

void LoopbackMac::handleMessage(cMessage *msg)
{
    emit(packetReceivedFromUpperSignal, msg);
    EV << "Received " << msg << " for transmission\n";
    if (PK(msg)->hasBitError())
    {
        EV << "Bit error in " << msg << endl;
        emit(dropPkBitErrorSignal, msg);
        numBitErr++;
        delete msg;
    }
    else
    {
        // pass up payload
        numRcvdOK++;
        emit(packetSentToUpperSignal, msg);
        numSent++;
        send(msg, "netwOut");
    }

    if (ev.isGUI())
        updateDisplayString();
}

void LoopbackMac::updateDisplayString()
{
    if (ev.isDisabled())
    {
        // speed up things
        getDisplayString().setTagArg("t", 0, "");
    }
    else
    {
        /* TBD find solution for displaying IPv4 address without dependence on IPv4 or IPv6
                IPv4Address addr = interfaceEntry->ipv4Data()->getIPAddress();
                sprintf(buf, "%s / %s\nrcv:%ld snt:%ld", addr.isUnspecified()?"-":addr.str().c_str(), datarateText, numRcvdOK, numSent);
        */
        char buf[80];
        sprintf(buf, "rcv:%ld snt:%ld", numRcvdOK, numSent);

        if (numBitErr > 0)
            sprintf(buf + strlen(buf), "\nerr:%ld", numBitErr);

        getDisplayString().setTagArg("t", 0, buf);
    }
}

