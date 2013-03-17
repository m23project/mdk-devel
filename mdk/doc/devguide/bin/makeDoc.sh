#get version number & codename from the php file
m23codename=`cat /m23/inc/version.php | grep codename | cut -d"\"" -f2`
m23version=`cat /m23/inc/version.php | grep version | cut -d"\"" -f2`

#save it to the version.tex
echo "m23 $m23codename $m23version" > /mdk/doc/devguide/version.tex

chmod 700 /mdk/doc/devguide/html

cd /m23
echo "Generating the LaTeX file..."
/mdk/doc/devguide/bin/mdoc.sh . /mdk/doc/devguide/m23api.tex
cd /mdk/doc/devguide
rm -f -r *.dvi *.pdf devguide.ps html/* *.toc 2> /dev/null

#First run for creating the table of contents
pdflatex devguide.tex
#Next run to create the PDF with the updated table of contents
pdflatex devguide.tex

cd /mdk/doc/devguide
latex2html -local_icons devguide.tex -dir /mdk/doc/devguide/html

cp /mdk/doc/devguide/devguide.pdf /mdk/server/iso/doc

chmod 555 /mdk/doc/devguide/html -R