#!/bin/bash

/m23/bin/m23cli.php DB_changeAllCollations

#`ls *.sql.gz`

for sql in all.sql.gz
do
	gunzip -c $sql | mysql -uroot --force m23
done

#find `pwd` | grep 'tar\.gz' | while read tarF

for tarF in data.tar.gz
do
	cd /m23
	tar xfvz $tarF
done