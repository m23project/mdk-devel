#!/bin/bash
./autoTest.php 1m23server-auf-debian-installieren.m23test 'Debian9-i386' 192.168.1.93 
			ret=$?

			echo -n "./autoTest.php 1m23server-auf-debian-installieren.m23test 'Debian9-i386' 192.168.1.93  => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				exit $ret
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM17QianaKDE Linux Mint 17 Qiana Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM17QianaKDE Linux Mint 17 Qiana Mint17KDE => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386jessiedebMate jessie Debian8Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386jessiedebMate jessie Debian8Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386jessiedebLxdeFull jessie Debian8LxdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386jessiedebLxdeFull jessie Debian8LxdeFull => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM171RebeccaXfceFull Linux Mint 17.1 Rebecca Mint17XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM171RebeccaXfceFull Linux Mint 17.1 Rebecca Mint17XfceFull => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM173RosaMate Linux Mint 17.3 Rosa Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM173RosaMate Linux Mint 17.3 Rosa Mint17Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386XenialMate Ubuntu-Xenial Ubuntu1604Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386XenialMate Ubuntu-Xenial Ubuntu1604Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386XenialTrinity Ubuntu-Xenial Ubuntu1604Trinity
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386XenialTrinity Ubuntu-Xenial Ubuntu1604Trinity => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM18SarahKDE LinuxMint 18 Sarah Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM18SarahKDE LinuxMint 18 Sarah Mint18KDE => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM18SarahXfce LinuxMint 18 Sarah Mint18Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM18SarahXfce LinuxMint 18 Sarah Mint18Xfce => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM181SerenaMate LinuxMint 18.1 Serena Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM181SerenaMate LinuxMint 18.1 Serena Mint18Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386stretchdebCinnamonFull stretch Debian8CinnamonFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386stretchdebCinnamonFull stretch Debian8CinnamonFull => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386stretchdebKdeFull stretch Debian8KdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386stretchdebKdeFull stretch Debian8KdeFull => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386BionicK Ubuntu-Bionic UbuntuKubuntu1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386BionicK Ubuntu-Bionic UbuntuKubuntu1804 => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386Bionicdesktop Ubuntu-Bionic Ubuntudesktop1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386Bionicdesktop Ubuntu-Bionic Ubuntudesktop1804 => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM19TaraXfce LinuxMint 19 Tara Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM19TaraXfce LinuxMint 19 Tara Mint19Xfce => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM191TessaCinnamon LinuxMint 19.1 Tessa Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9i386LM191TessaCinnamon LinuxMint 19.1 Tessa Mint19Cinnamon => " >> aTS-Debian9-i386.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			