#!/bin/bash

for baseFile in $(grep input /mdk/doc/manual/tex/de/not-generated/manual.tex | cut -d '{' -f2 | grep 'hlp.tex' | xargs -n 1 basename | sed 's/\.hlp\.tex.*//')
do
	for lang in de en fr
	do
		png="/mdk/doc/manual/screenshots/$lang/$baseFile.png"
		if [ ! -f "$png" ]
		then
			echo "MISSING: $png"
# 		else
# 			hlp="/m23/inc/help/$lang/$baseFile.hlp"
# 
# 			hlpTime=$(find "$hlp" -printf "%T@\n" | cut -d'.' -f1)
# 			pngTime=$(find "$png" -printf "%T@\n" | cut -d'.' -f1)
# 			
# 			echo $[ $hlpTime - $pngTime ]
		fi
	done
	
	
done