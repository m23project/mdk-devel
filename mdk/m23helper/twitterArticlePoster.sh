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

find "$articleDir" -type f -printf "%p*%CY%Cm%Cd%CH%CM\n" | grep -v '~' | grep html | while read file
do
	name=`echo $file | cut -d'*' -f1`
	date=`echo $file | cut -d'*' -f2`
	id=`basename $name | cut -d'#' -f1`

	#Update the date of the newest file
	if [ $date -gt $newlast ]
	then
		echo $date > "$articleDir/lastUpdate.txt"
		newlast=$date
	fi

	#Get the heading and post it as Twitter message
	if [ $last -lt $date ]
	then
		msg=`head -1 $name`
		/mdk/m23helper/twitter/twitterFaceBook-Message.sh "$msg" m23news "#m23" "http://m23.sourceforge.net/PostNuke-0.750/html/index.php?id=$id"
	fi
done