#!/bin/bash

find /m23/inc/help -type f -printf "%f\n" | grep -v "~$" |sort -u > /tmp/i18nAllFiles







#get all directories with
for i18nfile in `cat /tmp/i18nAllFiles`
do
	changed=0

	for dir in `find /m23/inc/help/* -type d`
	do
		curI18Nfile="$dir/$i18nfile"
		lang=`basename $dir`

		if [ -e "$curI18Nfile" ]
		then
			sed 's/$[A-Za-z_18]*/\n&\n/g' < "$curI18Nfile" | grep I18N | sort -u > /tmp/$lang.i18ns
		else
			echo "missing: $curI18Nfile"
			changed=1
		fi
	done

	sort -u /tmp/*.i18ns > /tmp/all.i18ns

	for i18NsymbolFile in `ls /tmp/??.i18ns`
	do
		diff /tmp/all.i18ns $i18NsymbolFile | grep "<" | cut -d' ' -f2 > /tmp/missing.i18ns

		lang=`basename $i18NsymbolFile | cut -d'.' -f1`

		if [ `wc -l /tmp/missing.i18ns | cut -d' ' -f1` -gt 0 ]
		then
			echo -n "$lang/$i18nfile missing: "
			awk -v ORS=' ' '{print}' < /tmp/missing.i18ns
			echo "
	cmd: kate -u /m23/inc/help/$lang/$i18nfile"
			changed=1
		fi
		rm /tmp/missing.i18ns
	done
	
	rm /tmp/*.i18ns
	if [ $changed -gt 0 ]
	then
		echo
	fi
done

rm /tmp/i18nAllFiles





#get all directories with
# for dir in `find /m23/inc/help/* -type d`
# do
# 	find $dir -type f -printf "%f\n" | grep -v "~$" |sort -u > /tmp/i18nFiles
# 	for f in `diff /tmp/i18nAllFiles /tmp/i18nFiles | grep "<" | cut -d' ' -f2`
# 	do
# 		echo "$dir: $f"
# 	done
# done
# 
# rm /tmp/i18nAllFiles


exit

rm /tmp/*.m23index 2> /dev/null

for file in `cat /tmp/i18nAllFiles`
do
	for dir in `find /m23/inc/help/* -type d`
	do
		bn=`basename $dir`

		if [ -f "$dir/$file" ]
		then
			sed 's/<!--/\n<!--/g' "$dir/$file" | sed 's/-->/-->\n/g' | grep "<\!--" | awk -v file=$file '{print(file":"$0)}' >> /tmp/$bn.m23index
			#echo "$file: $md5" >> /tmp/$bn.m23index
		else
			echo "$file: missing" >> /tmp/$bn.m23index
		fi
	done
done

rm /tmp/i18nAllFiles

exit

