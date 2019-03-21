# m23 extra: checkdisklabel cmount cumount initCrypt mkcimage mkcfs percent wait4go

# leer: makedevs 

# raus: fsize add gethome getkpkg getpkg getmem getserver loadmodules killmod make-ssh-keys mkrescue chkfixed net-config net-start net-stop nohup pkgadd quit savecfg save-config savehome rmpkg sfdisk snarf statusbar sub

#BB noch dazu: read shift



#get the next free loop device and assigns a file or device to it
#parameter file or device to assign to the found loop device
getNextLoop()
{
	extra=$1

	if test ! -e "$extra"
	then
		echo "The device or file named \"$extra\" doesn't exist"
		exit 42
	fi

	lodev=`losetup -f`
	if [ $? -eq 0 ]
	then
		losetup "$lodev" "$extra"
		echo "$lodev"
	else
		echo "Can't bind the device/file to a loop device. Maybe you are running out of loop files and increasing the amount of loop files may help:"
		echo "	modprobe loop max_loop=255"
		echo "	for nr in \`seq 0 255\`"
		echo "	do"
		echo "		mknod /dev/loop\$nr b 7 $lnr"
		echo "	done"
		echo "The last checked loop file was: /dev/loop$lnr"
		exit 23
	fi
}





# Disconnect loop devices that match the search string.
#parameter Search string to find all loops that are connected to a matching file name.
disconnectLoop()
{
	loopSearch="$1"
	# Check first if anything is reported by 'losetup -a'
	losetup -a | grep "$loopSearch" > /dev/null 2>&1
	# Action only if something is setup
	if [ $? -eq 0 ]; then
	      echo ">>>> disconnectLoop: $loopSearch"
	losetup -a
	losetup -a | grep "$loopSearch" | cut -d':' -f1 | xargs -n1 losetup -d
	else
	      echo ">>>> disconnectLoop: NOP"
	fi
}





#copies a single library (with full name e.g. libnss_dns.so.2) to the RFS
createRFScopyLib()
{
	. /tmp/menuVars
	lib=$1
	neededBy=$2

	if [ -z $neededBy ]
	then
		neededBy="Added_manually"
	fi

	#get the filename of the library without the path
	fnamelib=`echo $lib | awk '{anz=split($0,arr,"/"); print(arr[anz]);}'`
	fnamelib=`basename "$fnamelib"`
	
	# Don't copy a lib again, that was copied before
	if [ -f "lib/$fnamelib" ]
	then
		return 0
	else
		echo $fnamelib >> /tmp/mdk-createRFSAllLibs.txt
	fi
	
# 	echo "öööfnamelib: $fnamelib"

	#where to find the library in the extracted debs (with full path)
	#HINT: "sort | tail -1" ensures that the tls version of the libs is used
	libsrc=`find $extractedDebsDir -name $fnamelib -xtype f | grep lib | sort | tail -1`
#	echo "ööölibsrc: $libsrc"

	if [ -z $libsrc ]
	then
		echo -n $lib  | grep -v 'required by' | egrep -v ^not$ | grep -v linux-gate | grep -v linux-vdso | grep -v '\t' > /dev/null 2>&1
		if [ $? -eq 1 ]
		then
			# ignore this lib
			#continue
			return
		elif [ $neededBy == "Added_manually" ]
		then
			echo "Error: library $lib added manually but is not found!"
			echo "lib: $lib fnamelib: $fnamelib extractedDebsDir: $extractedDebsDir"
			read errkey
			#continue
			return
		else
			echo "Error: Library $lib needed by \"$neededBy\" not found!"
			echo "lib: $lib fnamelib: $fnamelib libsrc: $libsrc extractedDebsDir: $extractedDebsDir"
			read errkey
			#continue
			return
		fi
	fi

	#link and filename separated with ?
	linkAndFile=`find $libsrc -printf "%l?%f"`
#	echo "ööölinkAndFile: $linkAndFile"
	
	#check if the library is a link to a real library
	if test `echo $linkAndFile | egrep -c '^\?'` -gt 0
	then
		#it's a real file
		cp -u -v $libsrc lib
	else
		#it's a link
		#get the link destination (the real library file)
		realFile=`echo $linkAndFile | cut -d'?' -f1`

		realFile=$(basename $realFile)
		
#		echo "ööörealFile: $realFile"

		#copy the real library to the bootimage lib dir
		cp -v -u `find $extractedDebsDir -name $realFile -xtype f | grep lib | sort | tail -1` lib

		#create a symlink from the requested library name to the real library (like it was in the extracted debs dir)
		cd lib
			ln -s $realFile $fnamelib 2> /dev/null
		cd ..
	fi
}





#copies all listed executables with their libraries (and in addition their links) to the root filesystem
createRFSBins()
{
	. /tmp/menuVars
	
	rm /tmp/mdk-createRFSAllLibs.txt 2> /dev/null

	echo "Copying binaries, needed libraries and symbolic links... "

	extractedDebsDir=`getRootFSDownloadDirWithArch`
	
	if [ -d $extractedDebsDir ] && [ -f $extractedDebsDir/bin/bash ]
	then
		true
	else
		echo "ERROR: The directory $extractedDebsDir that should contain the extracted Debian packages is missing or empty!!!"
		echo "Please select clientISO -> RFSdownload before building."
		echo "Press enter to exit"
		read lala
		exit 1
	fi
	
	
	if [ ! -f "$(getKernelStoreDirWithArch)/bzImage" ]
	then
		echo "ERROR: No kernel ($(getKernelStoreDirWithArch)/bzImage) found!!!"
		echo "Please select clientISO -> kernel -> Compile before building."
		echo "Press enter to exit"
		read lala
		exit 2
	fi
	

	#remove all symlinks, libs and bin directories
	rm $rootFSDir/lib/* $rootFSDir/lib64 $rootFSDir/sbin 2> /dev/null
	rm -r $rootFSDir/bin $rootFSDir/usr 2> /dev/null

	#create real directories
	mkdir -p $rootFSDir/bin $rootFSDir/lib $rootFSDir/usr
	cd $rootFSDir
	#make lib and lib64 the same
	ln -s lib lib64
	#sbin and bin should be the same too
	ln -s bin sbin
	
	cd lib
		ln -s . tls
	cd ..

	#create links to usr
	cd usr
		ln -s ../lib
		ln -s ../lib64
		ln -s ../bin
		ln -s ../sbin
	cd ..



	#not needed?: grub grub-install grub-md5-crypt

	#halt: Now in BusyBox
	for bin in wget ar bzip2 cryptsetup dhclient dialog dmidecode dmraid dropbear dropbearkey e2fsck find hdparm install-mbr losetup lspci mdadm mke2fs mkfs mkfs.minix mkfs.msdos mkfs.vfat mkreiserfs mkswap mtools nano parted partimage partprobe pivot_root scanlogd setterm testdisk ddcxinfo-knoppix hwsetup cfdisk tar  MAKEDEV getty ping md5sum discover discover-modprobe discover-pkginstall discover-config badblocks sfdisk tset hwinfo rsync mkntfs ntfscat ntfsclone ntfscluster ntfscmp ntfscp ntfsfix ntfsinfo ntfslabel ntfsls ntfsresize ntfsundelete ntfs-3g ntfs-3g.probe mount bash mkfifo gptsync mountpoint gawk modinfo blockdev scp partclone.btrfs partclone.extfs partclone.fat partclone.dd partclone.ntfs partclone.ntfsfixboot partclone.reiser4 partclone.restore resize_reiserfs resize2fs xfs_growfs kmod sgdisk haveged #lvchange lvconvert lvcreate lvdisplay lvextend lvm lvmchange lvmdiskscan lvmsadc lvmsar lvreduce lvremove lvrename lvresize lvs lvscan pvchange pvcreate pvdisplay pvmove pvremove pvresize pvs pvscan vgcfgbackup vgcfgrestore vgchange vgck vgconvert vgcreate vgdisplay vgexport vgextend vgimport vgmerge vgmknodes vgreduce vgremove vgrename vgs vgscan vgsplit #lvmiopversion #mount.ntfs-3g ms-sys ntfsmount
	do
		foundBins=$[ $(find $extractedDebsDir -name $bin -xtype f | grep bin -c) + $(find $extractedDebsDir -name $bin -type l | grep bin -c) ]

		if test $foundBins -eq 0
		then
			echo "find $extractedDebsDir -name $bin -xtype f | grep bin -c"
			echo $foundBins
			echo "ERROR: $bin was not found!"
			echo "Press enter to continue"
			read lala
			continue
		fi

		#copy the executable from the extracted debs to the bootimage bin directory
		#cp `find $extractedDebsDir -name $bin -xtype f | grep bin | head -1` bin

		#try if the binary can be found as a normal file
		binSource=`find $extractedDebsDir -name $bin -type f | grep bin | grep -v klibc | head -1`

		#if not try to find a link
		if [ -z $binSource ]
		then
			link=$(find $extractedDebsDir -name $bin -type l | grep bin | grep -v klibc | head -1)

			basenameLink=$( basename "$link" )

			#Make a link to the binary
			binSource=$( readlink "$link" )
			ln -s "$binSource" "bin/$basenameLink"
			echo "+lnk $basenameLink"

			# Make sure, the found file is from the temp directory and not the local root filesystem
			# e.g. /bin/modinfo => /bin/kmod => /mdk/client+server/tmpRoot/i386/bin/kmod
			#binSource=$( find $extractedDebsDir | grep "/$binSource$" | grep -v klibc | head -1 )
			# FABR: for the case $binSource == /bin/kmod grep is searching for //bin/kmod
			#       which will never succeed. IMHO the '/' must be removed in the pattern.
			binSource=$( find $extractedDebsDir | grep "$binSource$" | grep -v klibc | head -1 )
			
			# Check
			if [ -z $binSource ]
			then
				echo "FIXTHIS: binSource empty for link=$link"
			fi
		fi
		
		echo "+bin $binSource"

		#copy the found file or link
		# Check
		if [ -n $binSource ]
		then
			cp -f $binSource bin
		fi
		
		echo $bin > /tmp/mdk-createRFSBins.txt

		# Skip script and statically linked files
		if [ $(file bin/$bin | grep -i 'script' -c) -gt 0 ] || [ $(file bin/$bin | grep -i 'statically' -c) -gt 0 ]
		then
			continue
		fi

		#use different methods to detect the libraries needed by the executables
		if test `getBuildArch` == "x86"
		then
			# Use the bin source of symbolic links to get the needed libraries
			if [ $(file bin/$bin | grep -i 'symbolic link' -c) -gt 0 ]
			then
				bin=$( basename "$binSource")
			fi

			#simple ldd for same architecture as the host
			for lib in `ldd bin/$bin 2> /dev/null | grep -v 'required by' | cut -d " " -f1 | tr -d [:blank:] | grep -v '(0x'`
			do
				echo "LIB: $lib"
				createRFScopyLib $lib $bin
			done
		else
			# FABR: problem with objdump
			# /usr/bin/x86_64-linux-gnu-objdump on 32bit debian machines does not seem to regognize ELF 64"
			# However, the default binary /usr/bin/objdump does.
			# Thus, we prefer /usr/bin/objdump on 32bit debian 9 machines.
			our_arch=`arch`
			if [ $our_arch == "i686" -a -f /usr/bin/i686-linux-gnu-objdump ]
				then
				objdump="/usr/bin/i686-linux-gnu-objdump"
			elif [ -f /usr/bin/x86_64-linux-gnu-objdump ]
			then
				objdump="/usr/bin/x86_64-linux-gnu-objdump"
			else
				objdump="/usr/x86_64-unknown-linux-gnu/bin/x86_64-unknown-linux-gnu-objdump"
				fi

			bin=`cat /tmp/mdk-createRFSBins.txt`

			# Get the needed libs by objdump and ldd
			$objdump -x bin/$bin | grep NEEDED | tr -s [:blank:] | cut -d' ' -f3 | grep -v '(0x' > /tmp/mdk-createRFSLibs.txt
			# FABR: problem with ldd
			# On a 32bit machine the command
			#   ldd /mdk/client+server/tmpRoot/amd64/usr/bin/x86_64-linux-gnu-ar
			# yields
			#   'not a dynamic executable' and exit code 1
			# We need to cope with this, below

			ldd bin/$bin &> /dev/null
			if [ $? -eq 0 ]  # ldd found something
			then
				ldd bin/$bin 2> /dev/null | grep -v 'required by' | cut -d " " -f1 | tr -d [:blank:] | grep -v '(0x' >> /tmp/mdk-createRFSLibs.txt
			fi

			#special method for amd64
			for lib in `sort -u /tmp/mdk-createRFSLibs.txt`
			do
				echo ">>>createRFScopyLib $lib $bin"
				createRFScopyLib $lib $bin
			done
		fi
#		exit 0 #ööö
	done

	#Special handling for 7-Zip
	sevenZipSource=`find $extractedDebsDir -name 7zr -type f | grep lib | head -1`
	cp $sevenZipSource bin

	#Special handling for awk
	mv bin/gawk bin/awk

	#copy extra lib that is not linked against ping and wget, but that is needed for resolving hostnames
	createRFScopyLib libnss_dns.so.2
	#This is needed for authentification in dropbear.
	createRFScopyLib libnss_compat.so.2

	if [ `getBuildArch` == "x86_64" ]
	then
		createRFScopyLib ld-linux-x86-64.so.2
	fi

	#Needed for lspci (but seems not to be detected on amd64)
	createRFScopyLib libresolv.so.2

	#Needed for sgdisk (but seems not to be detected on amd64)
	createRFScopyLib libicudata.so.48

	createRFScopyLib libdbus-1.so.3
	createRFScopyLib libhal.so.1
	createRFScopyLib libudev.so.0
	createRFScopyLib libtinfo.so.5
	createRFScopyLib libtasn1.so.3
	createRFScopyLib libncurses.so.5
	createRFScopyLib libp11-kit.so.0
	createRFScopyLib libnsl.so.1				# for dropbear
	createRFScopyLib libdevmapper.so.1.02.1 	# for parted
	createRFScopyLib libudev.so.1				# for parted
	createRFScopyLib libx86emu.so.1				# for hwinfo
# 	createRFScopyLib libpcre.so.3
# 	createRFScopyLib libisccfg-export.so.90
#	createRFScopyLib libicuuc.so.52				# wget i386; removed by FABR
	createRFScopyLib libtasn1.so.6				# wget amd64 Debian 9
	createRFScopyLib libhogweed.so.4			# wget amd64 Debian 9
	createRFScopyLib libidn2.so.0				# wget amd64 Debian 9
	createRFScopyLib libunistring.so.0			# wget amd64 Debian 9
	createRFScopyLib libffi.so.6				# wget amd64 Debian 9

	# Store all libraries used for the bootimage, if it's the same as the CPU architecture of the m23 server
	if [ `getBuildArch` == `uname -m` ]
	then
		sort -u /tmp/mdk-createRFSAllLibs.txt | grep -v ':' > /mdk/client+server/allLibs.txt
	else
		if [ -f /mdk/client+server/allLibs.txt ]
		then
			for lib in `cat /mdk/client+server/allLibs.txt`
			do
				createRFScopyLib $lib
			done
		fi
	fi
	
	
	echo "done"
}





#creates generic device nodes in the root filesystem
createRFSDevs()
{
	devsFile="/m23/data+scripts/packages/baseSys/makedev.tar.gz"

	. /tmp/menuVars

	cd $rootFSDir
	rm -r dev 2> /dev/null
	mkdir dev
	
	if [ $? -ne 0 ]
	then
		echo "ERROR: Directory for the device nodes could not get created!"
		read maha
	fi
	
	cd dev
	echo -n "Creating devices... "

	if test -f $devsFile
	then
		tar xfz $devsFile
	else
		if test `whoami` = "root"
		then
			../bin/MAKEDEV generic
			#needed for loadkeymap
			mkdir vc
			cd vc
			ln -s ../tty0 0
			cd ..
		else
			echo "Here we need root access"
			su -c "../bin/MAKEDEV generic"
		fi

		tar cfz $devsFile .
	fi

	echo "done"
}





#copies extra files and directories into the root filesystem
createRFSCopyExtraFilesAndDirs()
{
	echo -n "Copying extra files and directories..."
	. /tmp/menuVars

	cd $rootFSDir

	tar --same-owner -C $rootFSExtraDir -c . | tar x
	tar --same-owner -C /mdk/client+server/extraRFS -c . | tar x
	tar --same-owner -C `getRootFSDownloadDirWithArch`/completeExtracted -c . | tar x
	cp -u -a `getCompiledDirWithArch`/* .
#	cp -n -a `getCompiledDirWithArch`/* .

	#Copy the current version of m23hwscanner
	cp -v /mdk/m23Debs/m23-initscripts/debian/etc/init.d/m23hwdetect /mdk/m23Debs/m23-hwscanner/debian/bin/m23hwscanner bin
	mkdir -p etc/init.d
	cp -v /mdk/m23Debs/m23-initscripts/debian/etc/init.d/m23hwdetect etc/init.d

	kernelName=$(ls lib/modules/)
	depmod -a -b . $kernelName

	echo " done"
}





#Creates an empty image file for the root filesystem
createRFSImage()
{
	. /tmp/menuVars

	#create the directory where to store the initrd image
	rm -r $initrdBuildDir 2> /dev/null
	mkdir -p $initrdBuildDir
	cd $initrdBuildDir

	#create the directory where to mount initrd
	rm -r $rootFSDir 2> /dev/null
	mkdir $rootFSDir

	#Make sure there is nothing mounted
	umount $rootFSDir 2> /dev/null

	#size of the kernel ramdisk
	ramdiskSize=`getRamDiskSize`

	echo "Kernel ramdisk size: $ramdiskSize"

	#write the kernel ramdisk size to kernelRamDisk.inc
	if test `whoami` = "root"
	then
		echo "<?PHP" > /m23/inc/kernelRamDisk.inc
		echo "\$kernelRamDisk=$ramdiskSize;" >> /m23/inc/kernelRamDisk.inc
		echo "?>" >> /m23/inc/kernelRamDisk.inc
		chown www-data.www-data /m23/inc/kernelRamDisk.inc
		chmod 644 /m23/inc/kernelRamDisk.inc
	else
		echo "<?PHP" > /tmp/kernelRamDisk.inc
		echo "\$kernelRamDisk=$ramdiskSize;" >> /tmp/kernelRamDisk.inc
		echo "?>" >> /tmp/kernelRamDisk.inc

		diff -q /tmp/kernelRamDisk.inc /m23/inc/kernelRamDisk.inc
		if test $? -ne 0
		then
			echo "Here we need root access"
			su -c "cp /tmp/kernelRamDisk.inc > /m23/inc/kernelRamDisk.inc; chown www-data.www-data /m23/inc/kernelRamDisk.inc; chmod 644 /m23/inc/kernelRamDisk.inc"
		fi
		rm /tmp/kernelRamDisk.inc
	fi

	rm initrd*

	echo -n "Creating empty $ramdiskSize bytes image file..."
	#create an empty image file for storing the root file system
	dd if=/dev/zero of=initrd bs=1k count=$ramdiskSize 2> /dev/null
	echo " done"

	#get a free loop device and assign the initrd to it
	loopDev=`getNextLoop initrd`

	#format loop device
	echo -n "Formating image file..."
	mkfs.ext2 -m 0 -N 10000 $loopDev -q
	echo " done"

	#remove the binding between the loop device and the initrd
	#because there is no connection between the future mount point and the loop device and the loop device would not get freed automatically by unmounting
	echo "losetup -d $loopDev"
	losetup -d $loopDev

	#mount it with loop option => connection the future mount point and the loop device
	if test `whoami` = "root"
	then
		mount -o loop initrd $rootFSDir
	else
		echo "Here we need root access"
		su -c "mount -o loop initrd $rootFSDir"
	fi
}





#Umounts the initrd image, configures the kernel file and compresses initrd. These steps are needed to build the bootimages.
createRFSFinishInitrd()
{
	. /tmp/menuVars
	kernelSDir=`getKernelStoreDirWithArch`

	cd /
	#unmount the initrd
	umount $rootFSDir

	#Disconnect the loop device
	disconnectLoop 'initrd/initrd'

	#prepare the kernel image
	chmod 0555 $kernelSDir/bzImage
	chown root:root $kernelSDir/bzImage
#	rdev $kernelSDir/bzImage /dev/ram0

	#compress the initrd image and move it to the directory
	echo -n "Compressing the initrd..."
	gzip -c $initrdBuildDir/initrd > $initrdBuildDir/initrd.gz
	echo " done"
}





#creates the Etherboot bootimage from the root filesystem
createRFSEtherboot()
{
	. /tmp/menuVars

	arch=`getDebianArch`
	kernelSDir=`getKernelStoreDirWithArch`

	#create the Etherboot image
	mknbi-linux $kernelSDir/bzImage --output=/m23/tftp/m23install-$arch --ip=dhcp --rootdir=/dev/ram0 $initrdBuildDir/initrd.gz
}





#creates the PXE bootimage from the root filesystem
createRFSPXE()
{
	. /tmp/menuVars
	cd $rootFSDir

	arch=`getDebianArch`
	kernelSDir=`getKernelStoreDirWithArch`

	#copy the kernel image
	cp $kernelSDir/bzImage /m23/tftp/m23pxeinstall-$arch

	#copy the compressed initrd image to the directory
	cp $initrdBuildDir/initrd.gz /m23/tftp/initrd-$arch.gz
}





#creates a bootable ISO from the root filesystem
createRFSISO()
{
	. /tmp/menuVars

	#The m23 servername to write into the isolinux.cfg
	m23server=$1
	#Optional directory where to move the created ISO to
	isoDir=$2

	arch=`getDebianArch`

	cd $ISOBuildDir

	#adjust the size of the kernel ramdisk in the isolinux configuration
	ramdiskSize=`getRamDiskSize`
	sed -i "s/ramdisk_size=[0-9]*/ramdisk_size=$ramdiskSize/g" isolinux/isolinux.cfg boot/grub/grub.cfg

	#copy the initrd and kernel to the dir the mkiso expects
	cp $initrdBuildDir/initrd.gz `getKernelStoreDirWithArch`/bzImage isolinux

	#Check if the optional parameter for setting the m23server in the isolinux.cfg should be set
	if [ $m23server ]
	then
		extraName="-$m23server"
		cp isolinux/isolinux.cfg /tmp/isolinux.cfg-createRFSISO.bak
		sed -i "s/APPEND/APPEND m23server=$m23server/" isolinux/isolinux.cfg
	fi

	#make the iso image
	if [ $arch = 'amd64' ]
	then
	# BIOS + EFI for 64 bit systems
		mkisofs -p "The m23 team [http://m23.sf.net]" -A "MDK [m23 development kit]" -T -J -joliet-long -R -V $bootCDPrefix -o $bootCDPrefix-$arch$extraName.iso -b isolinux/isolinux.bin -c isolinux/boot.cat --no-emul-boot -boot-load-size 4 -boot-info-table --eltorito-alt-boot boot/grub/efi.img .

		#xorriso -as mkisofs -isohybrid-mbr /usr/lib/syslinux/mbr/isohdpfx.bin -c isolinux/boot.cat -b isolinux/isolinux.bin -no-emul-boot -boot-load-size 4 -boot-info-table -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -isohybrid-gpt-basdat -o $bootCDPrefix-$arch$extraName.iso .
		
	else
		# BIOS only for 32 bit systems
		mkisofs -p "The m23 team [http://m23.sf.net]" -A "MDK [m23 development kit]" -T -J -joliet-long -R -V $bootCDPrefix -m boot -m efi -m .disk -o $bootCDPrefix-$arch$extraName.iso -b isolinux/isolinux.bin -c isolinux/boot.cat --no-emul-boot -boot-load-size 4 -boot-info-table .
	fi	

	#Check if the iso should be moved to a special directory
	if [ $isoDir ]
	then
		mv $bootCDPrefix-$arch$extraName.iso $isoDir
	else
		#move it to the right directory
		mv $bootCDPrefix-$arch$extraName.iso ..
	fi

	#Copy back the unchanged cfg
	if [ $m23server ]
	then
		cat /tmp/isolinux.cfg-createRFSISO.bak > isolinux/isolinux.cfg
		rm /tmp/isolinux.cfg-createRFSISO.bak
	fi
}





#creates links to multicall binaries
createRFSBinLinks()
{
	. /tmp/menuVars

	cd "$rootFSDir/bin"

	#Checking and creating of file systems
	makeLinks mke2fs mkfs.ext2 mkfs.ext3 mkfs.ext4
	makeLinks mkcfs mkcfs.ext2 mkcfs.ext3 mkcfs.ext4
	makeLinks e2fsck fsck.ext2 fsck.ext3 fsck.ext4

	#LVM tools
	#makeLinks lvchange lvconvert lvcreate lvdisplay lvextend lvm lvmchange lvmdiskscan lvmsadc lvmsar lvreduce lvremove lvrename lvresize lvs lvscan pvchange pvcreate pvdisplay pvmove pvremove pvresize pvs pvscan vgcfgbackup vgcfgrestore vgchange vgck vgconvert vgcreate vgdisplay vgexport vgextend vgimport vgmerge vgmknodes vgreduce vgremove vgrename vgs vgscan vgsplit #lvmiopversion

	#Use the reset funtion of tset
	rm reset 2> /dev/null
	makeLinks tset reset
}





#Downloads basic packages of a choosen architecture and extracts the contents. The extracted files can be used to build a new root file system for the bootimage
downloadExtractRFS()
{
. /tmp/menuVars

arch=`getDebianArch`

rootTempDir=`getRootFSDownloadDirWithArch`

echo -n "Cleaning of \"$rootTempDir\"..."
rm -r -f $rootTempDir
echo " done"
mkdir -p $rootTempDir
cd $rootTempDir

echo "deb http://ftp.de.debian.org/debian/ stretch main non-free contrib
deb http://security.debian.org/ stretch/updates main contrib non-free
deb http://ftp.de.debian.org/debian/ jessie main non-free contrib
deb http://security.debian.org/ jessie/updates main contrib non-free
deb http://ftp.de.debian.org/debian/ wheezy main non-free contrib
deb http://security.debian.org/ wheezy/updates main contrib non-free
deb http://dodger-tools.sourceforge.net/debs ./"> ./sources.list

localArchAptUpdate

localArchAptDownload lvm2 binutils bzip2 grub busybox cryptsetup isc-dhcp-client dialog dmidecode dmraid dropbear e2fsprogs findutils mdadm hdparm sysvinit mbr mount pciutils makedev util-linux dosfstools reiserfsprogs lilo mtools nano coreutils partimage scanlogd snarf tar wget libblkid1 libc6 libbz2-1.0 libuuid1 libselinux1 libsepol1 libpopt0 libpci3 libdebian-installer4 e2fslibs libgcc1 libncurses5 libgcc1 makedev testdisk iputils-ping dmtools parted p7zip discover lsb-base ncurses-bin discover-data hwinfo libdbus-1-3 rsync ntfs-3g ntfsprogs bash mktemp gptsync initscripts gawk dmsetup libudev0 openssh-client firmware-linux-nonfree kmod partclone xfsprogs jfsutils gdisk haveged libhal1-flash

# module-init-tools

# libpcre3 libisccfg-export90 libtasn1-6 libhogweed2

#hwdata-knoppix ddcxinfo-knoppix hwsetup m23hwscanner  #directly extracted from the MDK deb directory
# Not in Wheezy: ms-sys libntfs-3g31 

for deb in `ls archivs/*$arch*.deb archivs/*_all.deb`
do
	dpkg-deb -X $deb .
done

mkdir completeExtracted
# 	dpkg-deb -X /mdk/m23Debs/debs/hwsetup*$arch*.deb completeExtracted
	#The newer releases are containing only BASH code
	dpkg-deb -X /mdk/m23Debs/debs/hwsetup*_i386.deb completeExtracted
# 	read lala

	dpkg-deb -X /mdk/m23Debs/debs/hwdata-knoppix*.deb completeExtracted
	dpkg-deb -X /mdk/m23Debs/debs/ddcxinfo-knoppix*.deb completeExtracted
	dpkg-deb -X /mdk/m23Debs/debs/m23hwscanner_*$arch*.deb completeExtracted
	dpkg-deb -X /mdk/m23Debs/debs/m23hwscanner_*all*.deb completeExtracted
	dpkg-deb -X /mdk/m23Debs/debs/m23-initscripts_*.deb completeExtracted
	dpkg-deb -X archivs/discover-data*.deb completeExtracted
	dpkg-deb -X archivs/lsb-base*.deb completeExtracted
	dpkg-deb -X archivs/firmware-linux-nonfree*.deb completeExtracted

	dpkg-deb -X archivs/dmtools*.deb completeExtracted

	wget http://pciids.sourceforge.net/pci.ids -O completeExtracted/usr/share/hwdata/pci.ids

	#Convert the pci.ids database to the hwinfo DB format.
	mkdir -p completeExtracted/var/lib/hardware
	/mdk/m23helper/convert_hd completeExtracted/usr/share/hwdata/pci.ids
	mv -v hd.ids completeExtracted/var/lib/hardware

echo "Packages extracted"
}
