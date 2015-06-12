#!/bin/bash

#get username and password of the debian MySQL system
dbadmin=`grep "^user" /etc/mysql/debian.cnf | tr -d '[:blank:]' | cut -d'=' -f2 | head -1`
dbadmpass=`grep "^password" /etc/mysql/debian.cnf | tr -d '[:blank:]' | cut -d'=' -f2 | head -1`

mysql -u$dbadmin -p$dbadmpass m23 -e "delete from clients"

for sql in `ls *.sql.gz`
do
	gunzip -c $sql | mysql -u$dbadmin -p$dbadmpass --force m23
done

find `pwd` | grep 'tar\.gz' | while read tarF
do
	cd /m23
	tar xfvz $tarF
done