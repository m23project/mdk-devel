#!/bin/bash
./autoTest.php 1m23server-auf-debian-installieren.m23test 'Debian9-amd64' 192.168.1.96 
			ret=$?

			echo -n "./autoTest.php 1m23server-auf-debian-installieren.m23test 'Debian9-amd64' 192.168.1.96  => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				exit $ret
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM17QianaKDE Linux Mint 17 Qiana Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM17QianaKDE Linux Mint 17 Qiana Mint17KDE => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64jessiedebKdeFull jessie Debian8KdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64jessiedebKdeFull jessie Debian8KdeFull => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64jessiedebCinnamonFull jessie Debian8CinnamonFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64jessiedebCinnamonFull jessie Debian8CinnamonFull => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM171RebeccaMate Linux Mint 17.1 Rebecca Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM171RebeccaMate Linux Mint 17.1 Rebecca Mint17Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM173RosaMate Linux Mint 17.3 Rosa Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM173RosaMate Linux Mint 17.3 Rosa Mint17Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64XenialUnity3DMinimal Ubuntu-Xenial Unity3DMinimal1604
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64XenialUnity3DMinimal Ubuntu-Xenial Unity3DMinimal1604 => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64XenialMate Ubuntu-Xenial Ubuntu1604Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64XenialMate Ubuntu-Xenial Ubuntu1604Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM18SarahCinnamon LinuxMint 18 Sarah Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM18SarahCinnamon LinuxMint 18 Sarah Mint18Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM18SarahKDE LinuxMint 18 Sarah Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM18SarahKDE LinuxMint 18 Sarah Mint18KDE => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM181SerenaCinnamon LinuxMint 18.1 Serena Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM181SerenaCinnamon LinuxMint 18.1 Serena Mint18Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM181SerenaMate LinuxMint 18.1 Serena Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM181SerenaMate LinuxMint 18.1 Serena Mint18Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64stretchdebLxdeFull stretch Debian8LxdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64stretchdebLxdeFull stretch Debian8LxdeFull => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64stretchdebXfceFull stretch Debian8XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64stretchdebXfceFull stretch Debian8XfceFull => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64BionicdesktopMinimal Ubuntu-Bionic UbuntudesktopMinimal1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64BionicdesktopMinimal Ubuntu-Bionic UbuntudesktopMinimal1804 => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64BionicL Ubuntu-Bionic UbuntuLubuntu1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64BionicL Ubuntu-Bionic UbuntuLubuntu1804 => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM19TaraMate LinuxMint 19 Tara Mint19Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM19TaraMate LinuxMint 19 Tara Mint19Mate => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM191TessaCinnamon LinuxMint 19.1 Tessa Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM191TessaCinnamon LinuxMint 19.1 Tessa Mint19Cinnamon => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.96/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTdeb9amd64LM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce => " >> aTS-Debian9-amd64.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			