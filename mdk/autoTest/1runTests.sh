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

# ./autoTest.php 1m23server-iso-install.m23test autoTestServer /crypto/iso/m23server_19.1_rock-devel.iso
# TEST_M23_BASE_URL='https://god:m23@192.168.1.23/m23admin' ./autoTest.php 1m23client-distro-install.m23test aTm23sATstretchKDE stretch Debian8KdeFull