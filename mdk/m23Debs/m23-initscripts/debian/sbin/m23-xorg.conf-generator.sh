#!/bin/bash
#Generator for xorg.conf with 4 different methods


ubuntuJockey()
{
	apt-get --force-yes -y -q install jockey-common
	jockey-text -u
	jockey-text -l | cut -d' ' -f1 | xargs -n1 jockey-text -e
}



#Waits and loops until NO dpkg/apt-get/aptitude/adept process is running
waitForFreeLock()
{
	#If lsof is not available make a pause of 10 seconds
	if [ `type lsof &> /dev/null; echo $?` -ne 0 ]
	then
		sleep 10
	else
		while [ `lsof -n 2> /dev/null | grep /var/lib/dpkg/lock | wc -l` -gt 0 ]
		do
			sleep 5
		done
	fi
}





#Fetches the VBoxLinuxAdditions.run self-extracting archive.
fetchVBoxLinuxAdditions()
{
	#Get architecture for naming the VirtualBox addon package
	arch=`uname -m | sed 's/x86_64/amd64/' | sed 's/i[3-9].*/x86/'`
	serverIP=`egrep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" /etc/init.d/m23fetchjob | cut -d' ' -f2`

	mkdir -p /usr/m23-vboxAddon
	
	#Try to detect VBox's host version
	hostVer=`VBoxControl guestproperty get /VirtualBox/HostInfo/VBoxVer 2> /dev/null | grep Value | cut -d' ' -f2`
	echo $hostVer > /tmp/vbold.txt

	if [ $hostVer ]
	then
		echo -n "Try to fetch VBoxLinuxAdditions-$arch-$hostVer.run from the m23 server... "
		wget -q http://$serverIP/extraDebs/VBoxLinuxAdditions-$arch-$hostVer.run -O /usr/m23-vboxAddon/VBoxLinuxAdditions.run2
		ret=$?

		if [ $ret -ne 0 ]
		then
			echo "fail"
		fi
	fi

	#If there was an error fetching the *.run => Try without the VBox's host version (if it is set)
	if [ $ret -ne 0 ] || [ -z $hostVer ]
	then
		echo -n "Try to fetch VBoxLinuxAdditions-$arch.run from the m23 server... "
		wget -q http://$serverIP/extraDebs/VBoxLinuxAdditions-$arch.run -O /usr/m23-vboxAddon/VBoxLinuxAdditions.run2
		ret=$?
	fi

	#Check if the fetching of  VBoxLinuxAdditions from the m23 server worked
	if [ $ret -ne 0 ]
	then
		echo "fail"

		rm /tmp/vblast.txt 2> /dev/null

		#If there could be found the VBox's host version => Use it instead of the latest release
		if [ $hostVer ]
		then
			echo $hostVer > /tmp/vblast.txt
		else
			#Get the number of the latest version
			wget -q http://download.virtualbox.org/virtualbox/LATEST.TXT -O /tmp/vblast.txt
		fi

		echo -n "Try to fetch VBoxLinuxAdditions-$arch.run (`cat /tmp/vblast.txt`) directly from the net... "

		#Get the index of the directory
		wget -q http://download.virtualbox.org/virtualbox/`cat /tmp/vblast.txt`/ -O /tmp/vbIndex.txt

		#Get the name of the ISO file
		isoFile=`grep iso /tmp/vbIndex.txt | cut -d'"' -f2 | sort -n | tail -1`

		#Download the ISO
		wget -q http://download.virtualbox.org/virtualbox/`cat /tmp/vblast.txt`/$isoFile -O /usr/m23-vboxAddon/VBoxGuestAdditions.iso

		if [ $? -eq 0 ]
		then
			echo "done"

			#Mount the ISO
			mkdir -p /mnt/vbox-loop
			mount -o loop /usr/m23-vboxAddon/VBoxGuestAdditions.iso /mnt/vbox-loop

			#Get architecture for naming the VirtualBox addon package
			arch=`uname -m | sed 's/x86_64/amd64/' | sed 's/i[3-9].*/x86/'`

			if [ -f /mnt/vbox-loop/VBoxLinuxAdditions-$arch.run ]
			then
				#Copy the addons for the architecture
				cp /mnt/vbox-loop/VBoxLinuxAdditions-$arch.run /usr/m23-vboxAddon/VBoxLinuxAdditions.run
			else
				cp /mnt/vbox-loop/VBoxLinuxAdditions.run /usr/m23-vboxAddon/VBoxLinuxAdditions.run
			fi

			#Unmount the ISO
			umount /mnt/vbox-loop

			#Delete the ISO after download
			rm /usr/m23-vboxAddon/VBoxGuestAdditions.iso
		else
			echo "fail"
		fi
	else
		mv /usr/m23-vboxAddon/VBoxLinuxAdditions.run2 /usr/m23-vboxAddon/VBoxLinuxAdditions.run
		echo "done"
	fi
	
	#Make it executable
	chmod +x /usr/m23-vboxAddon/VBoxLinuxAdditions.run
}





#Exits, if xorg.conf with working screen section exists
xorgThereSoStop()
{
	if [ -f /etc/X11/xorg.conf ] && [ `grep Screen /etc/X11/xorg.conf -c` -gt 0 ]
	then
		touch /etc/sysconfig/vbox /etc/sysconfig2/vbox
		echo ">>>>Adding /tmp/reboot" >> /var/log/m23-xorg.conf-generator.log
		echo "yes" > /tmp/reboot
		checkNoXorgConfNeeded
		exit 0
	fi
}





#Hacks for running X on Ubuntu
ubuntuHacks()
{
	#remove /usr/X11R6/lib/modules from /etc/X11/XF86Config-4 on Ubuntu 6.06 and 6.10. Otherwise X wouldn't start.
	if [ `egrep -c "(Ubuntu 6.10|Ubuntu 6.06)" /etc/issue 2>/dev/null` -gt 0 ] || [ -d /usr/lib/xorg/modules ]
	then
			sed '/\/usr\/X11R6\/lib\/modules/d' /etc/X11/XF86Config-4  > /tmp/XF86Config-4
			mv /tmp/XF86Config-4 /etc/X11/XF86Config-4
			chmod 644 /etc/X11/XF86Config-4
			chown root /etc/X11/XF86Config-4
			chgrp root /etc/X11/XF86Config-4
	fi
}





#Insert 24 bit default depth in /etc/X11/xorg.conf
insertDefaultDepth()
{

#Find the line containing "Screen"
export m23searchLine=`awk -v START=0 -v SEARCH="Screen" 'match($0,SEARCH)&&NR>START {print NR; exit}' /etc/X11/xorg.conf`


if test `awk -v RS='pheeSheiso2sieseegaxeekeitoongei' -v SEARCH="	DefaultDepth	24" '
BEGIN { FOUND=0;}
{if (index($0, SEARCH) > 0) { FOUND++; }}
END {print(FOUND);}
' /etc/X11/xorg.conf` -lt 1
then
	
userGroup=`find /etc/X11/xorg.conf -printf "chown %u.%g /etc/X11/xorg.conf"`
perm=`find /etc/X11/xorg.conf -printf "chmod %m /etc/X11/xorg.conf"`

awk -v LINENR=$m23searchLine -v INSERT="	DefaultDepth	24" -v ADD=1 '
BEGIN {LINENR+=ADD}
NR==LINENR {print INSERT"\n"$0}
NR!=LINENR {print $0}' /etc/X11/xorg.conf > /etc/X11/xorg.conf#m23

mv /etc/X11/xorg.conf#m23 /etc/X11/xorg.conf

$userGroup
$perm

fi
}




#Run the mkxf86config or mkxorgconfig script to create X11 configurations
configXOrgKnoppix()
{
	echo "Run the mkxf86config or mkxorgconfig script to create X11 configurations" | tee -a /var/log/m23-xorg.conf-generator.log

	if [ `dpkg --get-selections | grep -c xserver-xorg-core` -eq 0 ]
	then
		waitForFreeLock
		apt-get --force-yes -y -q install xf86config-knoppix
	else
		waitForFreeLock
		apt-get --force-yes -y -q install xorgconfig-knoppix
	fi

	#detect what X11 system is used: XFree86 or XOrg
	[ -f /usr/sbin/mkxf86config ] && X11=f
	[ -f /usr/sbin/mkxorgconfig ] && X11=o

	#run the needed configuration file generator
	case $X11 in
		f)
			/usr/sbin/mkxf86config
			;;
		o)
			/usr/sbin/mkxorgconfig
			;;
		*)
			return
			;;
	esac

	ubuntuHacks
}





#Create xorg.conf via dpkg
configXOrgDebconf() {
#Include the file for keyboard language ($XKEYBOARD)
. /etc/sysconfig/keyboard

xVer=`Xorg -version 2>&1 | grep X.Org | cut -d' ' -f4`

if dpkg --compare-versions $xVer ge 1.4.0
then
	echo "Create xorg.conf via dpkg" | tee -a /var/log/m23-xorg.conf-generator.log

	#configure debconf settings for Xorg
	echo "
	xserver-xorg xserver-xorg/config/doublequote_in_string_error note
xserver-xorg xserver-xorg/config/device/bus_id_error note
xserver-xorg xserver-xorg/config/inputdevice/keyboard/options string
xserver-xorg xserver-xorg/autodetect_keyboard boolean false
xserver-xorg xserver-xorg/config/device/use_fbdev boolean false
xserver-xorg xserver-xorg/config/inputdevice/keyboard/variant string
xserver-xorg xserver-xorg/config/nonnumeric_string_error note
xserver-xorg xserver-xorg/config/inputdevice/keyboard/layout string $XKEYBOARD
xserver-xorg xserver-xorg/config/inputdevice/keyboard/model string pc104
xserver-xorg xserver-xorg/config/device/driver select
xserver-xorg xserver-xorg/config/null_string_error note
xserver-xorg xserver-xorg/config/device/bus_id string
xserver-xorg xserver-xorg/config/inputdevice/keyboard/rules string xorg" > /tmp/xorg.debconf
	debconf-set-selections /tmp/xorg.debconf

	#remove xorg.conf
	rm /etc/X11/xorg.conf /tmp/xorg.debconf

	#remove the creator script for xorg.conf
	apt-get remove --force-yes -y -q xorgconfig-knoppix

	#let xorg.conf get created by dexconf
	if dexconf
	then
		echo "xorg.conf created by dexconf"
	else
		#let xorg.conf get created by the script in xserver-xorg
		dpkg-reconfigure xserver-xorg 2>&1 | tee -a /var/log/m23-xorg.conf-generator.log
	fi
	
	if [ ! -f /etc/X11/xorg.conf ] || [ `grep Screen /etc/X11/xorg.conf -c` -lt 2 ]
	then
		rm /etc/X11/xorg.conf
		dpkg-reconfigure -phigh xserver-xorg 2>&1 | tee -a /var/log/m23-xorg.conf-generator.log
	fi

	insertDefaultDepth
fi
}





#Create xorg.conf via Xorg -configure
configXOrgconfigure() {

	echo "Create xorg.conf via Xorg -configure" | tee -a /var/log/m23-xorg.conf-generator.log

#Include the file for keyboard language ($XKEYBOARD)
. /etc/sysconfig/keyboard

	#Get the init script with full path (e.g. /etc/init.d/kdm)
	dm=$(grep $(cat /etc/X11/default-display-manager 2> /dev/null) /etc/init.d/* | cut -d':' -f1)

	#Check, if the scripts are converted to upstart
	if [ -z "$dm" ]
	then
		dm=$(find /etc/init.d/ | grep $(basename $(cat /etc/X11/default-display-manager 2> /dev/null)))
	fi

	#Stop the display manager
	if [ "$dm" ]
	then
		$dm stop
	fi
	
	killall -9 Xorg


echo ">>>>>>>>>>>>dm: $dm" >> /var/log/m23-xorg.conf-generator.log
echo ">>>>>>>>>>>>XKEYBOARD: $XKEYBOARD" >> /var/log/m23-xorg.conf-generator.log

	#Try to use the Xorg automatic detection
	Xorg -configure 2>&1 | tee -a /var/log/m23-xorg.conf-generator.log

echo ">>>>>>>>>>>>$HOME/xorg.conf.new" >> /var/log/m23-xorg.conf-generator.log
cat /root/xorg.conf.new >> /var/log/m23-xorg.conf-generator.log

	#Remove the frambuffer driver and add keyboard language setting via awk
	sed 's|Driver	  "fbdev"|#Driver	  "fbdev"|g' $HOME/xorg.conf.new | awk -v LANG="$XKEYBOARD" '
{
print
}

/"kbd"/ {
print(" Option  \"XkbRules\"	\"xorg\"\n\
		Option  \"XkbLayout\"	\""LANG"\"\
");
}
' > /etc/X11/xorg.conf

echo ">>>>>>>>>>>>/etc/X11/xorg.conf" >> /var/log/m23-xorg.conf-generator.log
cat /etc/X11/xorg.conf >> /var/log/m23-xorg.conf-generator.log

}





#Count the packages found by name
pkgAvailable()
{
	apt-cache search "$1" | grep "^$1 -" -c
}





#Check a list of package names and return the first available package name
checkPkgAvail()
{
for pkg in $@
do
	if [ $(apt-cache search "$pkg" | grep "^$pkg -" -c) -ne 0 ]
	then
		echo $pkg
		break
	fi
done
}





#Does basic configuration for VirtualBox
configXOrgVBox()
{

#Check if the m23 client is run in VirtualBox and try to install the VirtualBox guest addons
if [ `lspci | grep -c VirtualBox` -gt 0 ]
then
	#Make sure the log file doesn't exist
	rm /var/log/m23-VBox-Addon-Install.log 2> /dev/null

	echo "Running in VirtualBox => Configuring for it" | tee -a /var/log/m23-VBox-Addon-Install.log
	mkdir -p /etc/xdg/autostart

	kernelVer=`ls /lib/modules/ | grep -v m23 | cut -d'-' -f1 | tail -1`
	export DEBIAN_FRONTEND=noninteractive

	#Add temporaryly line to force accepting the new configuration files
	echo "force-confnew" >> /etc/dpkg/dpkg.cfg

	waitForFreeLock
	(apt-get --force-yes -y -q update 2>&1) | cat >> /var/log/m23-VBox-Addon-Install.log

	waitForFreeLock
	apt-get --force-yes -y -q install lsof

	waitForFreeLock
	(apt-get --force-yes -y -q install bzip2 linux-headers-`uname -r` build-essential 2>&1; ret=$?) | cat >> /var/log/m23-VBox-Addon-Install.log

	echo -n "Checking if there is a special VirtualBox package: " | tee -a /var/log/m23-VBox-Addon-Install.log

	if [ $(pkgAvailable "vbox-module-$kernelVer") -ne 0 ]
	then
		waitForFreeLock
		#Try to install the special VirtualBox package
		apt-get --force-yes -y -q install vbox-module-$kernelVer &> /dev/null
		ret=$?
	else
		ret=23
	fi

	#Check if there was an error installing it
	if [ $ret -ne 0 ]
	then
		echo "no" | tee -a /var/log/m23-VBox-Addon-Install.log

		echo -n "Installing kernel headers for compiling the VirtualBox addons... " | tee -a /var/log/m23-VBox-Addon-Install.log

		#Install the kernel headers (needed for compiling the VirtualBox modules)
		headerPackage=`dpkg --get-selections | egrep  linux-image-.*-.* | sed 's/\\t.*//' | sed 's/image/headers/'`
		waitForFreeLock
		(apt-get --force-yes -y -q install $headerPackage 2>&1) | cat >> /var/log/m23-VBox-Addon-Install.log

		echo "done" | tee -a /var/log/m23-VBox-Addon-Install.log
		echo -n "Try to installing the VirtualBox addons from packages... " | tee -a /var/log/m23-VBox-Addon-Install.log

		#Get the name of the guest X11 package
		guestx11=$(checkPkgAvail virtualbox-guest-x11 virtualbox-ose-guest-x11)

		#Get the name of the guest DKMS package
		guestDkms=$(checkPkgAvail virtualbox-guest-dkms virtualbox-ose-guest-dkms)

		#Get the name of the guest source package
		guestSource=$(checkPkgAvail virtualbox-guest-source virtualbox-ose-guest-source)

		#Now try to install the "official" package from the distribution
		waitForFreeLock
		(apt-get --force-yes -y -q install $guestx11 virtualbox-ose-guest-source 2>&1) | cat >> /var/log/m23-VBox-Addon-Install.log

		#moves the postinst out of the way, because it will fail if we use another kernel (boot kernel) that the system kernel
		mv /var/lib/dpkg/info/$guestSource.postinst /tmp 2> /dev/null

		waitForFreeLock
		apt-get --force-yes -y -q install $guestx11 &> /dev/null

		waitForFreeLock
		(apt-get --force-yes -y -q install $guestDkms 2>&1) | cat >> /var/log/m23-VBox-Addon-Install.log

		#Check, if the VBox packages are installed
		[ `dpkg --get-selections | grep $guestx11 -c` -gt 0 ]
		ret=$?

		#Make sure the kernel module was built
		waitForFreeLock
		dpkg-reconfigure $guestDkms

		mv /tmp/$guestSource.postinst /var/lib/dpkg/info 2> /dev/null

		if [ $ret -ne 0 ]
		then
			#Installation from the package failed => Try to fetch VBoxLinuxAdditions-$arch.run from the m23 server
			echo "fail" | tee -a /var/log/m23-VBox-Addon-Install.log

			fetchVBoxLinuxAdditions
		else
			echo "yes" > /tmp/reboot
			echo "done" | tee -a /var/log/m23-VBox-Addon-Install.log
		fi
	else
		echo "yes" | tee -a /var/log/m23-VBox-Addon-Install.log
		echo "yes" > /tmp/reboot
	fi

	if [ -e /usr/m23-vboxAddon/VBoxLinuxAdditions.run ]
	then
		cd /tmp
		/usr/m23-vboxAddon/VBoxLinuxAdditions.run | tee -a /var/log/m23-VBox-Addon-Install.log

		#The file is a symlink to the VirtualBox directory that contains the version number (e.g. /opt/VBoxGuestAdditions-3.1.4/lib/VBoxOGL.so)
		if [ -f /usr/lib/VBoxOGL.so ] && [ -z `cat /tmp/vbold.txt` ]
		then
			#Get the version number this way to save another installation round
			find /usr/lib/VBoxOGL.so -printf %l | cut -d'/' -f3 | sed 's/VBoxGuestAdditions-//' > /tmp/vbold.txt
		fi

		#Check if the VBhost version of the previous run of fetchVBoxLinuxAdditions was different (or not set (in case of VBoxControl not being there)) and download and run the addon again then
		if [ -z `cat /tmp/vbold.txt` ] || [ ! `VBoxControl guestproperty get /VirtualBox/HostInfo/VBoxVer 2> /dev/null | grep Value | cut -d' ' -f2` = `cat /tmp/vbold.txt` ]
		then
			fetchVBoxLinuxAdditions
			/usr/m23-vboxAddon/VBoxLinuxAdditions.run | tee -a /var/log/m23-VBox-Addon-Install.log
		fi
	fi

	#Remove the line again
	grep -v force-confnew /etc/dpkg/dpkg.cfg > /tmp/dpkg.cfg
	cat /tmp/dpkg.cfg > /etc/dpkg/dpkg.cfg
	rm /tmp/dpkg.cfg

	#Disable writing of xorg.conf on LinuxMint 18, because X will report "no screens found"
	if [ $(grep xenial -c /etc/apt/sources.list) -gt 0 ] && [ $(lsb_release -i -s) = 'LinuxMint' ]
	then
		touch /etc/sysconfig/disableConfig
		exit 0
	fi

	if [ -e /usr/lib/dri/vboxvideo_dri.so ] || [ -e /usr/lib/xorg/modules/drivers/vboxvideo_drv.so ]
	then
		echo "VirtualBox addons were installed => X11 is now optimised for VirtualBox" | tee -a /var/log/m23-VBox-Addon-Install.log
		configXOrgBasicConfig vboxvideo vboxmouse
		echo "yes" > /tmp/reboot
	else
		echo "Error when installing the VirtualBox addons => X11 is now configured with standard settings" | tee -a /var/log/m23-VBox-Addon-Install.log
		rm /etc/X11/xorg.conf 2> /dev/null
		configXOrgKnoppix
		xorgThereSoStop
	fi
fi
}





#Does basic configuration for VMWare
configXOrgVMware()
{
	#Check if the m23 client is run in VMWare and try to install the VMware X11 driver
	if [ `dmesg | grep VMware -c` -gt 0 ] && [ `dmesg | grep VBOX -c` -eq 0 ] && [ `dmesg | grep VMware | grep vmxnet3 -c` -eq 0 ]
	then
		waitForFreeLock
		apt-get --force-yes -y -q install xserver-xorg-video-vmware
		waitForFreeLock
		apt-get --force-yes -y -q install xserver-xorg-input-vmmouse
		waitForFreeLock
		apt-get --force-yes -y -q install open-vm-tools
		configXOrgBasicConfig vmware vmmouse
	fi
}





#Generates a basic xorg.conf with XServer, keyboard language and (optionally) mouse
configXOrgBasicConfig()
{
#Include the file for keyboard language ($XKEYBOARD)
. /etc/sysconfig/keyboard

rm /etc/X11/xorg.conf.beforeSed* /etc/X11/xorg.conf 2> /dev/null

#1st parameter is the XServer driver
XDriver=$1
#2nd parameter is the XMouse driver (or empty)
XMouse=$2

#Store mouse and video driver in the config files
echo $XMouse > /etc/sysconfig/vbox
echo $XDriver >> /etc/sysconfig/vbox
cp /etc/sysconfig/vbox /etc/sysconfig2/vbox

#Check if the XOrg version is newer than 1.10
# xorgv=$(Xorg -version 2>&1 | grep Server | cut -d' ' -f4)
# dpkg --compare-versions $xorgv ">" 1.10
# 
# if [ $? -eq 0 ]
# then
# 	configXOrgconfigure
# 	return
# fi

	echo "Generates a basic xorg.conf with XServer ($XDriver), keyboard language ($XKEYBOARD) and (optionally) mouses ($XMouse)" | tee -a /var/log/m23-xorg.conf-generator.log

#Check if the mouse driver section should be added
if [ $XMouse ]
then

cat >> /etc/X11/xorg.conf.beforeSed0 << "XOrgBasicConfigEOF0"
Section "ServerLayout"
	Identifier	"Layout0"
	Screen 0	"Screen0" 0 0
	InputDevice	"Keyboard0" "CoreKeyboard"
	InputDevice	"Mouse0" "CorePointer"
EndSection

Section "InputDevice"
	Identifier	"Mouse0"
	Driver		"XMOUSE"
EndSection

XOrgBasicConfigEOF0

sed "s/XMOUSE/$XMouse/g" /etc/X11/xorg.conf.beforeSed0 > /etc/X11/xorg.conf.beforeSed
fi

cat >> /etc/X11/xorg.conf.beforeSed << "XOrgBasicConfigEOF"
Section "InputDevice"
	Identifier 	"Keyboard0"
	Driver		"kbd"
	Option		"XkbLayout" "XKEYBOARD"
	Option		"XkbRules" "xorg"
EndSection

Section "Module"
	Load		"dbe"
	Load		"extmod"
	Load		"type1"
	Load		"freetype"
	Load		"glx"
EndSection

Section "Device"
	Identifier	"Configured Video Device"
	Driver		"XORGdriver"
EndSection

Section "Monitor"
	Identifier	"Configured Monitor"
EndSection

Section "Screen"
	Identifier	"Screen0"
	Monitor		"Configured Monitor"
	Device		"Configured Video Device"
	DefaultDepth	24
EndSection

Section "DRI"
	Mode		0666
EndSection
XOrgBasicConfigEOF

#Exchange XServer and keyboard layouts
sed "s/XORGdriver/$XDriver/g" /etc/X11/xorg.conf.beforeSed | sed "s/XKEYBOARD/$XKEYBOARD/g" > /etc/X11/xorg.conf
}





#Checks, if Xorg will run well without an xorg.conf
checkNoXorgConfNeeded()
{
	#Check, if Xorg is version 1.11.3 or higher
	dpkg --compare-versions $(Xorg -version 2>&1 | grep -i server | grep -i 'x.org' | cut -d' ' -f4) ">=" 1.11.3

	#If yes, no xorg.conf is needed
	if [ $? -eq 0 ]
	then
		echo "Xorg is version 1.11.3 or higher: No xorg.conf is needed" >> /var/log/m23-xorg.conf-generator.log
		rm /etc/X11/xorg.conf 2> /dev/null
		exit 0
	fi
}





#Make sure the reboot file is non-existent at the beginning
rm /tmp/reboot 2> /dev/null

configXOrgVBox
xorgThereSoStop
configXOrgVMware
xorgThereSoStop
checkNoXorgConfNeeded
configXOrgconfigure
xorgThereSoStop
configXOrgDebconf
xorgThereSoStop
configXOrgKnoppix