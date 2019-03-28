#!/bin/bash
./autoTest.php 1m23server-auf-UCS-installieren.m23test 'UCS 4.4 - Lokales Testrepo' 192.168.1.144 
			ret=$?

			echo -n "./autoTest.php 1m23server-auf-UCS-installieren.m23test 'UCS 4.4 - Lokales Testrepo' 192.168.1.144  => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				exit $ret
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM17QianaCinnamon Linux Mint 17 Qiana Mint17Cinnamon => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM17QianaKDE Linux Mint 17 Qiana Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM17QianaKDE Linux Mint 17 Qiana Mint17KDE => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepojessiedebKdeFull jessie Debian8KdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepojessiedebKdeFull jessie Debian8KdeFull => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepojessiedebLxdeFull jessie Debian8LxdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepojessiedebLxdeFull jessie Debian8LxdeFull => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoXenialGnome Ubuntu-Xenial UbuntuGnome1604
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoXenialGnome Ubuntu-Xenial UbuntuGnome1604 => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoXenialK Ubuntu-Xenial UbuntuKubuntu1604
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoXenialK Ubuntu-Xenial UbuntuKubuntu1604 => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM181SerenaMate LinuxMint 18.1 Serena Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM181SerenaMate LinuxMint 18.1 Serena Mint18Mate => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepostretchdebMateFull stretch Debian8MateFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepostretchdebMateFull stretch Debian8MateFull => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepostretchdebXfceFull stretch Debian8XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepostretchdebXfceFull stretch Debian8XfceFull => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM183SylviaCinnamon LinuxMint 18.3 Sylvia Mint18Cinnamon => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoBionicBudgie Ubuntu-Bionic UbuntuBudgie1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoBionicBudgie Ubuntu-Bionic UbuntuBudgie1804 => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoBionicL Ubuntu-Bionic UbuntuLubuntu1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoBionicL Ubuntu-Bionic UbuntuLubuntu1804 => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM19TaraMate LinuxMint 19 Tara Mint19Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM19TaraMate LinuxMint 19 Tara Mint19Mate => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.144/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTUCS44LokalesTestrepoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce => " >> aTS-UCS4.4-LokalesTestrepo.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			