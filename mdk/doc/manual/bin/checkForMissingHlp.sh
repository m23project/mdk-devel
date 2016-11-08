#!/bin/bash

. /mdk/bin/forkFunctions.inc

isDevelReturn
if [ $? -ne 0 ]
then
	echo "ERROR: Can only be run, if the development version is active"
	exit 23
fi


# Create a file with all .hlp and .inc files from /m23/inc/help
find /m23/inc/help -type f -printf "%f\n" | grep -v "~$" | grep -v bafh.hlp | grep -v customerCenter.hlp | grep -v customerRegistration.hlp | grep -v m23SharedBootingYourClient.hlp | sort -u > /tmp/helpAllFiles


# Log file to store the output messages
logFile="/tmp/allMissingChangedm23Help.txt"
rm $logFile 2> /dev/null

# File to store the file names of changed and missing .hlp and .inc files
tarList="/tmp/allMissingChangedm23HelpFiles.txt"
rm $tarList 2> /dev/null







# Find .hlp/.inc files that have too less I18N_ symbols
for helpFile in `cat /tmp/helpAllFiles`
do
	changed=0

	# Language directories (e.g. /m23/inc/help/de)
	for dir in `find /m23/inc/help/* -type d`
	do
		curhelpFile="$dir/$helpFile"

		# 2 char language (e.g. de)
		lang=`basename $dir`

		if [ -e "$curhelpFile" ]
		then
			sed 's/$[A-Za-z_18]*/\n&\n/g' < "$curhelpFile" | grep I18N | sort -u > /tmp/$lang.i18ns
		else
			echo "missing: $curhelpFile" | tee -a $logFile
			changed=1
		fi
	done

	# Get a list of all I18N_ symbols in all languages
	sort -u /tmp/*.i18ns > /tmp/all.i18ns

	# Check for missing I18N_ symbols in the .hlp/.inc files for a given language
	for i18NsymbolFile in `ls /tmp/??.i18ns`
	do
		diff /tmp/all.i18ns $i18NsymbolFile | grep "<" | cut -d' ' -f2 > /tmp/missing.i18ns

		lang=`basename $i18NsymbolFile | cut -d'.' -f1`

		if [ `wc -l /tmp/missing.i18ns | cut -d' ' -f1` -gt 0 ]
		then
			echo -n "$lang/$helpFile missing: "
			awk -v ORS=' ' '{print}' < /tmp/missing.i18ns
			echo "
	cmd: kate -u /m23/inc/help/$lang/$helpFile" | tee -a $logFile
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





devel=$(getm23DevelDir)
if [ $? -ne 0 ]
then
	echo "ERROR: m23 development directory could not be found!"
	exit 1
fi

release=$(getm23ReleaseDir)
if [ $? -ne 0 ]
then
	echo "ERROR: m23 release directory could not be found!"
	exit 1
fi


# Check for changes between the German help files in the m23 development and release directories
LC_ALL=C; diff -q -r $devel/inc/help/de/ $release/inc/help/de/ | grep -v '~' | grep differ$ | cut -d' ' -f2 | while read changedInGerman
do
	for lang in de en fr
	do
		echo -n "Changed: "
		basename $changedInGerman | awk -v dir="$devel/inc/help/$lang" '{print(dir"/"$0)}' | tee -a $logFile
		echo -n "Old version: "
		basename $changedInGerman | awk -v dir="$release/inc/help/$lang" '{print(dir"/"$0)}' | tee -a $logFile
	done
done


# # Get a list of comments for each language
# for file in `cat /tmp/helpAllFiles`
# do
# 	# Language directories (e.g. /m23/inc/help/de)
# 	for dir in `find /m23/inc/help/* -type d`
# 	do
# 		# 2 char language (e.g. de)
# 		lang=`basename $dir`
# 
# 		if [ -f "$dir/$file" ]
# 		then
# 			# Pull out the comments
# 			sed 's/<!--/\n<!--/g' "$dir/$file" | sed 's/-->/-->\n/g' | grep "<\!--" | awk -v file=$file '{print(file":"$0)}' >> /tmp/$lang.m23Comments
# 		else
# 			echo "$file: missing" >> /tmp/$lang.m23Comments | tee -a $logFile
# 		fi
# 	done
# done
# 
# 
# 
# 
# # Check for missing comments
# for lang in en fr
# do
# 	diffFile="/tmp/$lang.m23commentsMissing"
# 
# 	# Search comments that are missing in comparison to the German version
# 	diff /tmp/$lang.m23Comments /tmp/de.m23Comments  | grep "> " | cut -d' ' -f2 | cut -d':' -f1 | sort -u > $diffFile
# 
# 	# Show the files with missing comments
# 	if [ $(wc -l $diffFile | cut -d' ' -f1) -gt 0 ]
# 	then
# 		echo "Comments missing in:" | tee -a $logFile
# 		awk -v dir="/m23/inc/help/$lang" '{print(dir"/"$0)}' $diffFile | tee -a $logFile
# 	fi
# done





# Create a file that stores the file names of changed and missing .hlp and .inc files
cat $logFile | sed 's/ /\n/g' | while read file
do
	if [ -f "$file" ]
	then
		echo $file >> "$tarList.tmp"
		echo $file | sed 's#/\(fr\|en\)/#/de/#' >> "$tarList.tmp"
	fi
done

echo $logFile > $tarList
sort -u "$tarList.tmp" >> $tarList
rm "$tarList.tmp"

tar cfz /tmp/allMissingChangedm23HelpFiles.tar.gz -T /tmp/allMissingChangedm23HelpFiles.txt
echo "/tmp/allMissingChangedm23HelpFiles.tar.gz contains all files needed for translation"


#rm /tmp/helpAllFiles /tmp/??.m23Comments /tmp/??.m23commentsMissing /tmp/??.m23index /tmp/??.i18n 2> /dev/null