package inet.examples.inet.localudp;

import inet.nodes.inet.StandardHost;


network LocalUDP
{
    submodules:
        host: StandardHost {
            parameters:
                @display("i=device/pc3");
        }
}

