#gcc $1 wait4go.c -o wait4go
#strip -g wait4go

. /mdk/bin/archFunctions.inc
checkForx86_64Toolchain

gcc wait4go.c -o /mdk/client+server/compiled/i386/bin/wait4go
strip -g /mdk/client+server/compiled/i386/bin/wait4go 

x86_64-unknown-linux-gnu-gcc wait4go.c -o /mdk/client+server/compiled/amd64/bin/wait4go
strip -g /mdk/client+server/compiled/amd64/bin/wait4go
