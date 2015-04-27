#!/bin/bash
#Removes unneeded packages from a Raspbian installation and prepares for compression

apt-get -y remove aspell aspell-en dconf-gsettings-backend python-pygame cups-bsd cups-client cups-common gnome-accessibility-themes gnome-icon-theme gnome-themes-standard gnome-themes-standard-data libgtk-3* blt dbus-x11 debian-reference-common debian-reference-en desktop-base desktop-file-utils dillo cifs-utils fontconfig fontconfig-config fonts-droid fonts-freefont-ttf galculator gdbserver ghostscript git git-core git-man gpicview libfontconfig1 penguinspuzzle python2.7 python2.7-minimal python3 python3-minimal python3-numpy python3-rpi.gpio python3.2 python3.2-minimal ttf-dejavu ttf-dejavu-core ttf-dejavu-extra vim-common vim-tiny xfonts-* x11-* xserver-* libgtk* heirloom-mailx libraspberrypi-doc lxde-icon-theme ncdu pypy-upstream jackd1 libjack0 python2.6 python2.6-minimal libvorbis0a* libv4l* java-common

apt-get -y autoremove

userdel pi
rm -r /home/pi

echo 'dash dash/sh boolean false' > /tmp/dash.debconf
debconf-set-selections /tmp/dash.debconf
rm /tmp/dash.debconf

grep -v "http://m23debs" /etc/apt/sources.list > /tmp/sources.list

#Add the URL for server update if missing
if [ `grep -c "http://kent.dl.sourceforge.net" /tmp/sources.list` -eq 0 ]
then
	echo "deb http://kent.dl.sourceforge.net/project/m23/m23inst ./" >> /tmp/sources.list
fi

cat /tmp/sources.list > /etc/apt/sources.list

echo "Set Password to 'test'"
passwd root
