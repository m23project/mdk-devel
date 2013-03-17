#!/bin/bash





#Installs PTS and the pre-cached test archive
installPTS()
{
	type="$1"
	err=0

	if [ $(dpkg --get-selections | grep phoronix-test-suite | grep -v deinstall -c) -gt 0 ] && [ $(dpkg -s phoronix-test-suite | grep Version | cut -d' ' -f2) = '4.0.0' ]
	then
		echo "PTS is already installed!"
	else
		apt-get update
		cd /tmp

		#Download the PTS package
		wget http://switch.dl.sourceforge.net/project/m23/PTS/phoronix-test-suite_4.0.0_all.deb
		err=$[ $err + $? ]
		#Install it
		dpkg -i phoronix-test-suite_4.0.0_all.deb
		err=$[ $err + $? ]
		#Fix missing dependencies
		apt-get install -f -y
		#Install recommended packages for PTS
		apt-get install -y php5-curl php-fpdf
	fi

	#Check if there is config dir
	if [ ! -d ~/.phoronix-test-suite ]
	then
		#Download and extract the configuration
		cd ~
		wget http://switch.dl.sourceforge.net/project/m23/PTS/PTS-config.tar.gz
		err=$[ $err + $? ]
		tar xfvz PTS-config.tar.gz
		err=$[ $err + $? ]
		rm PTS-config.tar.gz

		#Download and extract the cached tests
		cd ~/.phoronix-test-suite
		if [ $type = "server" ]
		then
			archiv="PTS-server.tar"
		else
			archiv="PTS-client.tar"
		fi

		wget http://switch.dl.sourceforge.net/project/m23/PTS/$archiv
		err=$[ $err + $? ]
		tar xvf $archiv
		err=$[ $err + $? ]
		rm $archiv
	fi

	if [ $err -ne 0 ]
	then
		echo "There were error while installing/configuring PTS!"
		exit 1
	fi
}





#Configures the tests
configTests()
{
	#Get the mount point with the most available free space amount
	$mountPointWithMaxFreeDiskSpace=$(df -P -l | tr -s '[:blank:]' | awk '{print($4"#"$6)}' | sort -n | tail -1 | cut -d'#' -f2)

	#Download and extract the configuration
	cd ~
	wget http://switch.dl.sourceforge.net/project/m23/PTS/PTS-configTests.tar.gz
	err=$[ $err + $? ]
	tar xfvz PTS-configTests.tar.gz
	rm PTS-configTests.tar.gz

	#Adjust the directory for storing the temporary files
	sed -i "s#scratch#$mountPointWithMaxFreeDiskSpace/fs-mark-temp#" ~/.phoronix-test-suite/test-profiles/pts/fs-mark-1.0.0/test-definition.xml
	
}



runTests()
{
	type="$1"

	if [ $type = "server" ]
	then
		tests="pts/apache pts/compress-gzip pts/fs-mark pts/ramspeed pts/java-scimark2 pts/openssl"
	else
		tests="pts/nexuiz"
		# pts/compress-gzip pts/pgbench pts/fs-mark pts/ramspeed pts/java-scimark2 pts/openssl pts/encode-mp3 pts/graphics-magick pts/mencoder pts/povray"
		
		
		
# 		pts/apache-1.4.0           - Apache Benchmark
# pts/bork-1.0.0             - Bork File Encrypter
# pts/build-php-1.3.1        - Timed PHP Compilation
# pts/c-ray-1.1.0            - C-Ray
# pts/cachebench-1.0.0       - CacheBench
# pts/compress-7zip-1.6.0    - 7-Zip Compression
# pts/compress-gzip-1.1.0    - Gzip Compression
# pts/compress-lzma-1.2.0    - LZMA Compression
# pts/compress-pbzip2-1.3.0  - Parallel BZIP2 Compression
# pts/encode-flac-1.2.0      - FLAC Audio Encoding
# pts/encode-mp3-1.4.0       - LAME MP3 Encoding
# pts/etqw-1.1.0             - ET: Quake Wars
# pts/etqw-demo-1.1.0        - ET: Quake Wars Demo
# pts/fs-mark-1.0.0          - FS-Mark
# pts/gcrypt-1.0.0           - Gcrypt Library
# pts/gnupg-1.3.1            - GnuPG
# pts/graphics-magick-1.4.1  - GraphicsMagick
# pts/himeno-1.1.0           - Himeno Benchmark
# pts/hmmer-1.1.0            - Timed HMMer Search
# pts/iozone-1.8.0           - IOzone
# pts/j2dbench-1.1.0         - Java 2D Microbenchmark
# pts/java-scimark2-1.1.1    - Java SciMark
# pts/jgfxbat-1.1.0          - Java Graphics Basic Acceptance Test
# pts/john-the-ripper-1.3.0  - John The Ripper
# pts/mafft-1.4.0            - Timed MAFFT Alignment
# pts/mencoder-1.3.0         - Mencoder
# pts/network-loopback-1.0.0 - Loopback TCP Network Performance
# pts/nexuiz-1.6.0           - Nexuiz
# pts/nginx-1.1.0            - NGINX Benchmark
# pts/openssl-1.6.0          - OpenSSL
# pts/pgbench-1.4.0          - PostgreSQL pgbench
# pts/phpbench-1.0.0         - PHPBench
# pts/postmark-1.0.0         - PostMark
# pts/povray-1.0.0           - POV-Ray
# pts/prey-1.1.2             - Prey
# pts/ramspeed-1.4.0         - RAMspeed SMP
# pts/smallpt-1.0.1          - Smallpt
# pts/stream-1.1.0           - Stream
# pts/sunflow-1.1.0          - Sunflow Rendering System
# pts/tachyon-1.1.0          - Tachyon
# pts/tiobench-1.1.0         - Threaded I/O Tester
# pts/tremulous-1.1.0        - Tremulous
# pts/tscp-1.0.0             - TSCP
# pts/unigine-sanctuary-1.5.1 - Unigine Sanctuary
# pts/unigine-tropics-1.5.2  - Unigine Tropics
# pts/unpack-linux-1.0.0     - Unpacking The Linux Kernel
# pts/warsow-1.4.1           - Warsow

	fi
	
	phoronix-test-suite batch-install $tests
	configTests
	#phoronix-test-suite make-download-cache
	phoronix-test-suite batch-benchmark $tests
}


makeResultPDF()
{
	ls ~/.phoronix-test-suite/test-results/ | grep "^20" | xargs phoronix-test-suite merge-results > /tmp/res
	phoronix-test-suite result-file-to-pdf $(grep merge /tmp/res | sed 's#/#\n#g' | grep merge)
	rm /tmp/res
# 	phoronix-test-suite show-result
# 	phoronix-test-suite list-saved-results
# 	phoronix-test-suite result-file-to-pdf apache-test
}


installPTS server
runTests server
makeResultPDF


exit 0


getCacheFiles()
{
	for testName in "$@"
	do
		grep URL ~/.phoronix-test-suite/test-profiles/$testName*/downloads.xml 2> /dev/null | sed 's/[, <>]/\n/g' | grep '//' | xargs -n1 basename 2> /dev/null | sort -u | while read archive
		do
			if [ -f ~/.phoronix-test-suite/download-cache/$archive ]
			then
				echo -n "download-cache/$archive "
			fi
		done
	done
	echo "download-cache/pts-download-cache.xml"
}


