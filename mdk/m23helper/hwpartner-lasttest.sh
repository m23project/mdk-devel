#!/bin/bash


workstation=" dbench sunflow"
server=" sunflow"




#Finds the mount point with the maximum avalable space and creates a temporary directory on it and shows its 
getTempDir()
{
	#Get the mount point with the most available free space amount
	mountPointWithMaxFreeDiskSpace=$(df -P -l | tr -s '[:blank:]' | awk '{print($4"#"$6)}' | sort -n | tail -1 | cut -d'#' -f2)
	tmpDir="$mountPointWithMaxFreeDiskSpace/$$ghtest"
	mkdir -p "$tmpDir"
	if [ $? -ne 0 ]
	then
		failSuccessLog "Creating temp directory $tmpDir" 1
		exit 1
	fi
	echo $tmpDir
}





#Deletes the temporary directory
delTempDir()
{
	cd /tmp
	rm -r $1
}





#Downloads and extracts a tar.bz2 archive
dlExtract()
{
	dlDir="~/gh-hwpartner/downloads/"

	mkdir -p $dlDir

	if [ ! -f $dlDir$1 ]
	then
		wget http://downloads.sourceforge.net/project/m23/benchmarks/$1 -O $dlDir$1
		failSuccessLog "Downloading $1" $?
	fi

	tar xjf $dlDir$1
	failSuccessLog "Extraction of $1" $?
}





#Logs an info or error message
#parameter 1: Message text
#parameter 2: 0 for info, enything else for an error message
failSuccessLog()
{
	if [ $2 -eq 0 ]
	then
		echo "$1 complete"
	else
		echo "Error: $1 failed"
	fi
}



log()
{
	echo "$1"
}




#requires disk: sysbench dbench
disk()
{
	temp="$(getTempDir)"
	cd $temp
	dbench -D $temp -t 60 2
	dbench -D $temp -s -S -t 60 2

	cd $temp
	rm -r *
	sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw prepare
	sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw run
	sysbench --num-threads=16 --test=fileio --file-total-size=3G --file-test-mode=rndrw cleanup
}





#requires openGL: nexuiz
openGL()
{
	rm -r ~/.nexuiz/
	for i in seq 1 5
	do
		/usr/games/nexuiz -benchmark demos/demo$i -nosound -fullscreen 2>&1 | egrep -e '[0-9]+ frames'
	done
}




#requires java: sunflow
java()
{
	sunflow -Xmx1G -nogui -bench
}





#requires office: libreoffice-writer imagemagick
office()
{
	temp="$(getTempDir)"
	cd $temp



	log "LibreOffice tests..."
	#All odt files from http://de.libreoffice.org/hilfe-kontakt/handbuecher/ licensed under Creative Commons Attribution-Share Alike 3.0 License.
	dlExtract lohb-test.tar.bz2
	mkdir conv
	soffice --convert-to pdf --headless --outdir conv lohb/*.odt
	rm -r *



	log "ImageMagick tests..."
	#Images are from wikipedia and open clipart for licensing details see the included license.txt
	dlExtract images.tar.bz2
	mkdir conv
	for name in $(ls openclipart)
	do
		convert -bench 10 "openclipart/$name" -sharpen 0x1 null:
		failSuccessLog "Converting $name" $?
	done
	
	delTempDir $temp
}





#requires encoding: mplayer vorbis-tools
encoding()
{
	temp="$(getTempDir)"
	#temp="/mnt/encfs/14585ghtest"
	cd $temp

	log "Audio/video encoding tests..."
	
	#Music files from http://www.jamendo.com/de/list/a79368/fairytale-2 by zero-project from the album Fairytale 2 licensed under Attribution 3.0 Unported (CC BY 3.0)
	dlExtract Fairytale_2_-_a79368_---_Jamendo_-_MP3_VBR_192k.tar.bz2
	mkdir wavogg
	cd Fairytale_2_-_a79368_---_Jamendo_-_MP3_VBR_192k

	#Convert all mp3s to wav and the wavs to oggs
	for i in $(ls  *.mp3)
	do
		mplayer -vo null -ao pcm:fast:waveheader:file="../wavogg/${i}.wav" "$i"
		oggenc -q10 "../wavogg/${i}.wav"
	done

	#Get the number of CPUs
	threads=$(grep processor /proc/cpuinfo -c)

	#Video file from http://www.bigbuckbunny.org/index.php/download/ licensed under Creative Commons Attribution 3.0 by (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
	dlExtract big_buck_bunny_720p_surround.tar.bz2

	#Convert the 1280x720 Mpeg4/AC3 video to Xvid/AAC
	mencoder big_buck_bunny_720p_surround.avi -oac faac -ovc xvid -xvidencopts bitrate=1200:threads=$threads -o big_buck_bunny_720p_surround-recoded.avi

	delTempDir $temp
}

db()
{
	echo 1
}




#requires memory: memtester
memory()
{
	memtester 256M
}



#requires disk: sysbench
cpu()
{
	sysbench --test=cpu --cpu-max-prime=1000000 --num-threads=64 run
}

net()
{
	temp="$(getTempDir)"
	cd $temp

	dlExtract xampp-linux-1.8.0.tar.bz2
	dlExtract LAMPLoadTest.tar.bz2

	mv lampp /opt/
	mv LAMPLoadTest.php /opt/lampp/htdocs/xampp

	/opt/lampp/lampp startssl
	/opt/lampp/lampp startmysql
}





#Installs the needed packages for one or more tests
installRequirements()
{
	packages=""
	for req in $1
	do
		packages="$packages $(grep "#requires $req: " $0 | sed -e 's/.*: //' | awk -vORS=' ' '{print}')"
	done

	sudo apt-get install -y $packages
}


net

#installRequirements "office encoding"