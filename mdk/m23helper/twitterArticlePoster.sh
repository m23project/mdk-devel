#!/bin/sh

if [ $# -lt 1 ]
then
	echo "
Usage: $0 <article dir>

Help: `basename $0` will post all articles in the <article dir> that are
      newer than the newest article in the previous run to Twitter. The first
      line of the article is the heading and will be the message posted to
      Twitter."
	exit 1
fi

articleDir=$1

#Date of the article with the latest changing date that was posted on last run
last=`cat "$articleDir/lastUpdate.txt"`
newlast=0

find "$articleDir" -type f -printf "%p\n" | grep -v '~' | grep html | sort -n | while read name
do
	id=`basename $name | cut -d'#' -f1`


	#Get the heading and post it as Twitter message
	if [ $last -lt $id ]
	then
	#Update the date of the newest file
	if [ $id -gt $newlast ]
		then
			echo $id > "$articleDir/lastUpdate.txt"
			newlast=$id
		fi

		msg=`head -1 $name`
		/mdk/m23helper/twitter/twitterFaceBook-Message.sh "$msg" m23news "#m23" "http://m23.sourceforge.net/PostNuke-0.750/html/index.php?id=$id"
	fi
done