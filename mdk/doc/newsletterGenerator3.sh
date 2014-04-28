#!/bin/sh

#fixed order from now on: de / en / fr

tmpScr="/tmp/articleMenuTmp"
articleList="/tmp/articleList"
articleDir="/matrix23/arbeit/wwwTests/cms/data/articles/"

#returns a string with date formatted in German, English and French (readyforfrench)
getNewsletterTitle()
{
	date +"m23-Newsletter %d.%m.%Y / %D / %d/%m/%Y"
}



#needs replacement for French html-signs (&eacute;....)
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

		gsub("(<i>|<b>|</i>|</b>|<li>|<LI>|<ul>|<UL>|<p>|<ol>|<OL>|</li>|</LI>|</ul>|</UL>|</p>|</ol>|</OL>|<h[1-4]>|<H[1-4]>|</h[1-4]>|</H[1-4]>|</div>|</DIV>|<center>|<CENTER>|</center>|</CENTER>|\\[readmore\\]|</table>|</TABLE>|</td>|</TD>|</tr>|</TR>)","");
		gsub("^\t[ ]*"," ");
		gsub("&quot;","\"");
		gsub("&ccedil;","ç");
		gsub("&Ccedil;","Ç");
		gsub("&agrave;","à");
		gsub("&Agrave;","À");
		gsub("&egrave;","è");
		gsub("&Egrave;","È");
		gsub("&eacute;","é");
		gsub("&Eacute;","É");
		gsub("&icirc;","î");
		gsub("&Icirc;","Î");
		gsub("&ecirc;","ê");
		gsub("&Ecirc;","Ê");
		gsub("&acirc;","â");
		gsub("&Acirc;","Â");
		gsub("&ocirc;","ô");
		gsub("&Ocirc;","Ô");
		gsub("&ucirc;","û");
		gsub("&Ucirc;","Û");
		gsub("&euml;","");
		gsub("&Euml;","Ë");
		gsub("&euml;","ë");
		gsub("&Iuml;","Ï");
		gsub("&iuml;","ï");
		gsub("&Uuml;","Ü");
		gsub("&uuml;","ü");
		gsub("&laquo;","«");
		gsub("&raquo;","»");
		gsub("&reg;","(R)");
		gsub("&trade;","(TM)");
		gsub("<table.*>","");
		gsub("<TABLE.*>","");
		gsub("<td.*>","");
		gsub("<TD.*>","");
		gsub("<tr.*>","");
		gsub("<TR.*>","");
		gsub("<div.*>","");
		gsub("<DIV.*>","");
		gsub("<img .*>","");
		gsub("<IMG .*>","");
		gsub("<a href=\"index.php\\?currentpath=.*\">","");
		gsub("<a name=.*>.*</a>","");
		gsub("<a href=\"\\?id=.*\">", "");
		gsub("\\[include\\].*\\[/include\\]","");
		gsub("\\[/readmore\\]","");
		gsub("<a href=\"#.*\">.*</a>","");
		gsub("<a href=\"/fotos/.*\".*>","");
		gsub("<p[^>]*>", "");
		gsub("<br>", "\n");
		gsub("<br[ ]*/>", "\n");
		gsub("<BR>", "\n");


		
		

		
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
	}' | sed 's#</a>##g' | sed 's#<!--[.\r\n\t]*-->##g'
	
}

#underlines a string (languageindependent)
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



getHeadings()
{
rm /tmp/articlenumbers /tmp/pnHeadings.txt /tmp/pnBaseHeadings.txt 2> /dev/null
#create file which only contains article numbers, each one in one new line
for newsfile in `cat /tmp/articleList`
  do
	  number=`echo $newsfile | cut -d'#' -f1 | sed "s#'##g"` #get number of news article	
	  echo "$number" >> /tmp/articlenumbers
  done

echo " " >> /tmp/pnBaseHeadings.txt
#sort articlenumbers reversely, so that newest article with highest number is first, throw out doublettes 
for articlenumber in `cat /tmp/articlenumbers | sort -r -u`

#create file with correct headings
do
  
  #German
  for newsfile in `cat /tmp/articleList`
    do
      newsfile=`echo $newsfile | sed "s#'##g"` #delete all ' around the file name
      heading=`head -1 $articleDir$newsfile` #get first line of news article
      lang=`echo $newsfile | cut -d'#' -f2` #get language of news article
      number=`echo $newsfile | cut -d'#' -f1` #get number of news article
      if [ $number = $articlenumber ]
      then
	if [ $lang = "de" ]
	then
		echo -n "* $heading /"  >> /tmp/pnBaseHeadings.txt
	fi
      fi
    done
  #English
  for newsfile in `cat /tmp/articleList`
  do
    newsfile=`echo $newsfile | sed "s#'##g"` #delete all ' around the file name
    heading=`head -1 $articleDir$newsfile` #get first line of news article
    lang=`echo $newsfile | cut -d'#' -f2` #get language of news article
    number=`echo $newsfile | cut -d'#' -f1` #get number of news article
    if [ $number = $articlenumber ]
    then
      if [ $lang = "en" ]
      then
	      echo -n "/ $heading /" >> /tmp/pnBaseHeadings.txt
      fi
    fi
  done
  #French
  for newsfile in `cat /tmp/articleList`
  do
    newsfile=`echo $newsfile | sed "s#'##g"` #delete all ' around the file name
    heading=`head -1 $articleDir$newsfile` #get first line of news article
    lang=`echo $newsfile | cut -d'#' -f2` #get language of news article
    number=`echo $newsfile | cut -d'#' -f1` #get number of news article
    if [ $number = $articlenumber ]
    then
      if [ $lang = "fr" ]
      then
	      echo "/ $heading" >> /tmp/pnBaseHeadings.txt
      fi
    fi
  done
done
getASCII /tmp/pnBaseHeadings.txt > /tmp/pnHeadings.txt
}


getNews()
{
rm /tmp/pnNewsde.txt /tmp/pnNewsen.txt /tmp/pnNewsfr.txt /tmp/pnBaseNewsde.txt /tmp/pnBaseNewsen.txt /tmp/pnBaseNewsfr.txt 2> /dev/null

for newsfile in `cat /tmp/articleList`
do
	newsfile=`echo $newsfile | sed "s#'##g"` #delete all ' around the file name
	heading=`head -1 $articleDir$newsfile` #get first line of news article
	lang=`echo $newsfile | cut -d'#' -f2` #get language of news article

echo "
`underLine \"${heading}\"`

`getASCII $articleDir$newsfile`

" >> /tmp/pnBaseNews$lang.txt

echo " `getASCII /tmp/pnBaseNews$lang.txt`" > /tmp/pnNews$lang.txt
	
done
}

echo '#!/bin/sh

dialog --single-quoted --clear --title "News files" --checklist "Please select the news articles you want to use!" 13 70 5 \' > $tmpScr

ls $articleDir | sort -n -r | grep -v '~$' | head -n15 | awk '{print("\""$0"\" \"\" \"off\" \\")}' >> $tmpScr

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


echo
echo
echo
echo
getNews
getHeadings
getNewsletterTitle > /tmp/pnNewsletter.txt
echo "

Inhalt / Contents (english version in the middle) / Contenu (version française en bas)
" >> /tmp/pnNewsletter.txt
cat /tmp/pnHeadings.txt >> /tmp/pnNewsletter.txt
echo "
" >> /tmp/pnNewsletter.txt

cat /tmp/pnNewsde.txt >> /tmp/pnNewsletter.txt


echo "


===> english version


" >> /tmp/pnNewsletter.txt
cat /tmp/pnNewsen.txt >> /tmp/pnNewsletter.txt

echo "


===> version française


" >> /tmp/pnNewsletter.txt
cat /tmp/pnNewsfr.txt >> /tmp/pnNewsletter.txt

cat /tmp/pnNewsletter.txt | sed 's#[ \t]*$##' | sed '/^$/N;/\n$/N;//D' > /tmp/Newsletter.txt

echo "The newsletter was stored in /tmp/Newsletter.txt"
