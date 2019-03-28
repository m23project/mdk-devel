#!/bin/bash
./autoTest.php 1m23server-auf-UCS-installieren.m23test 'UCS 4.3 - Lokales Testrepo' 192.168.1.143 
			ret=$?

			echo -n "./autoTest.php 1m23server-auf-UCS-installieren.m23test 'UCS 4.3 - Lokales Testrepo' 192.168.1.143  => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				exit $ret
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepojessiedebMate jessie Debian8Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepojessiedebMate jessie Debian8Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepojessiedebXfceFull jessie Debian8XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepojessiedebXfceFull jessie Debian8XfceFull => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM171RebeccaMate Linux Mint 17.1 Rebecca Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM171RebeccaMate Linux Mint 17.1 Rebecca Mint17Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoXenialTrinity Ubuntu-Xenial Ubuntu1604Trinity
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoXenialTrinity Ubuntu-Xenial Ubuntu1604Trinity => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoXenialGnome Ubuntu-Xenial UbuntuGnome1604
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoXenialGnome Ubuntu-Xenial UbuntuGnome1604 => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM181SerenaXfce LinuxMint 18.1 Serena Mint18Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM181SerenaXfce LinuxMint 18.1 Serena Mint18Xfce => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepostretchdebXfceFull stretch Debian8XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepostretchdebXfceFull stretch Debian8XfceFull => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepostretchdebMate stretch Debian8Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepostretchdebMate stretch Debian8Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoBionicMate Ubuntu-Bionic UbuntuMate1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoBionicMate Ubuntu-Bionic UbuntuMate1804 => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoBionicdesktop Ubuntu-Bionic Ubuntudesktop1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoBionicdesktop Ubuntu-Bionic Ubuntudesktop1804 => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.143/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS43LokalesTestrepoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce => " >> aTS-UCS4.3-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			