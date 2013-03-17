#get version number & codename from the php file
m23codename=`cat /m23/inc/version.php | grep codename | cut -d"\"" -f2`
m23version=`cat /m23/inc/version.php | grep version | cut -d"\"" -f2`

#save it to the version.tex
echo "m23 $m23codename $m23version" > /mdk/doc/manual/tex/version.tex


for LANG in `cat /tmp/m23language`
do
	echo "========================================"
	echo "generating Tex files for language: $LANG"
	echo "========================================"

	mkdir -p /mdk/doc/manual/tex/$LANG
	cd /mdk/doc/manual/tex/$LANG
	
	#delete backup files
	rm `find /m23/inc/help/$LANG | grep '~'` 2> /dev/null

	for file in `find /m23/inc/help/$LANG/ -type f -printf "%f\n" | grep ".hlp$" | sort`
	do
		pure=`echo $file | sed 's/[.]hlp//g'`
		scale=`grep "$pure:" /mdk/doc/manual/screenshots/scalingTable | cut -d' ' -f2`
		/mdk/doc/manual/bin/html2tex.sh /m23/inc/help/$LANG/$file /mdk/doc/manual/screenshots/$LANG/$pure.png $file.tex $scale
	done
done
