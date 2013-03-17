#!/bin/sh
dbuser=`grep "^user" /etc/mysql/debian.cnf | tr -d [:blank:] | cut -d'=' -f2`
dbpass=`grep "^password" /etc/mysql/debian.cnf | tr -d [:blank:] | cut -d'=' -f2`

echo "CREATE DATABASE m23;" | mysql -u$dbuser -p$dbpass
bzcat m23.sql.bz2 | mysql -u$dbuser -p$dbpass m23

echo "CREATE DATABASE m23captured;" | mysql -u$dbuser -p$dbpass
bzcat m23captured.sql.bz2 | mysql -u$dbuser -p$dbpass m23captured

