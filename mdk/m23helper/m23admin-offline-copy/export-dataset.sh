#!/bin/bash

if [ $(whoami) != 'root' ]
then
	echo "ERROR: Must be run as root!"
	exit 1
fi

# Creates an SQL dump for generating the offline demo from
p=`/m23/bin/getMySQL-PasswordParam.sh`
u=`/m23/bin/getMySQL-UserParam.sh`

# Drop commands for some unwanted entries
echo 'TRUNCATE `clientjobs`;
TRUNCATE `clients`;
TRUNCATE `clientpackages`;
DROP TRIGGER IF EXISTS `trg_sum_clientpackages_add`;
DROP TRIGGER IF EXISTS `trg_sum_clientpackages_del`;
ALTER TABLE `clients` DROP `trg_sum_clientpackages`;
' > all.sql

# Dump data for clients and fix (now) incorrect values
mysqldump --extended-insert=FALSE $u $p m23 | grep "\(uefi64\|ueficl0\|defuefi\|m23-boot-iso-usbnet\)" | sed '/^INSERT INTO `clientjobs`/s/NULL,0);/"",0);/g' >> all.sql

# Compress the dump
rm all.sql.gz
gzip all.sql

# Upload the dump to the VM
scp-ohneSpeichern all.sql.gz root@192.168.1.23:/mdk/m23helper/m23admin-offline-copy

rm all.sql.gz