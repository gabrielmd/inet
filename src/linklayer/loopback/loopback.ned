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



package inet.linklayer.loopback;

//
// loopback interface mac module implementation.
//
// @see ~LoopbackInterface
//
simple LoopbackMac
{
    parameters:
        int mtu @unit("B") = default(4470B);
        @display("i=block/rxtx");

        @signal[dropPkBitError](type=cPacket);
        @signal[packetSentToUpper](type=cPacket);
        @signal[packetReceivedFromUpper](type=cPacket);
        @statistic[droppedPkBitError](title="packets dropped/bit error"; source=dropPkBitError; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[rcvdPkFromHL](title="packets received from higher layer"; source=packetReceivedFromUpper; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[passedUpPk](title="packets passed to higher layer"; source=packetSentToUpper; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);

    gates:
        input netwIn;
        output netwOut;
}

