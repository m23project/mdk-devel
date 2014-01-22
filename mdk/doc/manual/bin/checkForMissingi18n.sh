#!/bin/bash

for type in m23base.php m23inst.php
do
	#Get the symbols of all languages
	find /m23/inc/i18n | grep $type$ | xargs cat | cut -d' ' -f1 | grep '^\$' | cut -d'=' -f1 | sort -u > /tmp/all.i18n


	#Process the languages sepreately
	for lang in $(ls /m23/inc/i18n)
	do
		cat /m23/inc/i18n/$lang/$type | cut -d' ' -f1 | grep '^\$' | cut -d'=' -f1 | sort -u > /tmp/$lang.i18n
		echo "$lang/$type"
		#Get the missing symbols
		diff /tmp/$lang.i18n /tmp/all.i18n | grep '>'
	done
done