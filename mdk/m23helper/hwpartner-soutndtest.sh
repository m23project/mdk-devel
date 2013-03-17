#!/bin/bash

echo "Anzahl der Ausgänge?"
read num

for i in seq 1 $num
do
	echo "Kopfhörer einstecken, dann Enter drücken:"
	read lala
	speaker-test -c 6 -l 1 -t wav
done

echo "Aufnahmetest"

sucheAusgabe()
{
	dev="$1"
	echo "Ausgabesoundkarte suchen: Kopfhörer einstecken"
	while `true`
	do
		echo "Drücke Enter"
		read lala
		aplay -D $dev /usr/share/sounds/alsa/Front_Center.wav
		echo "Kommt Ton? (j/n)"
		read lala
		if [ $lala = "j" ]
		then
			break
		fi
	done
}


export LC_ALL=C; arecord -l | grep '^card' | sed 's#card \([0-9]*\).*device \([0-9]*\).*#hw:\1,\2#' | while read dev
do
	echo "ALSA: $dev"
	sucheAusgabe $dev

	while `true`
	do
		echo "Mikrofon/Line einstecken, dann Enter drücken:"
		read lala
		arecord -D $dev -d 10 /tmp/test-mic.wav
		aplay -D $dev /tmp/test-mic.wav
		echo "Kommt Ton? (j/n)"
		read lala
		if [ $lala = "j" ]
		then
			break
		fi
	done
done