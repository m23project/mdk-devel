#!/bin/bash
./autoTest.php 1m23server-iso-install.m23test 'atISOm23Server' /crypto/iso/m23server_19.1_rock-devel.iso 192.168.1.24 
			ret=$?

			echo -n "./autoTest.php 1m23server-iso-install.m23test 'atISOm23Server' /crypto/iso/m23server_19.1_rock-devel.iso 192.168.1.24  => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				exit $ret
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM17QianaXfceFull Linux Mint 17 Qiana Mint17XfceFull => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM17QianaKDE Linux Mint 17 Qiana Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM17QianaKDE Linux Mint 17 Qiana Mint17KDE => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisojessiedebKdeFull jessie Debian8KdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisojessiedebKdeFull jessie Debian8KdeFull => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisojessieTrinity jessie Trinity
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisojessieTrinity jessie Trinity => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM171RebeccaKDE Linux Mint 17.1 Rebecca Mint17KDE => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM171RebeccaCinnamon Linux Mint 17.1 Rebecca Mint17Cinnamon => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM172RafaelaCinnamon Linux Mint 17.2 Rafaela Mint17Cinnamon => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM172RafaelaMate Linux Mint 17.2 Rafaela Mint17Mate => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM173RosaMate Linux Mint 17.3 Rosa Mint17Mate => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM173RosaCinnamon Linux Mint 17.3 Rosa Mint17Cinnamon => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoXenialTrinity Ubuntu-Xenial Ubuntu1604Trinity
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoXenialTrinity Ubuntu-Xenial Ubuntu1604Trinity => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoXenialGnome Ubuntu-Xenial UbuntuGnome1604
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoXenialGnome Ubuntu-Xenial UbuntuGnome1604 => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM18SarahKDE LinuxMint 18 Sarah Mint18KDE => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM18SarahXfce LinuxMint 18 Sarah Mint18Xfce => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM181SerenaCinnamon LinuxMint 18.1 Serena Mint18Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM181SerenaCinnamon LinuxMint 18.1 Serena Mint18Cinnamon => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM181SerenaKDE LinuxMint 18.1 Serena Mint18KDE => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisostretchdebMateFull stretch Debian8MateFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisostretchdebMateFull stretch Debian8MateFull => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisostretchdebLxdeFull stretch Debian8LxdeFull
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisostretchdebLxdeFull stretch Debian8LxdeFull => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM183SylviaMate LinuxMint 18.3 Sylvia Mint18Mate => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM183SylviaKDE LinuxMint 18.3 Sylvia Mint18KDE => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoBionicBudgieMinimal Ubuntu-Bionic UbuntuBudgieMinimal1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoBionicBudgieMinimal Ubuntu-Bionic UbuntuBudgieMinimal1804 => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoBionicGnome Ubuntu-Bionic UbuntuGnome1804
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoBionicGnome Ubuntu-Bionic UbuntuGnome1804 => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM19TaraCinnamon LinuxMint 19 Tara Mint19Cinnamon => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM19TaraXfce LinuxMint 19 Tara Mint19Xfce => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM191TessaMate LinuxMint 19.1 Tessa Mint19Mate => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce
			ret=$?

			echo -n "AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTisoLM191TessaXfce LinuxMint 19.1 Tessa Mint19Xfce => " >> aTS-atISOm23Server.log

			if [ $ret -ne 0 ]
			then
				echo FAIL
				
			else
				echo OK
			fi
			