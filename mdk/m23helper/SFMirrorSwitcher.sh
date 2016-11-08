#!/bin/bash

fileList="/matrix23/arbeit/wwwTests/cms/data/articles/* /mdk/server/extraRFS/bin/m23InstallerBase.inc /m23/inc/sourceslist.php"


# Check, if the command line parameters are correct
if [ $# -ne 2 ] || [ $(echo $1 | grep sourceforge -c) -gt 0 ] || [ $(echo $2 | grep sourceforge -c) -gt 0 ]
then
	echo "Exchanges an old SourceForge mirror with a new one in the m23 CMS, server install ISO and sourceslist.php"
	echo "$0 <old mirror (without \".dl.sourceforge.net\")> <new mirror (without \".dl.sourceforge.net\")>"
	exit 1
fi



# Build the mirror host names
oldMirror="$1.dl.sourceforge.net"
newMirror="$2.dl.sourceforge.net"



# Check, if the new mirror can be pinged
ping -c 1 $newMirror &> /dev/null
if [ $? -ne 0 ]
then
	echo "ERROR: The new mirror ($newMirror) is not pingable!"
	exit 2
fi


for file in $fileList
do
	sed -i "s/$oldMirror/$newMirror/g" $file
done