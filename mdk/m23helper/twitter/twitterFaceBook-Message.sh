#!/bin/sh

msg=$1
category=$2
ttag=$3
link=$4

if [ $# -lt 3 ]
then
	echo "Posts a message to Twitter and FaceBook and shortens the Twitter message if needed"
	echo "usage $0 <message> <category> <Twitter tag (e.g. #m23)> <link>"
	exit 1
fi

# Convert HTML characters into "normal" characters
msg=$(php -r "print(utf8_decode(html_entity_decode(\"$msg\")));")

#Shorten the URL
# slink=`wget "http://bit.ly/?url=$link&submit=Shorten" -O - | grep tweet_body | grep  bit.ly | cut -d'>' -f2 | cut -d'<' -f1`
# slink=`wget http://is.gd/create.php --post-data="URL=$link" -O - | sed 's/>/\n/g' | grep shortened | grep is.gd | cut -d'"' -f2`
slink=`wget http://is.gd/create.php --post-data="url=$link" -O - | sed 's/>/\n/g' | grep short_url | cut -d'"' -f8`

#Create the Twitter message
tmsg="$category: $msg: $slink $ttag"

#Get its length
tmsglen=`echo -n $tmsg | wc -m`

#Check if the message is too long
if [ $tmsglen -gt 140 ]
then
	#Calculate how many chars are too much in the complete message
	tooLong=`expr $tmsglen - 137`

	#Length of the message itself
	msglen=`echo -n $msg | wc -m`

	#Calculate the new length of the mesage
	newMsgLen=`expr $msglen - $tooLong`

	#Shorten (... will be added)
	tmpmsg="`echo $msg | dd bs=$newMsgLen count=1 2>/dev/null`..."
	tmsg="$category: $tmpmsg: $slink $ttag"
fi

echo ">> $msg"
echo ">> Twittering"

twitter set "$tmsg"

#fbcmd POST "$category: $msg
#$link"
