#!/bin/bash

client="$1"
sourceslist="$2"
desktop="$3"

./autoTest.php 1deleteClient.m23test "$client"
./autoTest.php 1m23client-distro-install.m23test "$client" "$sourceslist" "$desktop"


# ./autoTest.php 1deleteClient.m23test autoTestLM19Mate
# ./autoTest.php 1m23client-distro-install.m23test autoTestLM19Mate 'LinuxMint 19 Tara' Mint19Mate

# ./autoTest.php 1deleteClient.m23test autoTestLM191Mate
# ./autoTest.php 1m23client-distro-install.m23test autoTestLM191Mate 'LinuxMint 19.1 Tessa' Mint19Mate

 ./autoTest.php 1m23server-iso-install.m23test autoTestServer /crypto/iso/m23server_19.1_rock-devel.iso 192.168.1.24

# TEST_M23_BASE_URL='https://god:m23@192.168.1.23/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTm23sATstretchKDE stretch Debian8KdeFull

./autoTest.php 1m23client-distro-install.m23test autoTestLM191Xfce 'LinuxMint 19.1 Tessa' Mint19Xfce
./autoTest.php 1m23client-distro-install.m23test autoTestLM191Mate 'LinuxMint 19.1 Tessa' Mint19Mate


./autoTest.php 1m23server-auf-debian-installieren.m23test Debian9-amd64 192.168.1.96
./autoTest.php 1m23server-auf-debian-installieren.m23test Debian9-i386 192.168.1.93
AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.93/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTm23sATstretchKDE stretch Debian8KdeFull


AT_debug=1 ./autoTest.php 1m23server-auf-UCS-installieren.m23test 'UCS 4.4 - Lokales Testrepo' 192.168.1.144
