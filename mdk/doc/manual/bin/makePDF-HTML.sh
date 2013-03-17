
for LANG in `cat /tmp/m23language`
do
	cd /mdk/doc/manual/tex/$LANG/not-generated

	rm manual.toc manual.aux manual.log

	pdflatex -interaction nonstopmode manual.tex
	
	if test $? -ne 0
	then
		echo "There were errors making the PDF"
		echo "Press \"Enter\" to continue"
		read
	fi

	pdflatex -interaction nonstopmode manual.tex

	#remove the pdf to make sure the link is deleted
	rm /mdk/doc/manual/pdf/m23-manual-$LANG.pdf
	mv manual.pdf /mdk/doc/manual/pdf/m23-manual-$LANG.pdf

	mkdir -p /mdk/doc/manual/html/$LANG

	latex2html manual.tex -local_icons -dir /mdk/doc/manual/html/$LANG/

	if test -d /m23/doc/manual/
	then
		ln -s /mdk/doc/manual/html/$LANG /m23/doc/manual/
	fi

	#move the pdf to the server boot CD directory and link it back
	mv /mdk/doc/manual/pdf/m23-manual-$LANG.pdf /mdk/server/iso/doc/
	ln -s /mdk/server/iso/doc/m23-manual-$LANG.pdf /mdk/doc/manual/pdf

	chmod 755 /m23/doc/manual/
done
