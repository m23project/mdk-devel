#!/bin/bash

for manualTex in `find /mdk/doc/manual/tex/ -mindepth 3 -maxdepth 3 -type f -printf "%h/%f\n" | grep "manual.tex$"`
do
		echo "======================$manualTex======================"
	for allTex in `find /mdk/doc/manual/tex/ -maxdepth 2 -type f -printf "%f\n" | sort -u | grep "tex$" | grep -v ".inc."`
	do
		if test `grep "$allTex" "$manualTex" | wc -l` -eq 0
		then
			echo "\input{../$allTex}"
		fi
	done
done