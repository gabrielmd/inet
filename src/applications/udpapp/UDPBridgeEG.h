//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
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


#ifndef __INET_UDPBRIDGEEG_H
#define __INET_UDPBRIDGEEG_H

#include <vector>
#include "INETDefs.h"
#include "UDPSocket.h"


/**
 * UDP application. See NED for more info.
 */
class INET_API UDPBridgeEG : public cSimpleModule
{
  protected:
    UDPSocket socket;
    int localPort, destPort;
    std::vector<IPvXAddress> destAddresses;

    const char * networkType;

    double networkTrafficParam;
    static int counter; // counter for generating a global number for each packet

    // statistics
    int numSentUDP;
    int numReceivedUDP;

    int numSentUSB;
    int numReceivedUSB;

    // usb connection
    int USBLayerIn;
    int USBLayerOut;
    int USBControlIn;
    int USBControlOut;

    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;

    // chooses random destination address
    virtual IPvXAddress chooseDestAddr();
    virtual void sendPacket(cMessage *msg);
    virtual void processPacket(cMessage *msg);

    virtual void sendUSB(cPacket *);

    enum APPL_MSG_TYPES
    {
        UPDATE_PARAM = 7919
    };

  protected:
    virtual int numInitStages() const {return 4;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif

