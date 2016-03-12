. /mdk/bin/forkFunctions.inc

debPath="/mdk/server/iso/pool/"




#Determines the maximum upload speed
rsyncUploadSpeedLimit()
{
	type rsUS 2> /dev/null > /dev/null
	if [ $? -eq 0 ]
	then
		rsUS
	else
		echo -n '--bwlimit=0'
	fi
}





#Generates a list of all base system packages for all (Debian/Ubuntu) releases
mkAllBaseSysFileLists()
{
cd /m23/inc/distr/baseSysFileLists

for rel in $(dpkg -L debootstrap | grep scripts | grep -v buildd | grep -v fakechroot | xargs -n1 basename | grep -v scripts  | grep -v breezy  | grep -v dapper  | grep -v edgy  | grep -v etch  | grep -v etch-m68k  | grep -v feisty  | grep -v gutsy  | grep -v hoary  | grep -v intrepid  | grep -v jaunty  | grep -v karmic  | grep -v lenny  | grep -v maverick  | grep -v oldstable  | grep -v potato  | grep -v sarge  | grep -v warty  | grep -v woody | sort)
do
	if [ ! -f $rel ]
	then
		debootstrap --print-debs $rel /tmp/debootstrap.tmp > $rel
		if [ $(find $rel -printf "%s") -eq 0 ]
		then
			rm $rel
		fi
	fi
done
}





#Writes the newest patch info from the changelog to the patch.info file.
writeNewestChangelogToPatchInfo()
{
awk '
BEGIN {
	cnt=0
}

/^[0-9+]/ {
	next
}

/^patch/ {
	cnt++;
}

{
	if (cnt == 2)
		exit(0);
	print($0);
}
' /m23/doc/changelog
}





#adds all m23 PHP commands to the PHP.js of codepress to highlight m23 commands like normal PHP commands.
addm23CommandsToCodepress()
{
fnks=`grep ^function /m23/inc/* -r -h | grep _ | cut -d' ' -f2 | cut -d'(' -f1 | sort -u | awk -v ORS='|' '{print}' | sed 's/|$//'`

sed "s/###M23###/$fnks/" /m23/data+scripts/m23admin/packages/codepress/languages/php.js.template > /m23/data+scripts/m23admin/packages/codepress/languages/php.js
}





#adds all m23 PHP commands to the mode-php.js of ace to highlight m23 commands like normal PHP commands.
addm23CommandsToACE()
{
fnks=`grep ^function /m23/inc/* -r -h | grep _ | cut -d' ' -f2 | cut -d'(' -f1 | sort -u | awk -v ORS='|' '{print}' | sed 's/|$//'`

sed "s/###M23###/$fnks/" /m23/data+scripts/ace/src-min-noconflict/mode-php.js.pre-insert-m23-functions > /m23/data+scripts/ace/src-min-noconflict/mode-php.js
}





#Get all m23 files and directories are owned by the user dodger and stores the result in the file /tmp/dodgerFiles.lst
dodgerFilesGetFileList()
{
	echo -n "Getting all m23 files and directories are owned by the user dodger..."
	find /m23/ -printf "%u?%h/%f\n" | grep dodger | cut -d'?' -f2 > /tmp/dodgerFiles.lst
	echo " done"
}





#Change user of all files and adirectories stored in /tmp/dodgerFiles.lst.
#parameter $1: Destination username.
dodgerFilesChangeUser()
{
echo -n "Changing user of all m23 files and directories originally owned by the user dodger to user \"$1\"..."
awk -vuser=$1 -vORS='' 'BEGIN {
	cnt=0
	str="chown "user
}

{
	str=str" \""$0"\"";
	cnt=cnt + 1;

	if (cnt == 10)
	{
		system(str);
		cnt=0
		str="chown "user
	}
}

END {
	system(str);
}
' /tmp/dodgerFiles.lst
	echo " done"
}


#get the version information string from the currently installed m23 software
getm23VersionString()
{
	ver=`grep version /m23/inc/version.php | cut -d'"' -f2`
	patch=`grep patchLevel /m23/inc/version.php | cut -d'"' -f2`
	
	echo "$ver-$patch"
}



#adjust the version and installed-size in the control and prints out the full name of the Debian package
adjustControl()
{
	#$1: path and file name of the control file
	#$2: file list

	ver=`getm23VersionString`
	arch=`grep "^Architecture:" "$1" | cut -d' ' -f2`
	pkg=`grep "^Package:" "$1" | cut -d' ' -f2`

	#get the accumulated size of all files listed in the file list
	du -c -s `cat "$2"` | tail -1 | sed 's/\t/#/' | cut -d '#' -f1 > /tmp/Installed-Size.m23 2> /dev/null
	size=`cat /tmp/Installed-Size.m23`
	rm /tmp/Installed-Size.m23

	sed "s/`grep 'Version: ' \"$1\" `/Version: $ver/" "$1" | sed "s/`grep 'Installed-Size:' \"$1\" `/Installed-Size: $size/" > /tmp/m23Deb.Name
	
	cat /tmp/m23Deb.Name > "$1"
	rm /tmp/m23Deb.Name 2> /dev/null

	echo $pkg"_"$ver"_"$arch".deb"
}

#check if building is needed and create index
prepareBuilding()
{
	#$1: package name to build

	echo
	echo ">>> $1"

	cd /mdk/m23Debs/indices

	#check if there are changes
	/mdk/m23update/bin/md5Check h $1"deb"
	if test $? -ne 0
	then
		echo "package up to date"
		#no changes => exit
		return 1
	fi

	#prepare a list of all files that should get copied to the package
	/mdk/m23update/bin/md5Check f $1"deb" /tmp/$1"deb".list
	/mdk/m23update/bin/md5Check m $1"deb"
	return 0
}


finishBuilding()
{
	#$1: package name to build

	cd /mdk/m23Debs/$1
	debname=`adjustControl DEBIAN/control`

	dpkg-deb --build . $debPath$debname

	if [ $? -ne 0 ]
	then
		echo "Package build ERROR"
		read lala
	fi

	rm -f "/tmp/$1""deb.list" 2> /dev/null
	rm -f -r m23 mdk 2> /dev/null
}


simpleBuild()
{
	#$1: package name to build
	#$2: for forced building set it to 1

#check if building is needed and create index
	prepareBuilding "$1"
	if [ $? -eq 1 ]  || [ $2 -ne 1 ]
	then
		return 1
	fi

	cd /mdk/m23Debs/"$1"
	rm -r m23 mdk 2> /dev/null

	#Check if the file with the file list exists
	if [ -e /tmp/"$1"deb.list ]
	then
		tar cv --same-owner -T /tmp/"$1"deb.list | tar x
	fi
	#clean backup files
	find | grep ~$ | xargs rm 2> /dev/null

#build package
	finishBuilding "$1"
}

updatePackages()
{
	cd $debPath
	makePackages
	chown www-data.www-data $debPath -R
}





#checks for changes and new files in the package directory and writes the filenames to /tmp/m23instUpload.list
newPackages()
{
	cd /mdk/m23Debs/indices
	#make sure the right package path is included
	echo $debPath > includem23instPackages
	#check if there have been made changes to the packages
	/mdk/m23update/bin/md5Check c m23instPackages
	#all new and all new files have to get uploaded
	cat indexm23instPackages.new indexm23instPackages.changed > /tmp/m23instUpload.list
	#make the new the current index
	/mdk/m23update/bin/md5Check m m23instPackages
}





#increments the patch level in the version.php
incPatchLevelInVersionPhp()
{
	patchNrLine=`grep m23_patchLevel /m23/inc/version.php`
	patchNr=`echo $patchNrLine | cut -d'"' -f2`
	nr=`getNextPatchlevel`

	#replace the old line with the new one (contains the new patch number)
	cat /m23/inc/version.php | sed "s/$patchNrLine/\$m23_patchLevel=\"$nr\";/" > /tmp/version.php
	cat /tmp/version.php > /m23/inc/version.php
	rm /tmp/version.php

	#Check if the first patch line has no patch numer (only if it has not, a number can be added)
	if [ `grep ^patch /m23/doc/changelog | head -1 | grep "patch [0-9][0-9][0-9][0-9][0-9]" -c` -eq 0 ]
	then
		#Add the number of the patch to the changelog
		sed -i "0,/^\(patch:\)\(.*\)/ s//patch $nr:\2/" /m23/doc/changelog
	fi
};





#deletes packages with the same name and older version numbers. Only the newest package remains
deleteOldDoubles()
{
	#cd $debPath
	ls *.deb | awk -vFS='_' '

function fillnr(innr)
{
	if (innr > 100 && innr < 1000)
		outnr=innr;
	else if (innr > 9 && innr < 100)
		outnr="0"innr;
	else if (innr < 10 && innr > 0)
		outnr="00"innr;
	else
		outnr="000";

	return(outnr);
}

{

split($2,nrs,"[.]+");
last="x";

for (i=1; i <= 3; i++)
{
	if (split(nrs[i],tmp,"-") > 1)
		last=tmp[2];

	out[i]=fillnr(tmp[1]);
}

if (last == "x")
	out[4]="xxx";
else
	out[4]=fillnr(last);

print($1"*"$3"*"out[1]out[2]out[3]out[4]"*"$0);

'} | sort -r -n | awk -vFS='*' '
BEGIN {
last=""
}

{
        if (last != $1""$2)
                last=$1""$2
        else
                system("rm "$4);
}'
}





#creates the Debian package index (Packages, Packages.gz, Packages.bz2)
makePackages()
{
	deleteOldDoubles
	rm *.gif *.htm* Packages.* 2> /dev/null
	touch tempmkpackages

	dpkg-scanpackages -m . tempmkpackages | grep -v "Depends: $" > Packages
	gzip -c Packages > Packages.gz
	bzip2 -k Packages

	rm tempmkpackages
	mkRelease
}





#uploads new and changed clients packages to SF
uploadClientPackagesToSF()
{
	cd /mdk/m23Debs/debs/
	makePackages
	mkRelease
	#Upload to webspace
	rsync `rsyncUploadSpeedLimit` -raPy -e ssh --delete-after --delete /mdk/m23Debs/debs/ hhabermann,m23@web.sourceforge.net:/home/project-web/m23/htdocs/m23debs/
	#Upload to FRS
	rsync `rsyncUploadSpeedLimit` -avPyrL -e ssh --delete-after --delete /mdk/m23Debs/debs/ "hhabermann,m23@frs.sourceforge.net:/home/frs/project/m/m2/m23/m23debs"
	#Upload to backups
	rsync `rsyncUploadSpeedLimit` -avPyrL -e ssh /mdk/m23Debs/debs/*.deb "hhabermann,m23@frs.sourceforge.net:/home/frs/project/m/m2/m23/m23clientpackages"
	
	rsync `rsyncUploadSpeedLimit` -rzaPy --delete-after --delete * root@tux05:/var/www/m23debs/
}





#converts a tar with gzip or bzip2 compression to a Debian package
#$1: name of the tar file
convertTarToDeb()
{
	archiveName=$1
	curDir=`pwd`
	
	if [ `echo $archiveName | grep -i -c "\(tar.gz\|tgz\)"` -gt 0 ]
	then
		packageName=`echo $archiveName | sed 's/\(.tar.gz\|.tgz\)//I'`
		packageName=`basename $packageName`
		extractTarCmd="z"
		tarExtension="tar.gz"
	else
		if [ `echo $archiveName | grep -i -c "\(tar.bz2\|tb2\)"` -gt 0 ]
		then
			packageName=`echo $archiveName | sed 's/\(.tar.bz2\|.tb2\)//I'`
			packageName=`basename $packageName`
			extractTarCmd="j"
			tarExtension="tar.bz2"
		else
			echo "The format of the input archive is neither tar.gz nor tar.bz2."
			exit 1
		fi
	fi
	
	controlTempDir=`mktemp -dt`
	dataTempDir=`mktemp -dt`
	
	echo -n "* Extract the archive to get the extracted size..."
	tar x${extractTarCmd}f $archiveName -C $dataTempDir
	extractedSize=`du -s $dataTempDir | cut -f1`
	echo " done"
	echo "i Size is $extractedSize KB"

cat > $controlTempDir/control <<EOF
Package: $packageName
Section: unknown
Priority: optional
Architecture: all
Version: 1.0
Installed-Size: $extractedSize
Maintainer: none
Description: Deb package created by m23 package generator from $archiveName.
EOF

	cd $dataTempDir
	find -type f | xargs md5sum > $controlTempDir/md5sums
	
	cd $controlTempDir
	tar czf control.tar.gz *
	
	cd $curDir
	cp $archiveName $controlTempDir/data.$tarExtension
	echo 2.0 > $controlTempDir/debian-binary
	
	echo -n "* Creating Debian package..."
	ar -cr ${packageName}_1.0_all.deb $controlTempDir/{debian-binary,control.tar.gz} $controlTempDir/data.$tarExtension
	echo " done"
	
	echo -n "* Cleaning temporary directories..."
	rm -Rf $controlTempDir $dataTempDir
	echo " done"
}





#Makes the Release file and signs it
mkRelease()
{
	apt-ftparchive release -o APT::FTPArchive::Release::Origin=m23 -o APT::FTPArchive::Release::Suite=stable .  > Release
	
	id dodger 2> /dev/null > /dev/null
	if [ $? -eq 0 ]
	then
		cat Release | sudo -u dodger gpg --default-key 0x3305E63A --no-options --detach-sign --armor --textmode > Release.gpg
		if [ $(find Release.gpg -printf "%s") -eq 0 ]
		then
			rm Release.gpg
		fi

		cat Release | sudo -u dodger gpg --default-key 0x3305E63A -a -s --clearsign > InRelease
		if [ $(find InRelease -printf "%s") -eq 0 ]
		then
			rm InRelease
		fi
	fi
}