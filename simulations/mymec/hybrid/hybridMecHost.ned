//
//                           Simu5G
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

package simu5g.simulations.mymec.hybrid;

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



network hybridMecHost
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
            @display("p=339.78,184.0475;is=l");
        }
        gNodeB1: gNodeB {
            @display("p=211.35126,134.49625;is=vl");
        }
        gNodeB2: gNodeB {
            @display("p=211.35126,264.9475;is=vl");
        }
        gNodeB3: gNodeB {
            @display("p=211.35126,392.365;is=vl");
        }
        car[numCars]: NRUe {
            @display("p=140.56375,264.9475");
        }
        //# MEC module
        mecHost1: MECHost {
            @display("p=331.69,114.27125;is=l");
        }
        mecHost1_1: MECHost {
            @display("p=393.37625,63.70875;is=l");
        }
        mecHost1_2: MECHost {
            @display("p=464.16376,75.84375;is=l");
        }
        mecHost1_3: MECHost {
            @display("p=526.86127,104.15875;is=l");
        }
        mecHost2: MECHost {
            @display("p=339.78,276.07126;is=l");
        }
        mecHost2_1: MECHost {
            @display("p=494.50125,306.40875;is=l");
        }
        mecHost2_2: MECHost {
            @display("p=543.04126,254.835;is=l");
        }
        mecHost2_3: MECHost {
            @display("p=414.6125,276.07126;is=l");
        }
        mecHost3_1: MECHost {
            @display("p=393.37625,372.14;is=l");
        }
        mecHost3_2: MECHost {
            @display("p=451.0175,431.80374;is=l");
        }
        mecHost3_3: MECHost {
            @display("p=519.78253,392.365;is=l");
        }
        mecHost3: MECHost {
            @display("p=313.4875,404.5;is=l");
        }
        mecOrchestrator: MecOrchestrator {
            @display("p=469.22,197.19376;is=l");
        }

        ualcmp: UALCMP {
            @display("p=398.4325,190.115;i=device/smallrouter");
        }


    connections allowunconnected:

        upf.pppg++ <--> Eth10G <--> gNodeB1.ppp;
        upf.pppg++ <--> Eth10G <--> gNodeB2.ppp;
        upf.pppg++ <--> Eth10G <--> gNodeB3.ppp;
        ualcmp.ppp++ <--> Eth10G <--> upf.filterGate;

        ualcmp.toMecOrchestrator --> mecOrchestrator.fromUALCMP;
        ualcmp.fromMecOrchestrator <-- mecOrchestrator.toUALCMP;

        //#	mec
        // direct connection between gNodeB1 and mecHost
        mecHost1.pppENB++ <--> Eth10G <--> gNodeB1.pppMEHost;
        mecHost2.pppENB++ <--> Eth10G <--> gNodeB2.pppMEHost;
        mecHost3.pppENB++ <--> Eth10G <--> gNodeB3.pppMEHost;
        mecHost1_1.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        mecHost1_2.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        mecHost1_3.ppp++ <--> Eth10G <--> mecHost1.ppp++;
        mecHost2_1.ppp++ <--> Eth10G <--> mecHost2.ppp++;
        mecHost2_2.ppp++ <--> Eth10G <--> mecHost2.ppp++;
        mecHost2_3.ppp++ <--> Eth10G <--> mecHost2.ppp++;
        mecHost3_1.ppp++ <--> Eth10G <--> mecHost3.ppp++;
        mecHost3_2.ppp++ <--> Eth10G <--> mecHost3.ppp++;
        mecHost3_3.ppp++ <--> Eth10G <--> mecHost3.ppp++;


}
