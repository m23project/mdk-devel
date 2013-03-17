if test $# -lt 3
then
	echo "$0 <input HTML file> <image file> <output Tex file> [image scale factor]"
	exit 1
fi

if test $# -eq 4
then
	scale=$4
else
	scale=0.4
fi

echo "converting HTML->Tex $1"

php /mdk/doc/manual/bin/readHelp.php $1 $2 $scale > $3
