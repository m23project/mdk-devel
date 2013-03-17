#!/bin/bash

#Check for needed directories and create them when needed
[ -d /dev ] || mkdir -m 0755 /dev
[ -d /root ] || mkdir -m 0700 /root
[ -d /sys ] || mkdir /sys
[ -d /proc ] || mkdir /proc
[ -d /tmp ] || mkdir /tmp
mkdir -p /var/lock
mkdir /dev/pts
[ -e /dev/console ] || mknod -m 0600 /dev/console c 5 1
[ -e /dev/null ] || mknod /dev/null c 1 3

# Mount basic system directories
mount -t sysfs -o nodev,noexec,nosuid sysfs /sys
mount -t proc -o nodev,noexec,nosuid proc /proc
mount -t devpts -o noexec,nosuid,gid=5,mode=0620 devpts /dev/pts

#Make mdev the firmware loader
echo /sbin/mdev > /proc/sys/kernel/hotplug

#Suppress (some) kernel warnings
echo "0" > /proc/sys/kernel/printk

#Load the USB modules
modprobe ehci-hcd
modprobe ohci-hcd
modprobe uhci-hcd
modprobe xhci-hcd

#Load the USB keyboard modules
modprobe usbhid
modprobe hid

#Make device nodes for USB keyboard
mkdir -p /dev/usb/hid
mknod /dev/usb/hid/hiddev0 c 180 96
mknod /dev/usb/hid/hiddev1 c 180 97
mknod /dev/usb/hid/hiddev2 c 180 98
mknod /dev/usb/hid/hiddev3 c 180 99