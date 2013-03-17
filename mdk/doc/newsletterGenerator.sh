#!/bin/sh

#get the HTML file of a news
getNews()
{
	echo -n "Downloading $1..." > /dev/stderr
	wget -q "http://m23.sourceforge.net/PostNuke-0.750/html/index.php?name=News&file=article&sid=$1" -O /tmp/pnNews.html
	echo " done" > /dev/stderr
}


#get the heading of the news
getHeading()
{
	grep "index.php?name=News" /tmp/pnNews.html | grep "</a></b></td>" | cut -d'>' -f2 | cut -d'<' -f1
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
	getText | awk 'BEGIN {
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

		gsub("(<li>|<LI>|<ul>|<UL>|<p>|<ol>|<OL>|</li>|</LI>|</ul>|</UL>|</p>|</ol>|</OL>|<h[1-4]>|<H[1-4]>|</h[1-4]>|</H[1-4]>|<br />)","");
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
lang="de"

rm /tmp/pnNewsde.txt /tmp/pnNewsen.txt /tmp/pnHeadings.txt 2> /dev/null

for newsnr in `cat /tmp/pnNewsList.txt`
do
	getNews $newsnr
	heading=`getHeading`
	
echo "
`underLine \"${heading}\"`

`getASCII`

" >> /tmp/pnNews$lang.txt

	let nr=nr+1

	if [ $lang = "de" ]
	then
		echo -n " * $heading / " >> /tmp/pnHeadings.txt
		lang="en"
	else
		echo "$heading" >> /tmp/pnHeadings.txt
		lang="de"
	fi
done
}


echo "Please enter the numbers of the news that should be placed into the newsletter!
German an English news rotational. E.g.:

1000000204
1000000205
...

Press hit Return to enter the numbers.
"

read maha
nano /tmp/pnNewsList.txt


getAll
getNewsletterTitle > /tmp/pnNewsletter.txt
echo "

Inhalt / Contents (english version in the middle)
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