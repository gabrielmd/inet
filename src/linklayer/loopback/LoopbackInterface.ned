//
// Copyright (C) 2005 Andras Varga
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

import inet.base.IHook;
import inet.linklayer.INic;
import inet.linklayer.ITrafficConditioner;


//
// Loopback interface. Complements the ~LoopbackMac module.
//
module LoopbackInterface like INic
{
    parameters:
        @display("i=block/ifcard;bgb=214,249;bgl=53");
        string ingressTCType = default(""); // a module type implementing ~ITrafficConditioner for optional traffic conditioning of incoming traffic
        string egressTCType = default(""); // a module type implementing ~ITrafficConditioner for optional traffic conditioning of outgoing traffic
        int numOutputHooks = default(0);
        int numInputHooks = default(0);
    gates:
        input upperLayerIn;
        output upperLayerOut;
    submodules:
        outputHook[numOutputHooks]: <default("Nop")> like IHook if numOutputHooks>0 {
            @display("p=42,33");
        }
        inputHook[numInputHooks]: <default("Nop")> like IHook if numInputHooks>0 {
            @display("p=177,33");
        }
        ingressTC: <ingressTCType> like ITrafficConditioner if ingressTCType != "" {
            @display("p=177,100");
        }
        egressTC: <egressTCType> like ITrafficConditioner if egressTCType != "" {
            @display("p=42,100");
        }
        mac: LoopbackMac {
            parameters:
                @display("p=98,207");
        }
    connections:
        upperLayerIn --> { @display("m=n"); } --> outputHook[0].in if numOutputHooks > 0;
        upperLayerIn --> { @display("m=n"); } --> egressTC.in if numOutputHooks == 0 && egressTCType != "";
        upperLayerIn --> { @display("m=n"); } --> mac.netwIn if numOutputHooks == 0 && egressTCType == "";

        egressTC.out --> mac.netwIn if egressTCType != "";

        for i=0..numOutputHooks-2 {
            outputHook[i].out --> outputHook[i+1].in;
        }
        outputHook[numOutputHooks-1].out --> egressTC.in if numOutputHooks > 0 && egressTCType != "";
        outputHook[numOutputHooks-1].out --> mac.netwIn if numOutputHooks > 0 && egressTCType == "";

        mac.netwOut --> { @display("m=n"); } --> upperLayerOut if numInputHooks == 0 && ingressTCType == "";

        mac.netwOut --> ingressTC.in if ingressTCType != "";
        ingressTC.out --> { @display("m=n"); } --> upperLayerOut if ingressTCType != "" && numInputHooks == 0;

        mac.netwOut --> inputHook[0].in if numInputHooks > 0 && ingressTCType == "";
        ingressTC.out --> inputHook[0].in if numInputHooks > 0 && ingressTCType != "";

        for i=0..numInputHooks-2 {
            inputHook[i].out --> inputHook[i+1].in;
        }
        inputHook[numInputHooks-1].out --> { @display("m=n"); } --> upperLayerOut if numInputHooks > 0;
}

