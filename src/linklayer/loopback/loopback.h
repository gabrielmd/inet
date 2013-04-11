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

#ifndef __INET_LOOPBACK_H
#define __INET_LOOPBACK_H


#include "INETDefs.h"

#include "TxNotifDetails.h"
#include "INotifiable.h"

class InterfaceEntry;
class IPassiveQueue;
class NotificationBoard;

/**
 * loopback interface implementation.
 */
class INET_API LoopbackMac : public cSimpleModule
{
  protected:
    InterfaceEntry *interfaceEntry;  // points into IInterfaceTable

    // statistics
    long numSent;
    long numRcvdOK;
    long numBitErr;

    static simsignal_t dropPkBitErrorSignal;
    static simsignal_t packetSentToUpperSignal;
    static simsignal_t packetReceivedFromUpperSignal;

  protected:
    virtual InterfaceEntry *registerInterface();
    virtual void updateDisplayString();

  public:
    LoopbackMac();
    virtual ~LoopbackMac();

  protected:
    virtual int numInitStages() const {return 4;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
};

#endif
