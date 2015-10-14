#!/bin/bash

ls *.tar.7z | while read basesys
do
	cat "$basesys" | gpg --default-key 0x3305E63A --no-options --detach-sign --armor --textmode > "$basesys.sig"
done

rsync -Pya *.sig root@tux05:/var/www/basesys