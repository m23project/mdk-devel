#!/bin/bash

ls *.tar.7z | while read basesys
do
	if [ ! -e "$basesys.sig" ]
	then
		cat "$basesys" | gpg --default-key 0x3305E63A --no-options --detach-sign --armor --textmode > "$basesys.sig"
	fi
done

rsync -Pya *.sig root@tux05:/var/www/basesys