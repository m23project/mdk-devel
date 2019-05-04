#!/bin/bash

export DEBIAN_FRONTEND=noninteractive
export LC_ALL=C

#/etc/init.d/slapd stop
#rm -r /var/lib/ldap

rm /etc/init.d/mysql

apt-get update

echo "
slapd slapd/internal/generated_adminpw password test
slapd slapd/password1 password test
slapd slapd/password2 password test
slapd slapd/internal/adminpw password test

m23-ldap m23-ldap/LDAPpass string test
m23-ldap m23-ldap/LDAPhint note
m23 m23/configDHCP boolean true
m23 m23/configMySQL boolean true
m23 m23/configureApache boolean true
m23 m23/configureBackupPC boolean true
m23 m23/configureSquid boolean true
m23 m23/configureSSH boolean true
m23 m23/configureSSL boolean true
m23 m23/configureSudo boolean true
m23 m23/experimentalWarn note
m23 m23/warnDHCP note
m23-tftp m23-tftp/configureTFTP boolean true
" > /tmp/debconf-setting

debconf-set-selections /tmp/debconf-setting

dpkg-reconfigure slapd

apt-get install m23 --allow-unauthenticated -y

sed -i '/afterRebootSetup/d' /etc/rc.local