#!/bin/sh

if test $# -ne 3
then
	echo "usage $0 <deb file> <new architecture> <destination directory>"
	echo "	<deb file>: Name of the Debian package (with full path)"
	echo "	<new architecture>: The architecture the new package should have (e.g. amd64)"
	echo "	<destination directory>: Where to store the changed package"
	exit
fi

#store the command line parameters in named variables
package=$1
packageArch=$2
destDir=$3

#make and go to the destination dir
mkdir -p $destDir
cd $destDir

#create the temporary directory for the extracted package
mkdir repackTmp
cd repackTmp

#extract the control and data part of the package
dpkg-deb -e $package DEBIAN
dpkg-deb -x $package .

#get version and package name from control
ver=`grep Version DEBIAN/control | cut -d' ' -f2`
packageName=`grep Package DEBIAN/control | cut -d' ' -f2`

#change the architecture in control
cat DEBIAN/control | sed "s/Architecture: [0-9a-z]*/Architecture: $packageArch/" > /tmp/control
cat /tmp/control > DEBIAN/control
rm /tmp/control

#build the new package
dpkg-deb -b . "../"$packageName"_"$ver"_$packageArch.deb"

#remove the temporary directory
cd ..
rm -r repackTmp
