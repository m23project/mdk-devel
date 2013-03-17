#!/bin/bash

for LANG in `cat /tmp/m23language`
do
	cd /mdk/doc/manual/tex/$LANG/
	echo "Language: $LANG"

	for f in `ls *.tex`
	do
		awk 'BEGIN {
		beginc=0;
		endc=0;
		}
		
		{
			beginc+=gsub(/\\begin/, "begin", $0);
			endc+=gsub(/\\end/, "end", $0);
		}
		
		END {
			if (beginc != endc)
				print(FILENAME" begin:"beginc" end: "endc"\n");
		};' $f
	done
done