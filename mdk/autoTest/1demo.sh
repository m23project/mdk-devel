#!/bin/bash
./autoTest.php 1m23server-iso-install.m23test 'atISOm23Server' /crypto/iso/m23server_19.1_rock-devel.iso 192.168.1.24

AT_M23_SSH_PASSWORD='test' TEST_M23_BASE_URL='https://god:m23@192.168.1.24/m23admin' ./autoTest.php '1m23client-distro-install.m23test' 'aTisoLM191TessaMate-amd64' 'LinuxMint 19.1 Tessa' 'Mint19Mate'
