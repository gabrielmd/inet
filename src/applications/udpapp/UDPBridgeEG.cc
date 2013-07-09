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

#include "UDPBridgeEG.h"
#include "UDPControlInfo_m.h"
#include "IPvXAddressResolver.h"
#include "InterfaceTableAccess.h"

Define_Module(UDPBridgeEG);

int UDPBridgeEG::counter;
simsignal_t UDPBridgeEG::sentPkSignal = SIMSIGNAL_NULL;
simsignal_t UDPBridgeEG::rcvdPkSignal = SIMSIGNAL_NULL;

void UDPBridgeEG::initialize(int stage)
{
    // because of IPvXAddressResolver, we need to wait until interfaces are registered,
    // address auto-assignment takes place etc.
    if (stage != 3)
        return;

    // usb connection
    USBLayerIn  = findGate("USBLayerIn");
    USBLayerOut = findGate("USBLayerOut");

    networkType = par("networkType").stringValue();
    networkTrafficParam = par("trafficParam").doubleValue();

    counter = 0;

    // keep tracking the counter
    numSentUDP = 0;
    numReceivedUDP = 0;
    numSentUSB = 0;
    numReceivedUSB = 0;
    WATCH(numSentUDP);
    WATCH(numReceivedUDP);
    WATCH(numSentUSB);
    WATCH(numReceivedUSB);

    // udp configurations
    sentPkSignal = registerSignal("sentPk");
    rcvdPkSignal = registerSignal("rcvdPk");

    localPort = par("localPort");
    destPort = par("destPort");

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != NULL)
        destAddresses.push_back(IPvXAddressResolver().resolve(token));

    socket.setOutputGate(gate("udpOut"));
    socket.bind(localPort);

    if (destAddresses.empty())
        return;

}

void UDPBridgeEG::finish()
{
    recordScalar("UDP packets sent", numSentUDP);
    recordScalar("UDP packets received", numReceivedUDP);

    recordScalar("USB packets sent", numSentUSB);
    recordScalar("USB packets received", numReceivedUSB);
}

IPvXAddress UDPBridgeEG::chooseDestAddr()
{
    // it can send to a random address, to all the neighbors, ...
    //int k = intrand(destAddresses.size());
    //return destAddresses[k];
    return destAddresses[0];
}

void UDPBridgeEG::sendPacket(cMessage *msg)
{


    char payload[100];
    sprintf(payload, "%s %s", msg->getName(), networkType);
    cPacket * pkt = new cPacket(payload);

    emit(sentPkSignal, pkt);

    IPvXAddress destAddr = chooseDestAddr();
    socket.sendTo(pkt, destAddr, destPort);

    numSentUDP++;
    delete msg;
}

void UDPBridgeEG::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {


    } else if(msg->getKind() == UPDATE_PARAM) {
        // received from sink node ... will retransmit
        sendPacket(msg);
    }else if (msg->getKind() == UDP_I_DATA)
    {
        // sent from another EG through UDP
        // process incoming packet
        processPacket(msg);
    }
    else if (msg->getKind() == UDP_I_ERROR)
    {
        EV << "Ignoring UDP error report\n";
        delete msg;
    }
    else
    {
        error("Unrecognized message (%s)%s (%d/%d) ", msg->getClassName(), msg->getName(), msg->getKind(), UPDATE_PARAM);
    }

    if (ev.isGUI())
    {
        char buf[40];
        sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceivedUDP, numSentUDP);
        getDisplayString().setTagArg("t", 0, buf);
    }
}

void UDPBridgeEG::processPacket(cMessage *msg)
{
    char type[3]; // network type ("T", "RH"...)
    int value;
    sscanf(msg->getName(), "%d %s", &value, type);


    double trafficParam;

    if(!strcmp(networkType, "T") && !strcmp(type, "RH")){ // received relative humidity and controls temperature
        if(value < 20){
            trafficParam = 1;
        }else{
            trafficParam = 5;
        }
        EV << "This network measures relative humidity and has received the value " << value << " from the temperature sensors.\n";
    }else if(!strcmp(networkType, "RH") && !strcmp(type, "T")){ // received temperature and controls RH
        if(value > 50){
            trafficParam = 1;
        }else{
            trafficParam = 5;
        }
        EV << "This network measures temperature and has received the value " << value << " from the relative humidity sensors.\n";
    }else{
        error("Network types (%s, %s) not recognized", networkType, type);

    }

    if(networkTrafficParam != trafficParam){
        char payload[10];
        networkTrafficParam = trafficParam;
        sprintf(payload, "%lf", networkTrafficParam);

        cPacket *pkt = new cPacket(payload, UPDATE_PARAM);
        sendUSB(pkt);
    }

    // logs received message
    emit(rcvdPkSignal, msg);
    EV << "Received packet: " << UDPSocket::getReceivedPacketInfo(PK(msg)) << endl;
    delete msg;
    numReceivedUDP++;
}

void UDPBridgeEG::sendUSB(cPacket *pkt)
{
    send(pkt, USBLayerOut);
}

