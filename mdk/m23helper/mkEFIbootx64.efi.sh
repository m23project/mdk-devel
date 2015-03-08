#!/bin/bash

tempDir='/tmp/efi'
grubCfg="$tempDir/boot/grub/grub.cfg"

# Create the temp directory
mkdir -p $tempDir/boot/grub
cd $tempDir

# Write the grub.cfg
rm $grubCfg 2> /dev/null
cat >> $grubCfg << "GrubCfgEOF"
set timeout=5

menuentry 'm23 PXE EFI boot' --class os {
	insmod net
	insmod efinet
	insmod tftp
	insmod gzio
	insmod part_gpt
	insmod efi_gop
	insmod efi_uga

	echo 'Network status: '
	net_ls_cards
	net_ls_addr
	net_ls_routes

	echo 'Loading Linux ...'
	linux (tftp)/m23pxeinstall-amd64

	echo 'Loading initial ramdisk ...'
	initrd (tftp)/initrd-amd64.gz
}
GrubCfgEOF

# Build the image
grub-mkstandalone --compress=xz -d /usr/lib/grub/x86_64-efi/ -O x86_64-efi --fonts="unicode" -o bootx64.efi boot/grub/grub.cfg