#!/bin/sh

if [ $# -lt 1 ]
then
	echo "No package selected"
	exit 1
fi

#make sure the needed globalVars exists
. /mdk/bin/menuFunctions.inc
menuVarInit

cd /mdk/m23Debs/bin
. ./m23Deb.inc

incPatchLevelInVersionPhp

case $1 in
	"m23")
		touch /m23/inc/version.php;
		./mkm23Deb
		updatePackages
		exit;;
	"m23-mdk")
		cd /mdk/m23Debs/bin
		./mkm23-mdkDeb
		updatePackages
		exit;;
	"m23shared")
		cd /mdk/m23Debs/bin
		./mkm23sharedDeb
		#updatePackages #is not in the normal pool
		exit;;
	"m23-ucs-extra")
		finishBuilding m23-ucs-extra
		# Move the package to /mdk/ucs/debs, so it will net get installed by default
		mkdir -p /mdk/ucs/debs
		rm /mdk/ucs/debs/m23-ucs-extra*
		mv /mdk/server/iso/pool/m23-ucs-extra* /mdk/ucs/debs
		cd /mdk/ucs/debs
		makePackages
		#updatePackages
		exit;;
esac

#Make the MD5 sum file invalid
echo "muh" > ../indices/index$1deb.md5
simpleBuild $1 1

#Copy the m23-vbox package to the client package directory
if [ $1 == "m23-vbox" ]
then
	cp /mdk/server/iso/pool/m23-vbox_* /mdk/m23Debs/debs/
fi

updatePackages