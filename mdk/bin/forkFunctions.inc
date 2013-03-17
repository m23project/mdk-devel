#!/bin/bash


excludeMDKKernel=0
excludeMDKtmpRoot=0
excludeMDKiso=0

#Read the destination of the /m23 symlink, then generate the real directory names for storing the m23 release and development version
m23ReadLink=$(readlink /m23)
m23ReleaseDir=$(echo $m23ReadLink | sed 's/-devel$//')
m23DevelDir="$m23ReleaseDir-devel"

#Read the destination of the /mdk symlink, then generate the real directory names for storing the MDK release and development version
mdkReadLink=$(readlink /mdk)
mdkReleaseDir=$(echo $mdkReadLink | sed 's/-devel$//')
mdkDevelDir="$mdkReleaseDir-devel"
err=0





#####
# name diffExcludeMeld
# description Checks for differences between a development and a release directory and calls meld with the changed files.
# parameter: Full path to the development directory.
# parameter: Full path to the release directory.
# parameter: Rules for files/directories to exclude.
#####
diffExcludeMeld()
{
	# Check, if meld is installed and a X11 session is running
	type meld &> /dev/null
	if [ $? -ne 0 ] || [ -z $DISPLAY ]
	then
		echo 'Error: For diffing the graphical tool "meld" is used. The tool cannot be found and/or there is nor running X11 session. Make sure, "meld" is installed and you are running the MDK from you graphical desktop.'
		exit 1
	fi

	# Temporary directory storing the directories and links to the differring files
	tmpDir="/tmp/m23diff$$/"	
	mkdir -p "$tmpDir/devel" "$tmpDir/release"

	# Directories to compare
	devDir="$1"
	relDir="$2"
	nonRootUser="$3"
	
	echo "nonRootUser: $nonRootUser"
	read lala

	# Throw away the first three (used) parameters
	shift 3

	# build the grep exclude chain
	exclude='grep -v ~$'
	for ex in $@
	do
		exclude="$exclude | grep -v $ex"
	done

	# Get the files and directories that differ between $devDir and $relDir (run rsync in dry-mode)
	cmd="rsync -nav --delete $devDir/ $relDir/ | sed -e '/^sent [0-9]* /d' -e '/^total size is [0-9]* /d' -e '/^sending incremental file list/d' -e 's#^deleting ##g' | $exclude"

	# Execute and read the files/directories
	echo $cmd | sh | while read diffFile
	do
		# Build a full file/directory path
		diffFilePath="$devDir/$diffFile"

		# Check, if it is no directory and a text file
		if [ -d "$diffFilePath" ] || [ $(file -b "$diffFilePath" | grep ' text' -c ) -eq 0 ]
		then
			continue
		fi

		# Only text files here
		echo "D: $diffFile"

		# Get the name and the directory of the file
		dirName=$(dirname $diffFile)
		fileName=$(basename $diffFile)

		# Generate directories for storing the link of the currect file as devel and release links
		devTempDir="$tmpDir/devel/$dirName"
		relTempDir="$tmpDir/release/$dirName"
		mkdir -p $devTempDir $relTempDir

		# Make links or create empty files
		linkOrTouchINT "$devDir/$diffFile" "$relDir/$diffFile" "$devTempDir/$fileName"
		linkOrTouchINT "$relDir/$diffFile" "$devDir/$diffFile" "$relTempDir/$fileName"
	done

	# Run graphical diff
	su $nonRootUser -c "meld -a \"$tmpDir/devel\" \"$tmpDir/release\""

	# Copy new files back: Temporary directory => fork directory
	copyNewFilesINT "$tmpDir/release" "$relDir"
	copyNewFilesINT "$tmpDir/devel" "$devDir"

	rm -r $tmpDir
}





#####
# name linkOrTouchINT
# description Makes a link, if there is an original file, otherwise create an empty file.
# parameter: Original file name (with full path to link) from the SAME fork.
# parameter: Alternative file name (with full path) from the OTHER fork. Only attributes are copied, not the file contents.
# parameter: Destination file name (with full path). There the link points to or there the empty file will be created.
#####
linkOrTouchINT()
{
	orig="$1"
	alt="$2"
	dest="$3"

	# Check, if the original file exists
	if [ -e "$orig" ]
	then
		# Create a link from the original file to the
		ln -s "$orig" "$dest"
	else
		# Copy file for storing the attributes
		cp -a "$alt" "$dest"
		# Empty the file afterwards
		echo -n > "$dest"
	fi
}





#####
# name copyNewFilesINT
# description Copies new files (that didn't exist in the CURRENT fork, but in the OTHER fork) to the CURRENT fork.
# parameter: Temporary directory with the differencing files and links.
# parameter: Destination directory with the CURRENT fork.
#####
copyNewFilesINT()
{
	diffDir="$1"
	destDir="$2"

	curDir=`pwd`
	cd "$diffDir"

	# Get only the differencing files (not the links)
	find . -type f -printf '%p\n' | while read file
	do
		# Skip the file, if it is empty => Then no contents from the OTHER fork was imported
		if [ $(find $file -printf '%s') -eq 0 ]
		then
			continue
		fi

		# Get the directory of the file
		dirName=$(dirname $file)
		fullDestDir="$destDir/$dirName"

		# Make sure, the destination exists
		mkdir -p "$fullDestDir"

		# Copy the file attributes and make sure that no existing files are overwritten
		cp -a -i -v "$file" "$fullDestDir"
	done

	cd "$curDir"
}





#####
# name getNextPatchlevel
# description Shows the next available patch number.
#####
getNextPatchlevel()
{
	isDevelReturn

	nrRelease=$(grep m23_patchLevel $m23ReleaseDir/inc/version.php | cut -d'"' -f2)
	nrDevel=$(grep m23_patchLevel $m23DevelDir/inc/version.php | cut -d'"' -f2)

	if [ $nrRelease -gt $nrDevel ]
	then
		nr=$nrRelease
	else
		nr=$nrDevel
	fi

	expr $nr \+ 1
}





#####
# name diffExcludeMeld
# description Checks for differences between a development and a release directory and calls meld with the changed files.
# parameter: Full path to the development directory.
# parameter: Full path to the release directory.
# parameter: Rules for files/directories to exclude.
#####
diffExcludeMeldOld()
{
	LC_ALL='C'
	
	# Temporary directory storing the directories and links to the differring files
	tmpDir="/tmp/m23diff$$/"	
	mkdir -p "$tmpDir/devel" "$tmpDir/release"

	# Directories to compare
	devDir="$1"
	relDir="$2"
	IFS=$'\n'

	# Throw away the first two (used) parameters
	shift 2

	# build the grep exclude chain
	exclude='grep -v ~$'
	for ex in $@
	do
		exclude="$exclude | grep -v $ex"
	done

	# Get all non-binary files
	grep -Ilsr -m 1 '.' "$devDir" | while read file
	do
		# Build the command for getting the file name with full path of files that should not be excluded
		cmd="diff -q \"$file\" \"${file/${devDir}/${relDir}}\" 2> /dev/null | grep \" differ$\" | sed -e 's#^Files ##g' -e 's# and .*##g' -e \"s#$devDir##\" | $exclude"

		# Execute the command
		diffFile=$(echo $cmd | sh)

		# If it is not empty, there is a difference between the file form devDir and relDir
		if [ $diffFile ]
		then
			echo "D: $diffFile"

			# Get the name of the directory and the file
			dirName=$(dirname $diffFile)
			fileName=$(basename $diffFile)

			# Generate directories for storing the link of the currect file as devel and release
			devTempDir="$tmpDir/devel/$dirName"
			relTempDir="$tmpDir/release/$dirName"
			mkdir -p $devTempDir $relTempDir

			# Make the links
			ln -s "$devDir/$diffFile" "$devTempDir/$fileName"
			ln -s "$relDir/$diffFile" "$relTempDir/$fileName"
		fi
	done

	# Check for differences
	meld -a "$tmpDir/devel" "$tmpDir/release"

	rm -r $tmpDir
}





#####
# name getm23ReleaseDir
# description Shows the full path to the m23 release directory or exits with an error.
#####
getm23ReleaseDir()
{
	isDevelReturn
	echo $m23ReleaseDir
}





#####
# name getm23DevelDir
# description Shows the full path to the m23 development directory or exits with an error.
#####
getm23DevelDir()
{
	isDevelReturn
	echo $m23DevelDir
}





#####
# name getmdkReleaseDir
# description Shows the full path to the MDK release directory or exits with an error.
#####
getmdkReleaseDir()
{
	isDevelReturn
	echo $mdkReleaseDir
}





#####
# name getmdkDevelDir
# description Shows the full path to the MDK development directory or exits with an error.
#####
getmdkDevelDir()
{
	isDevelReturn
	echo $mdkDevelDir
}





#####
# name forkStatusMessage
# description Generates a status text with the currently active fork (development or release), the release and development directories of m23/MDK.
#####
forkStatusMessage()
{
	isDevelReturn
	if [ $? -eq 0 ]
	then
		mode='development'
	else
		mode='release'
	fi

	echo "Mode:$mode"
	echo "m23-rel:$m23ReleaseDir m23-dev:$m23DevelDir"
	echo "mdk-rel:$mdkReleaseDir mdk-dev:$mdkDevelDir"
}





#####
# name startDaemons
# description Starts the MySQL and Apache 2 daemons.
#####
startDaemons()
{
	/etc/init.d/apache2 start
	/etc/init.d/mysql start
	/etc/init.d/tftpd-hpa start
}





#####
# name stopDaemons
# description Stops the MySQL and Apache 2 daemons.
#####
stopDaemons()
{
	/etc/init.d/apache2 stop
	/etc/init.d/mysql stop
	/etc/init.d/tftpd-hpa stop
}





#####
# name isDevelReturn
# description Checks if /m23 and /mdk are linking to the development version.
# returns: 0, if in development mode, with 1, when in normal mode.
#####
isDevelReturn()
{
	#Test, if the /m23 link points to the development version
	[ $(echo $m23ReadLink | grep '\-devel$' -c) -gt 0 ]; ism23Devel=$?
	#Test, if the /mdk link points to the development version
	[ $(echo $mdkReadLink | grep '\-devel$' -c) -gt 0 ]; ismdkDevel=$?

	#Check, if /m23 and /mdk point to different versions
	if [ $ism23Devel -ne $ismdkDevel ]
	then
		if [ $ism23Devel -eq 0 ]
		then
			errmsg='/m23 is a link to the development version,'
		else
			errmsg='/m23 is a link to the release version,'
		fi

		if [ $ismdkDevel -eq 0 ]
		then
			errmsg='/mdk is a link to the development version.'
		else
			errmsg='/mdk is a link to the release version.'
		fi

		echo "Err: $errmsg. A mismatch should NOT happen here."
		exit 5
	fi

	#Check, if /m23 and /mdk point to the development versions
	if [ $ism23Devel -eq 0 ] && [ $ismdkDevel -eq 0 ]
	then
		return 0
	else
		return 1
	fi
}





#####
# name showExcludeRule
# description Shows a file/directory excluding rule in human readable form.
# parameter: Description of the rule
# parameter: 0, if the rule us active, otherwise inactive.
#####
showExcludeRule()
{
	if [ $2 -eq 0 ]
	then
		yn="yes"
	else
		yn="no"
	fi
	echo "I: Exclude $1: $yn"
}






#####
# name checkLinksNDirs
# description Checks, if /m23 and /mdk are symlinks and the development directories are not existing
#####
checkLinksNDirs()
{
	mode=$1
	err=0

	if [ -z $m23ReadLink ]; then err=1; echo "Err: /m23 is no symlink!"; fi
	if [ -z $mdkReadLink ]; then err=1; echo "Err: /mdk is no symlink!"; fi

#moveDevel2Release
	#Nichtleer $m23ReleaseDir, $mdkReleaseDir, $m23DevelDir, $mdkDevelDir
	if [ $mode = 'moveDevel2Release' ]
	then
		if [ -z $m23ReleaseDir ] || [ ! -d $m23ReleaseDir ]; then err=1; echo "Err: No m23 release version directory found!"; fi
		if [ -z $mdkReleaseDir ] || [ ! -d $mdkReleaseDir ]; then err=1; echo "Err: No MDK release version directory found!"; fi
		if [ -z $m23DevelDir ] || [ ! -d $m23DevelDir ]; then err=1; echo "Err: No m23 development version directory found!"; fi
		if [ -z $mdkDevelDir ] || [ ! -d $mdkDevelDir ]; then err=1; echo "Err: No MDK development version directory found!"; fi
	fi


#switchRelease
	##Symlinks: /m23 & /mdk
	#Nichtleer: $m23ReleaseDir & $mdkReleaseDir
	#Verzeichnisse: $m23ReleaseDir & $mdkReleaseDir
	if [ $mode = 'switchRelease' ]
	then
		if [ -z $m23ReleaseDir ] || [ ! -d $m23ReleaseDir ]; then err=1; echo "Err: No m23 release version directory found!"; fi
		if [ -z $mdkReleaseDir ] || [ ! -d $mdkReleaseDir ]; then err=1; echo "Err: No MDK release version directory found!"; fi
	fi

#switchDevel
	##Symlinks: /m23 & /mdk
	#Nichtleer: $m23DevelDir & $mdkDevelDir
	#Verzeichnisse: $m23DevelDir & $mdkDevelDir
	if [ $mode = 'switchDevel' ]
	then
		if [ -z $m23DevelDir ] || [ ! -d $m23DevelDir ]; then err=1; echo "Err: No m23 development version directory found!"; fi
		if [ -z $mdkDevelDir ] || [ ! -d $mdkDevelDir ]; then err=1; echo "Err: No MDK development version directory found!"; fi
	fi

#makeDevel
	#Symlinks: /m23 & /mdk
	#Leer: $m23DevelDir & $mdkDevelDir
	if [ $mode = 'makeDevel' ]
	then
		if [ -e $m23DevelDir ]; then err=1; echo "Err: $m23DevelDir exists!"; fi
		if [ -e $mdkDevelDir ]; then err=1; echo "Err: $mdkDevelDir exists!"; fi
	fi
	
	if [ $err -gt 0 ]
	then
		echo "Cannot continue with $mode..."
		echo "Press Return for exit"
		read lala
		exit 1
	fi
}






#####
# name infoBeforeStart
# description Shows an info message about what would be done and asks to continue or not
#####
infoBeforeStart()
{
	echo "I: $m23ReadLink will be copied to $m23DevelDir"
	echo "I: $mdkReadLink will be copied to $mdkDevelDir"
	showExcludeRule 'Linux kernel (sources, object files)' $excludeMDKKernel
	showExcludeRule 'extracted Debian packages for the netbootimage' $excludeMDKtmpRoot
	showExcludeRule 'm23 server ISO(s)' $excludeMDKiso

	echo "Continue (y/n)?"
	read yn
	if [ $yn != 'y' ]
	then
		echo "Aborted"
		exit 0
	fi
}





#####
# name copym23
# description Copies the m23 release version to a new development version directory.
#####
copym23()
{
	mkdir -p -m777 $m23DevelDir
	chown www-data.www-data $m23DevelDir

	cd $m23ReadLink
	tar cv "." --exclude=./var/cache/clients/* --exclude=./var/cache/m23apt/* --exclude=./tmp/* | tar xp --same-owner --directory=$m23DevelDir
	sed -i 's/rock/rock-devel/' $m23DevelDir/inc/version.php
}





#####
# name copymdk
# description Copies the mdk release version to a new development version directory.
#####
copymdk()
{
	mkdir -p -m777 $mdkDevelDir
	chown www-data.www-data $mdkDevelDir
	
	exclude=''

	if [ $excludeMDKKernel -eq 0 ]
	then
		exclude="$exclude --exclude=./client+server/kernel/*"
	fi
	

	if [ $excludeMDKtmpRoot -eq 0 ]
	then
		exclude="$exclude --exclude=./client+server/tmpRoot/*"
	fi

	if [ $excludeMDKiso -eq 0 ]
	then
		exclude="$exclude --exclude=./server/*.iso*"
	fi

	cd $mdkReadLink
	tar cv "." $exclude | tar xp --same-owner --directory=$mdkDevelDir
}





#####
# name switchRelease
# description Switches to the (hopefully existing) release version and restarts the daemons.
#####
switchRelease()
{
	checkLinksNDirs switchRelease

	stopDaemons
	#Remove the symlinks
	rm /m23 /mdk
	#Create new symlinks pointing to the release version
	ln -s $m23ReleaseDir /m23
	ln -s $mdkReleaseDir /mdk
	startDaemons
}





#####
# name switchDevel
# description Switches to the (hopefully existing) development version and restarts the daemons.
#####
switchDevel()
{
	checkLinksNDirs switchDevel

	stopDaemons
	#Remove the symlinks
	rm /m23 /mdk
	#Create new symlinks pointing to the development version
	ln -s $m23DevelDir /m23
	ln -s $mdkDevelDir /mdk
	startDaemons
}





#####
# name moveDevel2Release
# description Makes the development version the current release and moves the (now old) release to outdated.
#####
moveDevel2Release()
{
	checkLinksNDirs moveDevel2Release

	#Generate postfix for the outdated directories
	outdated=$(date +"outdated_by_%F_%H-%M")

	#Rename the release directories of m23/MDK to outdated directories
	mv $m23ReleaseDir "$m23ReleaseDir$outdated"
	mv $mdkReleaseDir "$mdkReleaseDir$outdated"

	#Rename the development directories of m23/MDK to release directories
	mv $m23DevelDir $m23ReleaseDir
	mv $mdkDevelDir $mdkReleaseDir

	#Remove the -devel tag from the codename
	sed -i 's/rock-devel/rock/' $m23ReleaseDir/inc/version.php

	#Make the release version active
	switchRelease
}





#####
# name makeDevel
# description Creates a development copy of the release directory.
#####
makeDevel()
{
	checkLinksNDirs makeDevel
	infoBeforeStart
	copym23
	copymdk
}





#####
# name makeSymlinkm23mdk
# description Moves the directories /mdk and /m23 to /mdk-dir and /m23-dir. Then creates symlinks form /m23-dir to /m23 and /mdk-dir to /mdk.
#####
makeSymlinkm23mdk()
{
	if [ -L /m23 ]; then echo "Err: /m23 is already a symlink!"; exit 6; fi
	if [ -L /mdk ]; then echo "Err: /mdk is already a symlink!"; exit 6; fi

	mv /m23 /m23-dir
	ln -s /m23-dir /m23
	mv /mdk /mdk-dir
	ln -s /mdk-dir /mdk
}





diffMerge()
{
	meld $m23DevelDir $m23ReleaseDir
	meld $mdkDevelDir $mdkReleaseDir
}