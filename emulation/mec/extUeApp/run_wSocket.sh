#!/bin/bash
# make sure you run '. setenv' in the Simu5G root directory before running this script


# create virtual ethernet link: veth0 <--> veth1
sudo ip link add veth0 type veth peer name veth1

# veth0 <--> veth1 link uses 192.168.2.x addresses;
sudo ip addr add 192.168.3.2 dev veth1

# bring up both interfaces
sudo ip link set veth0 up
sudo ip link set veth1 up


# add routes for new link
sudo route add -net 192.168.4.0 netmask 255.255.255.0 dev veth1     # to device App
sudo route add -net 192.168.3.0 netmask 255.255.255.0 dev veth1     # enables backward path to the simulation
sudo route add -net 10.0.8.0 netmask 255.255.255.0 dev veth1     # to MEC app

sudo ethtool --offload veth1 rx off tx off # disable TCP checksum offloading to make sure that TCP checksum is actually calculated

# run simulation
simu5g -u Qtenv -c ExtUEApp_Socket

# destroy virtual ethernet link
sudo ip link del veth0
