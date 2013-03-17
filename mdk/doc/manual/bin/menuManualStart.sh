. /mdk/bin/docFunctions.inc

scrde=`find /mdk/doc/manual/screenshots/de/serverupdate.png -printf "Screenshots [%TD %TH:%TM]"`
textde=`find /mdk/doc/manual/tex/de/welcome.hlp.tex -printf "Text [%TD %TH:%TM]\n"`
pdfde=`find /mdk/doc/manual/pdf/m23-manual-de.pdf -printf "        PDF         [%TD %TH:%TM]\n"`
htmlde=`find /mdk/doc/manual/html/de/index.html -printf "HTML [%TD %TH:%TM]"`

scren=`find /mdk/doc/manual/screenshots/en/serverupdate.png -printf "Screenshots [%TD %TH:%TM]"`
texten=`find /mdk/doc/manual/tex/en/welcome.hlp.tex -printf "Text [%TD %TH:%TM]\n"`
pdfen=`find /mdk/doc/manual/pdf/m23-manual-en.pdf -printf "       PDF         [%TD %TH:%TM]\n"`
htmlen=`find /mdk/doc/manual/html/en/index.html -printf "HTML [%TD %TH:%TM]"`

scrfr=`find /mdk/doc/manual/screenshots/fr/serverupdate.png -printf "Screenshots [%TD %TH:%TM]"`
textfr=`find /mdk/doc/manual/tex/fr/welcome.hlp.tex -printf "Text [%TD %TH:%TM]\n"`
pdffr=`find /mdk/doc/manual/pdf/m23-manual-fr.pdf -printf "        PDF         [%TD %TH:%TM]\n"`
htmlfr=`find /mdk/doc/manual/html/fr/index.html -printf "HTML [%TD %TH:%TM]"`

lang=`cat /tmp/m23language`

#check if screenshots are available for a certain language
if test -z "$scrde"
then
scrde="Screenshots [not generated]"
fi

if test -z "$scren"
then
scren="Screenshots [not generated]"
fi

if test -z "$scrfr"
then
scrfr="Screenshots [not generated]"
fi





#check if text is available for a certain language
if test -z "$textde"
then
textde="Text [not generated]"
fi

if test -z "$texten"
then
texten="Text [not generated]"
fi

if test -z "$textfr"
then
textfr="Text [not generated]"
fi





#check if pdf is available for a certain language
if test -z "$pdfde"
then
pdfde="       PDF         [not generated]"
fi

if test -z "$pdfen"
then
pdfen="         PDF         [not generated]"
fi

if test -z "$pdffr"
then
pdffr="        PDF         [not generated]"
fi





#check if HTML is available for a certain language
if test -z "$htmlde"
then
htmlde="HTML [not generated]"
fi

if test -z "$htmlen"
then
htmlen="HTML [not generated]"
fi

if test -z "$htmlfr"
then
htmlfr="HTML [not generated]"
fi





menuSelection=`menuSelection 2>/dev/null` || menuSelection=/tmp/lilom23$$
dialog --backtitle "MDK manual generator" --default-item "ja" --clear --title "Select what you want to do" \
        --menu "Last updated:\nGerman:  $scrde $textde\n $pdfde $htmlde\nEnglish: $scren $texten\n$pdfen $htmlen\nFrench:  $scrfr $textfr\n $pdffr $htmlfr\n\nSelected language(s): $lang" 21 75 6 \
        "lang"  "select manual language" \
        "screenshots"  "make screenshots for the manual" \
		"text"  "generate text files" \
		"pdf-html" "generate the PDF and HTML manual(s)"\
		"upload" "upload the PDF and HTML files"\
		"help" "help" 2> $menuSelection
retval=$?
case $retval in
    1)
       dialog --backtitle "MDK manual generator" --title "MDK manual generator" --clear --msgbox "Language selection aborted!" 12 41
       exit;;
    255)
       dialog --backtitle "MDK manual generator" --title "MDK manual generator" --clear --msgbox "Language selection aborted!" 12 41
       exit;;
esac


choice=`cat $menuSelection`

if test $choice == "lang"
then
	/mdk/doc/manual/bin/menuLanguage.sh
	/mdk/doc/manual/bin/menuManualStart.sh
fi

if test $choice == "screenshots"
then
	/mdk/doc/manual/bin/menuScreenshot.sh
	/mdk/doc/manual/bin/menuManualStart.sh
fi

if test $choice == "text"
then
	/mdk/doc/manual/bin/makeTexHelp.sh
	/mdk/doc/manual/bin/menuManualStart.sh
fi

if test $choice == "pdf-html"
then
	/mdk/doc/manual/bin/makePDF-HTML.sh
	/mdk/doc/manual/bin/menuManualStart.sh
fi

if test $choice == "upload"
then
	release=`date +"%Y-%m-%d"`
	datetime=`date +"%Y-%m-%d %H:%M:%S"`
	version=`grep version /m23/inc/version.php | cut -d'"' -f2`

	#Create the manual download link page
	echo "
	<?
\$downloads[\$i++] = array (
title => \"Manuel de l'utilisateur (Fran&ccedil;ais)\",
descr => \"Manuel de l'utilisateur pour m23 (pdf)\",
version => \"$version\",
size => \"`find -L /mdk/doc/manual/pdf/m23-manual-fr.pdf -printf %s`\",
date => \"$datetime\",
filename => \"m23-manual-fr.pdf\",
url => \"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-fr.pdf/download\"
);

\$downloads[\$i++] = array (
title => \"User manual (English)\",
descr => \"User manual for m23 (pdf)\",
version => \"$version\",
size => \"`find -L /mdk/doc/manual/pdf/m23-manual-en.pdf -printf %s`\",
date => \"$datetime\",
filename => \"m23-manual-en.pdf\",
url => \"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-en.pdf/download\"
);

\$downloads[\$i++] = array (
title => \"Benutzerhandbuch (deutsch)\",
descr => \"m23-Benutzerhandbuch (pdf)\",
version => \"$version\",
size => \"`find -L /mdk/doc/manual/pdf/m23-manual-de.pdf -printf %s`\",
date => \"$datetime\",
filename => \"m23-manual-de.pdf\",
url => \"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-de.pdf/download\"
);
?>" > /matrix23/arbeit/wwwTests/cms/data/home/Downloads/docs/manualDownload.html.php

# 	echo "
# <!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">
# <html>
# 
# <head>
#   <title>Manual download</title>
#   <meta name=\"GENERATOR\" content=\"MDK\">
#   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-15\">
# </head>
# <body>
# <table>
#     <tr>
#       <td>m23-Handbuch (deutsch)</td>
#       <td><a href=\"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-de.pdf/download\">Herunterladen</a></td>
#       <td>`find -L /mdk/doc/manual/pdf/m23-manual-de.pdf -printf %s` Bytes</td>
#     </tr>
#     <tr>
#       <td>m23 manual (english)</td>
#       <td><a href=\"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-en.pdf/download\">Download</a></td>
#       <td>`find -L /mdk/doc/manual/pdf/m23-manual-en.pdf -printf %s` Bytes</td>
#     </tr>
#     <tr>
#       <td>Manuel de l'utilisateur pour m23 (french)</td>
#       <td><a href=\"http://sourceforge.net/projects/m23/files/docs/$release/m23-manual-fr.pdf/download\">Download</a></td>
#       <td>`find -L /mdk/doc/manual/pdf/m23-manual-fr.pdf -printf %s` Bytes</td>
#     </tr>
# </table>
# 
# </body>
# </html> " > /mdk/doc/manual/html/manualDownload.html

	
	uploadPDFFRS /mdk/doc/manual/pdf/m23-manual-de.pdf
	uploadPDFFRS /mdk/doc/manual/pdf/m23-manual-en.pdf
	uploadPDFFRS /mdk/doc/manual/pdf/m23-manual-fr.pdf

	uploadHtml /mdk/doc/manual/ /home/project-web/m23/htdocs/docs/manual/

	#Upload the changed manual download link page
	/matrix23/arbeit/wwwTests/cms-upload manualDownload.html.php

	/mdk/doc/manual/bin/menuManualStart.sh
fi


if test $choice == "help"
then
	dialog --backtitle "m23 Software Development Kit (MDK)" --textbox help/menuManual.hlp 22 80
	/mdk/doc/manual/bin/menuManualStart.sh
fi
