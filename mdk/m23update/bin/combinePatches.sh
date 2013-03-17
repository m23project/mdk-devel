#!/bin/bash

if test $# -lt 3
then
	echo "Merges patches to a new patch. List the patches by their numbers in descending order."
	echo "usage $0 <number of the merged patch> <patch 1> <patch 2> [more patches]"
	exit
fi


rm -r /tmp/m23Combine
mkdir -p /tmp/m23Combine/tmp
cd /tmp/m23Combine/tmp

#extract all patches, combine all removeFiles.sh, start and end scripts and info files
for name in $2 $3 $4 $5 $6 $7 $8 $9
do
echo "Extracting patch $name"

tarName=`find /mdk/m23update/updates/ -name $name.tb2`
tar xfjp $tarName --same-owner

for ext in start end info
do
	fname=`find /mdk/m23update/updates/ -name $name.$ext`
	
	if test $fname
	then
		cat $fname >> ../$1.$ext
	fi
done

cat tmp/removeFiles.sh >>  tmp/removeFiles.all
done

#make lines in removeFiles unique and sort out BASH marking lines
sort -u tmp/removeFiles.all | grep -v "#/bin/sh" | grep -v "#!/bin/bash" > removeFiles.sort
rm tmp/removeFiles.all tmp/removeFiles.sh

#make a new removeFiles.sh that only contains files that are not in the patch archives
echo "#!/bin/bash" > tmp/removeFiles.sh
cat removeFiles.sort | while read delfile
do
if test -f `echo "$delfile" | sed 's/rm \//.\//'`
then
	true
else
	echo "$delfile" >> tmp/removeFiles.sh
fi
done
rm removeFiles.sort

#adjust the patch number in version.php
patchNrLine=`grep m23_patchLevel ./m23/inc/version.php`
cat ./m23/inc/version.php | sed "s/$patchNrLine/\$m23_patchLevel=\"$1\";/" > /tmp/version.php
userGroup=`find ./m23/inc/version.php -printf 'chown %u.%g /m23/inc/version.php'`
perm=`find ./m23/inc/version.php -printf 'chmod %m /m23/inc/version.php'`
mv /tmp/version.php ./m23/inc/version.php
$userGroup
$perm


echo "Compressing new patch..."
tar cjp --same-owner -f ../$1.tb2 *

find ../$1.tb2 -printf "%s\n" > "../$1.size"

cd ..
rm -r tmp

echo "The new patch files have been stored under /tmp/m23Combine"