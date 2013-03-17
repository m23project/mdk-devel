#!/bin/bash

if test $# -lt 1
then
echo "usage:  mdoc <start directory> <tex output file>
	start directory: directory to start search for files
	tex output file: filename the latex output file should be saved to";
fi

rm "$2" 2> /dev/null

grep '/*$mdocInfo' `find $1 2> /dev/null | grep php$` -r -l 2> /dev/null | grep -v "~$" | sort | while read f
do
	echo "Generating LaTeX from \"$f\""
	gawk -f /mdk/doc/devguide/bin/mdoc.awk "$f" >> $2
done