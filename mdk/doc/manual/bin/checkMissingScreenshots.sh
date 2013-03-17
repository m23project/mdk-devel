#!/bin/bash

for scrFile in `find /m23/inc/help -type f -printf "%f\n" | grep -v "~$" | grep "hlp$" | sort -u| sed 's/\.hlp//g'`
do
	if test `grep "$scrFile.png" /mdk/doc/manual/bin/makeScreenshots.sh | wc -l` -eq 0
	then
		echo "../../bin/kh2p \"page=$scrFile\" $scrFile.png"
	fi
done