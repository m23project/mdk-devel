#!/bin/sh

#Calculates the total amount of MSR_statusBarIncCommand points per file for all PHP files in the current directory

ls *.php | while read phpFile
do
echo -n "$phpFile: "
grep MSR_statusBarIncCommand "$phpFile" | sed 's/.*(//g' | sed 's/).*//g' | awk 'BEGIN {
	cnt=0
}

{
	cnt=cnt+$0
}

END {
	print(cnt)
}'
done