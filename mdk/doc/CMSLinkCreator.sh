#!/bin/bash

inputFile="$1"
cmsLinkBase='/matrix23/sync/wwwTests/cms/data/home/Downloads'


if [ $# -ne 1 ]
then
	echo "Creates link files for the CMS for ISO, appliance or RaspBerry Pi image"
	echo "$1 <input file>"
	echo "* input file: ISO, appliance or RaspBerry Pi image of the m23 server"
	exit 1
fi

if [ ! -e "$inputFile" ]
then
	echo "$inputFile doesn't exists!"
	exit 2
fi





# Get the m23 version from the file name
getVersionFromFile()
{
	basename "$1" | sed 's/\([0-9][0-9]\.[0-9]\)/°\1°/' | cut -d'°' -f2
}





# Get some basic information
init()
{
# Detect the m23 version of the input file
	export inputVersion=$(getVersionFromFile "$inputFile")


# Detect the type of the input file
	# RespBerry PI image
	if [ $(echo "$inputFile" | grep -i -c rapi) -gt 0 ]
	then
		export inputType='r'
		export cmsLinkDir="$cmsLinkBase/RaspberryPi"
		export oldCMSLinkFile=$(ls $cmsLinkDir/m23server_??.?_rock_for_Raspberry_Pi.7z.php | tail -1)
	else
		# Serverinstallation ISO
		if [ $(echo "$inputFile" | grep -i -c 'iso$') -gt 0 ]
		then
			export inputType='i'
			export cmsLinkDir="$cmsLinkBase/ISOImages"
			export oldCMSLinkFile=$(ls $cmsLinkDir/m23server_??.?_rock.iso.php | tail -1)
		else
			# Server appliance
			if [ $(echo "$inputFile" | grep -i -c '7z$') -gt 0 ]
			then
				export inputType='v'
				export cmsLinkDir="$cmsLinkBase/vms"
				export oldCMSLinkFile=$(ls $cmsLinkDir/m23-Server-??.?.7z.php | tail -1)
			fi
		fi
	fi

# Get the name of the new link file
	export linkFileVersion=$(getVersionFromFile "$oldCMSLinkFile")
	export newCMSLinkFile=$(echo $oldCMSLinkFile | sed "s/$linkFileVersion/$inputVersion/")

# Get the SHA256 checksum
	export sha256=$(sha256sum "$inputFile" | cut -d' ' -f1)

# Copy the old link file to the new
	cp -n -v "$oldCMSLinkFile" "$newCMSLinkFile"
	
	if [ $? -ne 0 ]
	then
		echo "ERROR: The file $newCMSLinkFile exists. No new release?"
		exit 3
	fi
}





# Replaces the value in a line containg a variable name
replaceLine()
{
	var="$1"
	val="$2"
	
	sed -i "s/^$var => .*/$var => \"$val\",/" "$newCMSLinkFile"
}





init

# Adjust SHA256 and all strings containing the version number
sed -i -e "s/SHA256: [0-9a-f]\{64\}/SHA256: $sha256/" -e "s/$linkFileVersion/$inputVersion/g" "$newCMSLinkFile"

# Adjust the file site
inputFileSize=$(find "$inputFile" -printf "%s")
replaceLine size "$inputFileSize"

# Adjust the file date
inputFileDate=$(find "$inputFile" -printf "%CY-%Cm-%Cd %CH:%CM:00")
replaceLine date "$inputFileDate"

echo ">>>$cmsLinkDir"
echo "Version change: $linkFileVersion => $inputVersion"
echo "Input file size: $inputFileSize"
echo "Input file date: $inputFileDate"


# Move the outdated link file
mv -v -i "$oldCMSLinkFile" "$cmsLinkDir/old"