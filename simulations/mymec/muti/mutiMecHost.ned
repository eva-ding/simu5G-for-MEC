//
//                           Simu5G
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

package simu5g.simulations.mymec.muti;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import simu5g.world.radio.LteChannelControl;
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.mec.MECHost;

import simu5g.nodes.mec.MECOrchestrator.MecOrchestrator;
import simu5g.nodes.mec.UALCMP.UALCMP;



network mutiMecHost
{
    parameters:

        int numCars = default(0);

        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=805.92,499.56");

    submodules:

        routingRecorder: RoutingTableRecorder {
            @display("p=690,104.88;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=690,184.92");
        }
        channelControl: LteChannelControl {
            @display("p=690,24.84;is=s");
        }
        binder: Binder {
            @display("p=690,264.96;is=s");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=691.38,342.24;is=s");
        }
        upf: Upf {
            @display("p=425.73624,249.77875;is=l");
        }
        gNodeB1: gNodeB {
            @display("p=140,250;is=vl");
        }
        car[numCars]: NRUe {
            @display("p=300,300");
        }
        //# MEC module
        mecHost1: MECHost {
            @display("p=402.4775,71.79875;is=l");
        }
        mecHost2: MECHost {
            @display("p=479.3325,32.36;is=l");
        }
        mecHost3: MECHost {
            @display("p=479.3325,184.0475;is=l");
        }
        mecHost4: MECHost {
            @display("p=544.0525,115.2825;is=l");
        }
        mecOrchestrator: MecOrchestrator {
            @display("p=564.2775,249.77875;is=l");
        }

        ualcmp: UALCMP {
            @display("p=490.45624,248.7675;i=device/smallrouter");
        }


    connections allowunconnected:

        upf.pppg++ <--> Eth10G <--> gNodeB1.ppp;
        ualcmp.ppp++ <--> Eth10G <--> upf.filterGate;

        ualcmp.toMecOrchestrator --> mecOrchestrator.fromUALCMP;
        ualcmp.fromMecOrchestrator <-- mecOrchestrator.toUALCMP;

        //#	mec
        // direct connection between gNodeB1 and mecHost
        mecHost1.pppENB++ <--> Eth10G <--> gNodeB1.pppMEHost;
        mecHost2.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        mecHost3.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        mecHost4.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        
}
