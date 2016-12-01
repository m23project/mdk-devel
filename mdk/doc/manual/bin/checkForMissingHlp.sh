#!/bin/bash

. /mdk/bin/forkFunctions.inc

isDevelReturn
if [ $? -ne 0 ]
then
	echo "ERROR: Can only be run, if the development version is active"
	exit 23
fi



# Temporary directory to store the old and new files
tmp="/tmp/missingHelp"
rm -r "$tmp" 2> /dev/null



# The associative arrays $d and $r contain the full path to the help files directories in development and release version
declare -A d
declare -A r
declare -A di
declare -A ri



# Create the output directories and fill the arrays
for lang in de en fr
do
	d[$lang]="$(getm23DevelDir)/inc/help/$lang"
	di[$lang]="$(getm23DevelDir)/inc/i18n/$lang"
	r[$lang]="$(getm23ReleaseDir)/inc/help/$lang"
	ri[$lang]="$(getm23ReleaseDir)/inc/i18n/$lang"
	mkdir -p "$tmp/old/help/$lang" "$tmp/new/help/$lang"
	mkdir -p "$tmp/old/i18n/$lang" "$tmp/new/i18n/$lang"
done



# Get the German help files that differ from release to development version
diff -r -q ${d[de]} ${r[de]} | grep -v '~' | sed -e 's°: °/°' -e 's/\.$//' | xargs -n1 basename | sort -u | while read name
do
	if [ -f ${d[de]}/$name ]
	then
		for lang in de en fr
		do
			cp -v "${d[$lang]}/$name" "$tmp/new/help/$lang"
			cp -v "${r[$lang]}/$name" "$tmp/old/help/$lang"
		done
	fi
done



# Copy m23inst.php and m23base.php if they differ (or are missing) from release to development version in the German version
for instBase in m23inst.php m23base.php
do
	diff -q "${di[de]}/$instBase" "${ri[de]}/$instBase" > /dev/null
	if [ $? -ne 0 ]
	then
		for lang in de en fr
		do
			cp -v "${di[$lang]}/$instBase" "$tmp/new/i18n/$lang"
			cp -v "${ri[$lang]}/$instBase" "$tmp/old/i18n/$lang"
		done
	fi
done



# Write a script for starting meld
echo "#!/bin/bash
meld old new" > $tmp/run-meld
chmod +x $tmp/run-meld



# Make an archive and delete the temp files
cd "$tmp"
tar cfvz ../m23-i18n-help-translate.tar.gz .
cd ..
rm -r "$tmp"

echo "Translation files stored in /tmp/m23-i18n-help-translate.tar.gz"