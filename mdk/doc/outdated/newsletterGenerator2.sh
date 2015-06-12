#!/bin/sh


tmpScr="/tmp/articleMenuTmp"
articleList="/tmp/articleList"
articleDir="/matrix23/arbeit/wwwTests/cms/data/articles/"


getNewsletterTitle()
{
	date +"m23-Newsletter %d.%m.%Y / %F"
}


#get the text body of the news
getText()
{
	awk 'BEGIN {
		found=0;
	}
	
	{
		if (index($0, "      </td>") > 0)
			found=0;

		if (found == 1)
			print($0);

		if (index($0, "<td width=\"100%\" valign=\"top\">") > 0)
			found=1;
	}
	' /tmp/pnNews.html
}


getASCII()
{
	#1. omit the first line of the file (it's the heading)
	#2. filter out all 'target="_blank"'
	#3. replace all HTML symbols by text equivalents
	awk '
	BEGIN {first=1;}

	{
		if (first==1)
			first=0;
		else
			print($0);
	}' $1 |
	sed 's# target="_blank"##g' | awk 'BEGIN {
		listType="";
	}
	
	{
		if (match($0, "(<ol>|<OL>)"))
			{
				listType="o";
				listCounter=1;
			}

		if (match($0, "(<ul>|<UL>)"))
			{
				listType="u";
			}


		if (listType != "")
		{
			if (match($0,"(<li>|<LI>)") > 0)
			{
				if (listType == o)
				{
					gsub("(<li>|<LI>)", " "listCounter". ");
					listCounter++;
				}
				else
					gsub("(<li>|<LI>)", " * ");
			}
		}

		gsub("(<i>|<b>|</i>|</b>|<li>|<LI>|<ul>|<UL>|<p>|<ol>|<OL>|</li>|</LI>|</ul>|</UL>|</p>|</ol>|</OL>|<h[1-4]>|<H[1-4]>|</h[1-4]>|</H[1-4]>|<br />)","");
		gsub("^\t[ ]*"," ");
		gsub("&quot;","\"");

		if (match($0, "<a href=\"") > 0)
		{
			count = split($0,arr, "(<a href=\"|</a>|\">)");

			ORSBack=ORS;
			ORS="";
			for (i=0; i < count; i+=3)
			{
				/*print("\n>>>cnt: "count" i"i"\n");*/
				if ((count - i) >= 3)
					print(arr[i+1]""arr[i+3]" ("arr[i+2]")");
				else
					{
						print(arr[i+1]);
					}
			}
			ORS=ORSBack;
			print("\n");
		}
		else
			print($0);
	}'
	
}

#underlines a string
underLine()
{
	echo $1 | awk -v ORS='' '{
		print($0);
		print("\n");

		for (i=0; i < length($0); i++)
			print("=");

		print("\n");
	}'
}


getNewsletterTitle()
{
	date +"m23-Newsletter %d.%m.%Y / %F"
}


printContents()
{
	count=$1
	deArr=$2
	enArr=$3
	
	echo "Inhalt / Contents (english version in the middle)
"
	echo $deArr
	for nr in `seq 1 $count`
	do
		de=${deArr[${nr}]}
		en=${enArr[${nr}]}
		echo "*$nr $de / $en"
	done
}


getAll()
{
nr=1
rm /tmp/pnNewsde.txt /tmp/pnNewsen.txt /tmp/pnHeadings.txt 2> /dev/null

for newsfile in `cat /tmp/articleList`
do
	newsfile=`echo $newsfile | sed "s#'##g"`
	heading=`head -1 $articleDir$newsfile`
	lang=`echo $newsfile | cut -d'#' -f2`

echo "
`underLine \"${heading}\"`

`getASCII $articleDir$newsfile`

" >> /tmp/pnNews$lang.txt

	let nr=nr+1

	if [ $lang = "en" ]
	then
		echo -n " * $heading / " >> /tmp/pnHeadings.txt
	else
		echo "$heading" >> /tmp/pnHeadings.txt
	fi
done
}

echo '#!/bin/sh

dialog --single-quoted --clear --title "News files" --checklist "Please select the kernel directory you want to use" 13 70 5 \' > $tmpScr
# >> /tmp/$tmpScr

ls $articleDir | sort -n -r | grep -v '~$' | head -n10 | awk '{print("\""$0"\" \"\" \"off\" \\")}' >> $tmpScr

echo ' 2> '$articleList'
retval=$?
case $retval in
        1)
                exit;;
        255)
                exit;;
esac

' >> $tmpScr

sh $tmpScr
rm $tmpScr

getAll
getNewsletterTitle > /tmp/pnNewsletter.txt
echo "

Contents (english version in the middle) / Inhalt
" >> /tmp/pnNewsletter.txt
cat /tmp/pnHeadings.txt >> /tmp/pnNewsletter.txt
echo "
" >> /tmp/pnNewsletter.txt

cat /tmp/pnNewsde.txt >> /tmp/pnNewsletter.txt

echo "


===> english version


" >> /tmp/pnNewsletter.txt
cat /tmp/pnNewsen.txt >> /tmp/pnNewsletter.txt

echo "The newsletter was stored in /tmp/pnNewsletter.txt"