#/bin/bash
grep -r sourceforge.net /mdk /m23 | grep m23inst | cut -d':' -f1 | sort -u