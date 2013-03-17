#!/bin/bash





#Checks the USB ports by mounting, writing and reading a prepared USB stick
usbTest()
{
	tmp="/tmp/usbTestMount"
	mkdir -p $tmp

	while `true`
	do
		echo "USB-Stick einstecken und Enter drücken"
		read lala

		sleep 3
		#Get the last plugged in USB stick
		dev=$(dmesg | grep 'Attached SCSI removable disk' -B5 | tail -5 | grep ']  ' | sed 's/.*\(sd[a-z]1\)/\1/')

		if [ -z "$dev" ]
		then
			echo "Kein USB-Stick mit einer ext?-Partition (z.B. /dev/sdb1) gefunden
	=> USB-Stick mit einer Primärpartition anlegen
	=> Partition mit ext4 formatieren
	=> Mounten und dort Datei \"hwpartner.txt\" anlegen"
			continue
		fi

		#Generate correct device name
		dev="/dev/$dev"

		umount $dev 2> /dev/null

		#Mount the device
		mount -o sync $dev $tmp
		if [ $? -ne 0 ]
		then
			echo "$dev konnte nicht gemountet werden."
			continue
		else
			echo "$dev erfolgreich gemountet."
		fi

		#Check if it is the right USB stick
		if [ -f "$tmp/hwpartner.txt" ]
		then
			echo "$tmp/hwpartner.txt gefunden => richtiger USB-Stick"
		else
			echo "$tmp/hwpartner.txt fehlt => falscher USB-Stick"
			continue
		fi

		randFile="/tmp/randfile.bin"
		usbRandFile="$tmp/randfile.bin"
		
		dd if=/dev/urandom bs=1M count=64 of=$randFile
		md5sum $randFile > /tmp/sticktester.log
		sed -i "s#$randFile#$usbRandFile#" /tmp/sticktester.log
		
		#Write the random file
		mv $randFile $usbRandFile
		if [ $? -eq 0 ]
		then
			echo "$usbRandFile erfolgreich geschrieben."
		else
			echo "Fehler beim Schreiben von $usbRandFile."
			continue
		fi

		#Clear read caches
		sync
		echo 3 > /proc/sys/vm/drop_caches

		#Check the random file
		md5sum -c /tmp/sticktester.log
		if [ $? -eq 0 ]
		then
			echo "$usbRandFile erfolgreich überprüft."
		else
			echo "Fehler beim Überprüfen von $usbRandFile."
			continue
		fi

		umount $dev 2> /dev/null
	done
}





#Checks, if ALSA device can be used for outputting sounds. It plays a sound as long as the user interrupts
sucheAusgabe()
{
	dev="$1"
	echo "Ausgabesoundkarte suchen: Kopfhörer einstecken"
	while `true`
	do
		echo "Drücke Enter"
		read lala
		aplay -D $dev /usr/share/sounds/alsa/Front_Center.wav
		echo "Kommt Ton? (j/n)"
		read lala
		if [ $lala = "j" ]
		then
			break
		fi
	done
}





#Tests sound output and input.
soundTest()
{
	installRequirements "alsa-utils"

	echo "Anzahl der Ausgänge?"
	read num
	
	for i in seq 1 $num
	do
		echo "Kopfhörer einstecken, dann Enter drücken:"
		read lala
		speaker-test -c 6 -l 1 -t wav
	done

	echo "Aufnahmetest"

	export LC_ALL=C; arecord -l | grep '^card' | sed 's#card \([0-9]*\).*device \([0-9]*\).*#hw:\1,\2#' | while read dev
	do
		echo "ALSA: $dev"
		sucheAusgabe $dev
	
		while `true`
		do
			echo "Mikrofon/Line einstecken, dann Enter drücken:"
			read lala
			arecord -D $dev -d 10 /tmp/test-mic.wav
			aplay -D $dev /tmp/test-mic.wav
			echo "Kommt Ton? (j/n)"
			read lala
			if [ $lala = "j" ]
			then
				break
			fi
		done
	done

	exit 0
}





#Stores all logs and gathered information into a zip file.
zipInfo()
{
	#Check, if the log files are there
	if [ $(ls ~/hwpartner* -1 2> /dev/null | wc -l) -lt 2 ]
	then
		echo "The (two) info files (~/hwpartner*) are missing. Call \"$0 workstation/server\" before."
		exit 1
	fi

	installRequirements "dmidecode pciutils zip"
	lspci -k -x > ~/hwpartner-lspci.txt
	dmidecode > ~/hwpartner-dmi.txt

	chmod 755 ~/hwpartner*

	zip -9 ~/info-zert.zip ~/hwpartner*

	chmod 755 ~/info-zert.zip
	
	exit 0
}





#Shows information about the PCI device(s) of a class
extraPCIInfo()
{
	lspci -k -x | egrep -B5 '(Kernel modules|Kernel driver in use)' | grep -A5 -i "$1" | awk 'BEGIN {
	show=1
	}
	/--/ {
	show=!show;
	}

	{
		if (show == 1)
			print
	}
	' | grep -v Subsystem
}





#Shows system information that can be used for the m23-Hardware-Zertifikat
systemInfo()
{
installRequirements "dmidecode parted util-linux pciutils"

echo 'CPU(s):'
cat /proc/cpuinfo | grep "model name" | tr -s '[:blank:]' | sed 's/model name[ \t]*: //g' | awk '{ i += 1; print i": "$0 }'

echo 'CPU(s) mit Virtualisierungsfunktion:'
egrep -c '(vmx|svm)' /proc/cpuinfo

echo 'CPU(s) mit 64-Bit-Unterstützung:'
grep -c ' lm ' /proc/cpuinfo

echo 'CPU-Info:'
lscpu



echo 'Ram:'
dmidecode --type memory | grep Size | tr -s '[:blank:]' | sed 's/[ \t]*Size: //g' | awk '{ i += 1; print i": "$0 }'

echo 'Ram-Hersteller:'
dmidecode --type memory | grep Manufacturer | tr -s '[:blank:]' | sed 's/[ \t]*Manufacturer: //g' | awk '{ i += 1; print i": "$0 }'



echo 'Grafikkarte:'
extraPCIInfo 'VGA compatible controller'



echo 'Festplatte(n):'
parted -l | grep '/dev/'
extraPCIInfo 'IDE interface'
extraPCIInfo 'SATA controller'



echo 'Soundkarte'
extraPCIInfo 'Audio device'



echo 'USB'
extraPCIInfo 'USB Controller'



echo 'Netzwerkkarte(n)'
extraPCIInfo 'Ethernet controller'

exit 0
}





#Finds the mount point with the maximum avalable space and creates a temporary directory on it and shows its 
getTempDir()
{
	#Get the mount point with the most available free space amount
	mountPointWithMaxFreeDiskSpace=$(df -P -l | tr -s '[:blank:]' | awk '{print($4"#"$6)}' | sort -n | tail -1 | cut -d'#' -f2)
	tmpDir="$mountPointWithMaxFreeDiskSpace/tmp/$$ghtest"
	mkdir -p "$tmpDir"
	if [ $? -ne 0 ]
	then
		failSuccessLog "Creating temp directory $tmpDir" 1
		exit 1
	fi
	echo $tmpDir
}





#Downloads and extracts a tar.bz2 archive
#parameter 1: The name of the tar.bz2 archive to download and extract
#parameter 2: Set to "1", if the file should be extraced with root rights
dlExtract()
{
	dlDir="$HOME/gh-hwpartner/downloads/"

	mkdir -p $dlDir

	if [ ! -f $dlDir$1 ]
	then
		(axel -n4 -a http://downloads.sourceforge.net/project/m23/benchmarks/$1 -o $dlDir$1 2>&1; echo $? > /tmp/ret) | log
		failSuccessLog "Downloading $1" $(cat /tmp/ret)
	fi

	if [ $2 ] && [ $2 -eq 1 ]
	then
		asudo=$(addSudo)
	else
		asudo=""
	fi
	
	($asudo tar xjf $dlDir$1 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "Extraction of $1" $(cat /tmp/ret)
}





#Generates a human readable and sortable date/time stamp
mkDate()
{
	date +"%Y-%m-%d %H:%M:%S"
}





#Logs a completion or error message
#parameter 1: Message text
#parameter 2: 0 for info, enything else for an error message
failSuccessLog()
{
	d=$(mkDate)
	if [ $2 -eq 0 ]
	then
		echo "$d>>O>> $1 complete" | tee -a ~/hwpartner.log | log
	else
		echo "$d>>E>> Error: $1 failed" | tee -a ~/hwpartner.log | log
	fi
}





#Logs an info message
#parameter 1: Message text
info()
{
	d=$(mkDate)
	echo "$d>>I>> $1" | tee -a ~/hwpartner.log | log
}




log()
{
	tee -a ~/hwpartner-complete.log
}



#requires disk: sysbench dbench
disk()
{
	info "Running disk benchmarks"

	temp="$(getTempDir)"
	cd $temp
	info "Running dbench"
	(dbench -D $temp -t 60 2 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "dbench -D $temp -t 60 2" $(cat /tmp/ret)
	(dbench -D $temp -s -S -t 60 2 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "dbench -D $temp -s -S -t 60 2" $(cat /tmp/ret)

	cd $temp
	info "Running sysbench"
	(sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw prepare 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "sysbench prepare" $(cat /tmp/ret)
	(sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw run  --max-time=600 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "sysbench run" $(cat /tmp/ret)
	(sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw cleanup 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "sysbench cleanup" $(cat /tmp/ret)
}





#requires openGL: nexuiz
openGL()
{
	info "Running openGL tests"
	rm -r ~/.nexuiz/
	for i in $(seq 1 5)
	do
		(/usr/games/nexuiz -benchmark demos/demo$i -nosound -fullscreen 2>&1; echo $? > /tmp/ret) | egrep -e '[0-9]+ frames' 2>&1 | log
		failSuccessLog "nexuiz demo$i" $(cat /tmp/ret)
	done
}





#requires office: libreoffice-writer imagemagick
office()
{
	temp="$(getTempDir)"
	cd $temp



	info "LibreOffice tests..."
	#All odt files from http://de.libreoffice.org/hilfe-kontakt/handbuecher/ licensed under Creative Commons Attribution-Share Alike 3.0 License.
	dlExtract lohb-test.tar.bz2
	mkdir conv
	(soffice --convert-to pdf --headless --outdir conv lohb/*.odt 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "Converted ODT files to PDF" $(cat /tmp/ret)



	info "ImageMagick tests..."
	#Images are from wikipedia and open clipart for licensing details see the included license.txt
	dlExtract images.tar.bz2
	mkdir conv
	for name in $(ls openclipart)
	do
		(convert -bench 10 "openclipart/$name" -sharpen 0x1 null: 2>&1; echo $? > /tmp/ret) | log
		failSuccessLog "Converted $name" $(cat /tmp/ret)
	done
}





#requires encoding: mplayer vorbis-tools mencoder
encoding()
{
	temp="$(getTempDir)"
	cd $temp

	info "Audio/video encoding tests..."
	
	#Music files from http://www.jamendo.com/de/list/a79368/fairytale-2 by zero-project from the album Fairytale 2 licensed under Attribution 3.0 Unported (CC BY 3.0)
	dlExtract Fairytale_2_-_a79368_---_Jamendo_-_MP3_VBR_192k.tar.bz2
	mkdir wavogg
	cd Fairytale_2_-_a79368_---_Jamendo_-_MP3_VBR_192k

	#Convert all mp3s to wav and the wavs to oggs
	for i in $(ls  *.mp3)
	do
		(mplayer -vo null -ao pcm:fast:waveheader:file="../wavogg/${i}.wav" "$i" 2>&1; echo $? > /tmp/ret) | log
		failSuccessLog "Converted ${i} to WAV" $(cat /tmp/ret)
		(oggenc -q10 "../wavogg/${i}.wav" 2>&1; echo $? > /tmp/ret) | log
		failSuccessLog "Converted ${i}.wav to OGG" $(cat /tmp/ret)
	done

	#Get the number of CPUs
	threads=$(grep processor /proc/cpuinfo -c)

	#Video file from http://www.bigbuckbunny.org/index.php/download/ licensed under Creative Commons Attribution 3.0 by (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
	dlExtract big_buck_bunny_720p_surround.tar.bz2

	#Convert the 1280x720 Mpeg4/AC3 video to Xvid/MP2
	(mencoder big_buck_bunny_720p_surround.avi -oac lavc -lavcopts acodec=mp2 -ovc xvid -xvidencopts bitrate=1200:threads=$threads -o big_buck_bunny_720p_surround-recoded.avi 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "Re-Encoded AVI" $(cat /tmp/ret)
}





#requires memory: memtester
memory()
{
	info "Memory tests..."
	($(addSudo) memtester 128M 10 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "memtester 128M" $(cat /tmp/ret)
}





#requires disk: sysbench
cpu()
{
	info "CPU tests..."
	(sysbench --test=cpu --cpu-max-prime=1000000 --num-threads=64 --max-time=600 run 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "sysbench cpu run" $(cat /tmp/ret)
}





#Setups a LAMP server
LAMPprepare()
{
	temp="$(getTempDir)"
	cd $temp

	info "Installing LAMP stack..."
	dlExtract xampp-linux-1.8.0.tar.bz2 1
	dlExtract LAMPLoadTest.tar.bz2

	$(addSudo) rm -r /opt/lampp
	$(addSudo) ln -s $temp/lampp /opt/
	failSuccessLog "LAMP installed" $?
	$(addSudo) mv LAMPLoadTest.php /opt/lampp/htdocs/xampp

	($(addSudo) /opt/lampp/lampp startssl 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "Apache+SSL started" $(cat /tmp/ret)
	($(addSudo) /opt/lampp/lampp startmysql 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "MySQL started" $(cat /tmp/ret)

	serverIP=$(export LC_ALL=C; ifconfig | grep eth -A2 | grep "inet addr" | cut -d':' -f2 | cut -d' ' -f1 | head -1)
	
	echo "Start the LAMP test from another system with:"
	echo "siege https://$serverIP/xampp/LAMPLoadTest.php -l/tmp/siege.log"
	echo "Then continue this script with pressing y+Enter"

	while read key
	do
		if [ $key = 'y' ]
		then
			break
		fi
		sleep 1
	done
}





#Outputs "sudo", if the user is not root
addSudo()
{
	if [ $(whoami) = 'root' ]
	then
		echo -n ""
	else
		echo -n "sudo"
	fi
}





#Installs the needed packages for one or more tests
installRequirements()
{
	($(addSudo) apt-get update 2>&1; echo $? > /tmp/ret) | log
	failSuccessLog "APT cache updated" $(cat /tmp/ret)
	packages="axel bzip2"
	for req in $1
	do
		packages="$packages $(grep "#requires $req: " $0 | sed -e 's/.*: //' | awk -vORS=' ' '{print}')"
	done
	
	($(addSudo) apt-get install -y $packages 2>&1; echo $? > /tmp/ret) | log
}





#Shows a help screen and exits
showhelp()
{
	echo "$0 <workstation/server/powerSV/powerWS/systemInfo/zipInfo/soundTest/usbTest>"
	echo "workstation: Runs tests for graphic workstations"
	echo "server: Runs tests for servers"
	echo "powerSV: Try to comsume the maximum power on a server"
	echo "powerWS: Try to comsume the maximum power on a workstation"
	echo "playVideo: Plays a video file with the MPlayer, the gMPlayer and xine"
	echo "systemInfo: Shows system information that can be used for the m23-Hardware-Zertifikat"
	echo "zipInfo: Stores all logs and gathered information into a zip file."
	echo "soundTest: Tests sound output and input."
	echo "usbTest: Checks the USB ports by mounting, writing and reading a prepared USB stick"
	exit 1
}





#Try to comsume the maximum power on a server
powerSV()
{
	installRequirements "cpu memory disk"
	LAMPprepare
	cpu&
	disk&
	memory
}





#Try to comsume the maximum power on a workstation
powerWS()
{
	installRequirements "cpu openGL disk"
	LAMPprepare
	cpu&
	disk&
	openGL
}





#Plays a video file with the MPlayer, the gMPlayer and xine
#requires playVideo: mplayer xine-ui
playVideo()
{
	temp="$(getTempDir)"
	cd $temp

	#Video file from http://www.bigbuckbunny.org/index.php/download/ licensed under Creative Commons Attribution 3.0 by (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
	dlExtract big_buck_bunny_720p_surround.tar.bz2

	mplayer big_buck_bunny_720p_surround.avi
	gmplayer big_buck_bunny_720p_surround.avi
	xine big_buck_bunny_720p_surround.avi
}





main()
{
	case "$1" in
		'workstation')
			tests="LAMPprepare disk cpu memory encoding office openGL"
		;;
		'server')
			tests="LAMPprepare disk cpu memory"
		;;
		'powerSV')
			powerSV
		;;
		'powerWS')
			powerWS
		;;
		'playVideo')
			tests="playVideo"
		;;
		'systemInfo')
			systemInfo
		;;
		'zipInfo')
			zipInfo
		;;
		'soundTest')
			soundTest
		;;
		'usbTest')
			usbTest
		;;
		*)
			showhelp
		;;
	esac

	installRequirements "$tests"

	for testname in $tests
	do
		$testname
	done
}

main $1
