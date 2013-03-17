#!/bin/bash

# (C) Hauke Goos-Habermann <hhabermann@pc-kiel.de> 2007
# License: GPL V2 or later

# READ first!!!!!!
#
# Purpose: This script setups network interfaces for VirtualBox to allow the guest machines to have access to the host machine and among
# themselves. It tries to figure out most of the essential network settings of your current configuration, but there are two variables
# you have to set on your own.
#
# This are:
# * user
# * vmamount
#
# When you start the script an information box with the detected values will be shown. If you detect errors in these values you have to
# change them by entering the correct value into the variable.
#
# E.g. the detected gateway is wrong. Then you have to exchange the line
#	gateway=`echo $gatewayDevice | cut -d' ' -f2`
# with
#	gateway="<my gateway IP>"
#
# Afterwards you can start the script again and should get a working network configuration for the VirtualBox guests.




###################EDIT HERE###################

# get the gateway and the device that has connection to the gateway
gatewayDevice=`route -n | grep "^0.0.0.0" | tr -s [:blank:]`
# gateway
gateway=`echo $gatewayDevice | cut -d' ' -f2`
# device that has connection to the internet
device=`echo $gatewayDevice | cut -d' ' -f8`
# host ip of the device that has connection to the internet
hostIP=`ifconfig $device | grep "inet " | cut -d':' -f2 | cut -d' ' -f1`
# user VirtualBox is run under
user=dodger
# amount of virtual machines that should have 
vmamount=20
# bridge device to create
bridgedev=br0
# The VirtualBox clients may need explicit access to the host system if it is used as gateway and an iptables firewall is active there.
# If you leave this variable empty ("") no holes are hit into your firewall. Notation is a combinaton of the network adress and the
# netmask in short form. E.g. the VirtualBox client uses the IP 192.168.1.23 its network address is 192.168.1.0 and its netmask is
# 255.255.255.0. The short form would be 192.168.1.0/24. Now all IPs (192.168.1.1 - 192.168.1.254) have access.
#vmNetworkNetmask="192.168.1.0/24"
disableScript="/tmp/VirtualBox-networking-disable.sh"

###############EDITABLE PART END###############


allInterfaces=""

if [ `whoami` != "root" ]
then
	echo ">> Sorry, this script must be run as root! Aborting!"
	exit 1
fi

echo "

>> Setup network devices for VirtualBox guests

* Bridge device to create: $bridgedev
* Device with access to your local network and/or internet: $device
* IP of the VirtualBox host: $hostIP
* Gateway: $gateway
* User VirtualBox is run under: $user
* Amount of VirtualBox guests: $vmamount
* The script for disabling the VirtualBox networking will be stored in $disableScript

This script will create the following devices that can be used in VirtualBox:"
tapnr=0
devCreated=0

echo "#!/bin/sh
ifconfig $bridgedev down
brctl delbr $bridgedev
ifconfig $device $hostIP
route add -net default gw $gateway
" >$disableScript

chmod +x $disableScript

while [ $devCreated -lt $vmamount ]
do
	ifconfig "tap$tapnr" 1> /dev/null 2> /dev/null
	if [ $? -eq 1 ]
	then
		echo -n "tap$tapnr "
		#echo "tunctl -d tap$tapnr" >> $disableScript
		echo "openvpn --rmtun --dev tap$tapnr" >> $disableScript
		allInterfaces="$allInterfaces tap$tapnr"
		devCreated=$[$devCreated + 1]
	fi
	tapnr=$[$tapnr + 1]
done
echo "

Do you want to proceed and create the devices and the bridge (y/n)?"

if [ $# -gt 0 ] && [ $1 = "noninteractive" ]
then
	key="y"
else
	read key
fi

if [ $key != "y" ]
then
	echo ">> Devices and bridge creation aborted!"
	exit 1
fi

modprobe tun
modprobe bridging
mknod /dev/net/tun c 10 200
echo 1024 > /proc/sys/dev/rtc/max-user-freq


#create the tapX devices
for tapDev in $allInterfaces
do
	#tunctl -t $tapDev -u $user
	openvpn --mktun --dev $tapDev
done

#create the bridge
brctl addbr $bridgedev

brctl stp $bridgedev off
brctl setfd $bridgedev 1
brctl sethello $bridgedev 1

#all packets on the network will be received by $device
ifconfig $device 0.0.0.0 promisc

#add the local device with connection to the network to the bridge
brctl addif $bridgedev $device

#set the same IP on the bridge and local device
ifconfig $bridgedev $hostIP

#ifconfig $device 0.0.0.0 promisc up
ifconfig $device $hostIP

echo 1 > /proc/sys/net/ipv4/conf/$device/proxy_arp

#re-add the gateway because it was lost when giving $device an IP
route add -net default gw $gateway

for tapDev in $allInterfaces
do
	#bring up the tapX device
	ifconfig $tapDev 0.0.0.0 promisc up
	#add it to the bridge
	brctl addif $bridgedev $tapDev
	
	echo 1 > /proc/sys/net/ipv4/conf/$tapDev/proxy_arp
done

arp -Ds $hostIP $device pub

#allow all users read and write access to the network device
chmod 666 /dev/net/tun

#make holes in the firewall
if test "$vmNetworkNetmask"
then
	for i in $allInterfaces $bridgedev
	do
		/sbin/iptables -A INPUT -i $i -s $vmNetworkNetmask -j ACCEPT
		/sbin/iptables -A FORWARD -i $i -s $vmNetworkNetmask -j ACCEPT
		/sbin/iptables -A FORWARD -i $i -d $vmNetworkNetmask -j ACCEPT
	done
fi
