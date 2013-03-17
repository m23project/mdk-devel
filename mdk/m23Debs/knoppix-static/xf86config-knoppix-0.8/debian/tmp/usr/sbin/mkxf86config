#!/bin/bash

PATH="/bin:/usr/bin:/sbin:/usr/sbin"; export PATH
umask 022

# [ "`id -u`" != "0" ] && echo "WARNING: $0 has to run as root to work properly." 1>&2

# ANSI COLORS
CRE="[K"
NORMAL="[0;39m"
# RED: Failure or error message
RED="[1;31m"
# GREEN: Success message
GREEN="[1;32m"
# YELLOW: Descriptions
YELLOW="[1;33m"
# BLUE: System messages
BLUE="[1;34m"
# MAGENTA: Found devices or drivers
MAGENTA="[1;35m"
# CYAN: Questions
CYAN="[1;36m"
# BOLD WHITE: Hint
WHITE="[1;37m"

TMPFILE="/tmp/mkxf86config-$$"
MONITORTMP="$TMPFILE-monitor"

rm -f "$TMPFILE" "$MONITORTMP"

### Utility Function(s)

KVER="$(uname -r)"

# Reread boot command line; echo last parameter's argument or return false.
CMDLINE="$(cat /proc/cmdline)"

getbootparam(){
case "$CMDLINE" in *\ $1=*) ;; *) return 1; ;; esac
result="${CMDLINE##*$1=}"
result="${result%%[     ]*}"
echo "$result"
return 0
}

checkbootparam(){
case "$CMDLINE" in *\ $1*) return 0;; esac
return 1
}

# Read in what hwsetup has found for X
[ -f /etc/sysconfig/xserver ] && . /etc/sysconfig/xserver

# Xserver, Xmodule, Xscreen, Xvrefresh, if specified by commandline
NEWXSERVER="$(getbootparam xserver 2>/dev/null)"
[ -z "$XSERVER" -a -z "$NEWXSERVER" ] && NEWXSERVER="XFree86"
[ -n "$NEWXSERVER" ] && { XSERVER="$NEWXSERVER" ; echo "XSERVER='$XSERVER'" >> /etc/sysconfig/knoppix ; echo "XSERVER='$XSERVER'" >> /etc/sysconfig/xserver; }
NEWXMODULE="$(getbootparam xmodule 2>/dev/null)"
[ -z "$XMODULE" -a -z "$NEWXMODULE" ] && NEWXMODULE="vesa"
[ -n "$NEWXMODULE" ] && { XMODULE="$NEWXMODULE" ; echo "XMODULE='$XMODULE'" >> /etc/sysconfig/knoppix ; echo "XMODULE='$XMODULE'" >> /etc/sysconfig/xserver; }
XSCREEN="$(getbootparam xscreen 2>/dev/null)"
[ -z "$XSCREEN" ] && XSCREEN="$(getbootparam screen 2>/dev/null)"
[ -n "$XSCREEN" ] && { echo "XSCREEN='$XSCREEN'" >> /etc/sysconfig/knoppix ; echo "XSCREEN='$XSCREEN'" >> /etc/sysconfig/xserver; }
XVREFRESH="$(getbootparam xvrefresh 2>/dev/null)"
[ -n "$XVREFRESH" ] || XVREFRESH="$(getbootparam vrefresh 2>/dev/null)"
[ -n "$XVREFRESH" ] || XVREFRESH="$(getbootparam xvsync 2>/dev/null)"
[ -n "$XVREFRESH" ] || XVREFRESH="$(getbootparam vsync 2>/dev/null)"
[ -n "$XVREFRESH" ] && { echo "XVREFRESH='$XVREFRESH'" >> /etc/sysconfig/knoppix ; echo "XVREFRESH='$XVREFRESH'" >> /etc/sysconfig/xserver; }
XHREFRESH="$(getbootparam xhrefresh 2>/dev/null)"
[ -n "$XHREFRESH" ] || XHREFRESH="$(getbootparam hrefresh 2>/dev/null)"
[ -n "$XHREFRESH" ] || XHREFRESH="$(getbootparam xhsync 2>/dev/null)"
[ -n "$XHREFRESH" ] || XHREFRESH="$(getbootparam hsync 2>/dev/null)"
[ -n "$XHREFRESH" ] && { echo "XHREFRESH='$XHREFRESH'" >> /etc/sysconfig/knoppix ; echo "XHREFRESH='$XHREFRESH'" >> /etc/sysconfig/xserver; }
XDEPTH="$(getbootparam xdepth 2>/dev/null)"
[ -n "$XDEPTH" ] || XDEPTH="$(getbootparam depth 2>/dev/null)"
[ -n "$XDEPTH" ] && { echo "XDEPTH='$XDEPTH'" >> /etc/sysconfig/knoppix ; echo "XDEPTH='$XDEPTH'" >> /etc/sysconfig/xserver; }
# Wheel
WHEEL=""
if checkbootparam "wheel"; then
WHEEL='s|"PS/2"|"IMPS/2"\
Option          "ZAxisMapping"          "4 5"|g;'
elif checkbootparam "nowheel"; then
# Don't change the default, which is plain PS/2
WHEEL=""
else
# Apparently, the mouse auto-protocol option works in XFree 4.3 now. Let's give it a try.
WHEEL='s|"PS/2"|"auto"\
Option          "ZAxisMapping"          "4 5"|g;'
fi

# Read in changes
[ -f /etc/sysconfig/knoppix ] && . /etc/sysconfig/knoppix

# Read default keyboard from config file.
# There seems to be no reliable autoprobe possible.
[ -f /etc/sysconfig/keyboard ] && . /etc/sysconfig/keyboard

# Create mouse link and insert a mouse default type into XF86Config
# if not already done by hwsetup
[ -f /etc/sysconfig/mouse ] && . /etc/sysconfig/mouse
[ -e /dev/mouse ] || ln -sf /dev/ttyS0 /dev/mouse
# PROTO="${XMOUSETYPE:-Auto}"
PROTO="${XMOUSETYPE:-Microsoft}"
NOEMU=""
[ "${XEMU3}" = "no" ] && NOEMU='s|^.*Emulate3|# No 2 -> 3 Button emulation|g'
DEADKEYS=""
[ "$XKEYBOARD" = "de" ] || DEADKEYS='s|^.*nodeadkeys.*$||g;'
if [ -n "XMODULE" ]; then
# Check for Framebuffer X-Modules and initialize framebuffer module
case "$XMODULE" in
pvr2fb) modprobe "$XMODULE" >/dev/null 2>&1 ; XMODULE="fbdev"; ;;
esac
fi

NODDC=""
checkbootparam noddc &&  NODDC="true"

MONITOR=""
COMPLETE=""
RC=""

if [ -z "$NODDC" ]; then
# Try to get Monitor data via ddcxinfo-knoppix
MONITOR="$(ddcxinfo-knoppix -monitor)"
RC="$?"
COMPLETE="$(awk '/EndSection/{print}' <<EOF
$MONITOR
EOF)"
fi

if [ "$RC" != "0" -o -z "$MONITOR" -o -z "$COMPLETE" ]; then
MONITOR='
Section "Monitor"
	Identifier   "Monitor0"
	ModelName    "Old Monitor (no DDC)"
#	HorizSync    28.0 - 78.0 # Warning: This may fry very old Monitors
	HorizSync    28.0 - 96.0 # Warning: This may fry old Monitors
#	VertRefresh  50.0 - 76.0 # Very conservative. May flicker.
	VertRefresh  50.0 - 60.0 # Extreme conservative. Will flicker. TFT default.

	#  Default modes distilled from
	#      "VESA and Industry Standards and Guide for Computer Display Monitor
	#       Timing", version 1.0, revision 0.8, adopted September 17, 1998.
	#  $XFree86: xc/programs/Xserver/hw/xfree86/etc/vesamodes,v 1.4 1999/11/18 16:52:17 tsi Exp $
	# 640x350 @ 85Hz (VESA) hsync: 37.9kHz
	ModeLine "640x350"    31.5  640  672  736  832    350  382  385  445 +hsync -vsync
	# 640x400 @ 85Hz (VESA) hsync: 37.9kHz
	ModeLine "640x400"    31.5  640  672  736  832    400  401  404  445 -hsync +vsync
	# 720x400 @ 85Hz (VESA) hsync: 37.9kHz
	ModeLine "720x400"    35.5  720  756  828  936    400  401  404  446 -hsync +vsync
	# 640x480 @ 60Hz (Industry standard) hsync: 31.5kHz
	ModeLine "640x480"    25.2  640  656  752  800    480  490  492  525 -hsync -vsync
	# 640x480 @ 72Hz (VESA) hsync: 37.9kHz
	ModeLine "640x480"    31.5  640  664  704  832    480  489  491  520 -hsync -vsync
	# 640x480 @ 75Hz (VESA) hsync: 37.5kHz
	ModeLine "640x480"    31.5  640  656  720  840    480  481  484  500 -hsync -vsync
	# 640x480 @ 85Hz (VESA) hsync: 43.3kHz
	ModeLine "640x480"    36.0  640  696  752  832    480  481  484  509 -hsync -vsync
	# 800x600 @ 56Hz (VESA) hsync: 35.2kHz
	ModeLine "800x600"    36.0  800  824  896 1024    600  601  603  625 +hsync +vsync
	# 800x600 @ 60Hz (VESA) hsync: 37.9kHz
	ModeLine "800x600"    40.0  800  840  968 1056    600  601  605  628 +hsync +vsync
	# 800x600 @ 72Hz (VESA) hsync: 48.1kHz
	ModeLine "800x600"    50.0  800  856  976 1040    600  637  643  666 +hsync +vsync
	# 800x600 @ 75Hz (VESA) hsync: 46.9kHz
	ModeLine "800x600"    49.5  800  816  896 1056    600  601  604  625 +hsync +vsync
	# 800x600 @ 85Hz (VESA) hsync: 53.7kHz
	ModeLine "800x600"    56.3  800  832  896 1048    600  601  604  631 +hsync +vsync
	# 1024x768i @ 43Hz (industry standard) hsync: 35.5kHz
	ModeLine "1024x768"   44.9 1024 1032 1208 1264    768  768  776  817 +hsync +vsync Interlace
	# 1024x768 @ 60Hz (VESA) hsync: 48.4kHz
	ModeLine "1024x768"   65.0 1024 1048 1184 1344    768  771  777  806 -hsync -vsync
	# 1024x768 @ 70Hz (VESA) hsync: 56.5kHz
	ModeLine "1024x768"   75.0 1024 1048 1184 1328    768  771  777  806 -hsync -vsync
	# 1024x768 @ 75Hz (VESA) hsync: 60.0kHz
	ModeLine "1024x768"   78.8 1024 1040 1136 1312    768  769  772  800 +hsync +vsync
	# 1024x768 @ 85Hz (VESA) hsync: 68.7kHz
	ModeLine "1024x768"   94.5 1024 1072 1168 1376    768  769  772  808 +hsync +vsync
	# 1152x864 @ 75Hz (VESA) hsync: 67.5kHz
	ModeLine "1152x864"  108.0 1152 1216 1344 1600    864  865  868  900 +hsync +vsync
	# 1280x960 @ 60Hz (VESA) hsync: 60.0kHz
	ModeLine "1280x960"  108.0 1280 1376 1488 1800    960  961  964 1000 +hsync +vsync
	# 1280x960 @ 85Hz (VESA) hsync: 85.9kHz
	ModeLine "1280x960"  148.5 1280 1344 1504 1728    960  961  964 1011 +hsync +vsync
	# 1280x1024 @ 60Hz (VESA) hsync: 64.0kHz
	ModeLine "1280x1024" 108.0 1280 1328 1440 1688   1024 1025 1028 1066 +hsync +vsync
	# 1280x1024 @ 75Hz (VESA) hsync: 80.0kHz
	ModeLine "1280x1024" 135.0 1280 1296 1440 1688   1024 1025 1028 1066 +hsync +vsync
	# 1280x1024 @ 85Hz (VESA) hsync: 91.1kHz
	ModeLine "1280x1024" 157.5 1280 1344 1504 1728   1024 1025 1028 1072 +hsync +vsync
	# 1600x1200 @ 60Hz (VESA) hsync: 75.0kHz
	ModeLine "1600x1200" 162.0 1600 1664 1856 2160   1200 1201 1204 1250 +hsync +vsync
	# 1600x1200 @ 65Hz (VESA) hsync: 81.3kHz
	ModeLine "1600x1200" 175.5 1600 1664 1856 2160   1200 1201 1204 1250 +hsync +vsync
	# 1600x1200 @ 70Hz (VESA) hsync: 87.5kHz
	ModeLine "1600x1200" 189.0 1600 1664 1856 2160   1200 1201 1204 1250 +hsync +vsync
	# 1600x1200 @ 75Hz (VESA) hsync: 93.8kHz
	ModeLine "1600x1200" 202.5 1600 1664 1856 2160   1200 1201 1204 1250 +hsync +vsync
	# 1600x1200 @ 85Hz (VESA) hsync: 106.3kHz
	ModeLine "1600x1200" 229.5 1600 1664 1856 2160   1200 1201 1204 1250 +hsync +vsync
	# 1792x1344 @ 60Hz (VESA) hsync: 83.6kHz
	ModeLine "1792x1344" 204.8 1792 1920 2120 2448   1344 1345 1348 1394 -hsync +vsync
	# 1792x1344 @ 75Hz (VESA) hsync: 106.3kHz
	ModeLine "1792x1344" 261.0 1792 1888 2104 2456   1344 1345 1348 1417 -hsync +vsync
	# 1856x1392 @ 60Hz (VESA) hsync: 86.3kHz
	ModeLine "1856x1392" 218.3 1856 1952 2176 2528   1392 1393 1396 1439 -hsync +vsync
	# 1856x1392 @ 75Hz (VESA) hsync: 112.5kHz
	ModeLine "1856x1392" 288.0 1856 1984 2208 2560   1392 1393 1396 1500 -hsync +vsync
	# 1920x1440 @ 60Hz (VESA) hsync: 90.0kHz
	ModeLine "1920x1440" 234.0 1920 2048 2256 2600   1440 1441 1444 1500 -hsync +vsync
	# 1920x1440 @ 75Hz (VESA) hsync: 112.5kHz
	ModeLine "1920x1440" 297.0 1920 2064 2288 2640   1440 1441 1444 1500 -hsync +vsync
	# Additional modelines
	ModeLine "1800x1440"  230    1800 1896 2088 2392  1440 1441 1444 1490 +HSync +VSync
	ModeLine "1800x1440"  250    1800 1896 2088 2392  1440 1441 1444 1490 +HSync +VSync
	# Extended modelines with GTF timings
	# 640x480 @ 100.00 Hz (GTF) hsync: 50.90 kHz; pclk: 43.16 MHz
	ModeLine "640x480"  43.16  640 680 744 848  480 481 484 509  -HSync +Vsync
	# 768x576 @ 60.00 Hz (GTF) hsync: 35.82 kHz; pclk: 34.96 MHz
	ModeLine "768x576"  34.96  768 792 872 976  576 577 580 597  -HSync +Vsync
	# 768x576 @ 72.00 Hz (GTF) hsync: 43.27 kHz; pclk: 42.93 MHz
	ModeLine "768x576"  42.93  768 800 880 992  576 577 580 601  -HSync +Vsync
	# 768x576 @ 75.00 Hz (GTF) hsync: 45.15 kHz; pclk: 45.51 MHz
	ModeLine "768x576"  45.51  768 808 888 1008  576 577 580 602  -HSync +Vsync
	# 768x576 @ 85.00 Hz (GTF) hsync: 51.42 kHz; pclk: 51.84 MHz
	ModeLine "768x576"  51.84  768 808 888 1008  576 577 580 605  -HSync +Vsync
	# 768x576 @ 100.00 Hz (GTF) hsync: 61.10 kHz; pclk: 62.57 MHz
	ModeLine "768x576"  62.57  768 816 896 1024  576 577 580 611  -HSync +Vsync
	# 800x600 @ 100.00 Hz (GTF) hsync: 63.60 kHz; pclk: 68.18 MHz
	ModeLine "800x600"  68.18  800 848 936 1072  600 601 604 636  -HSync +Vsync
	# 1024x768 @ 100.00 Hz (GTF) hsync: 81.40 kHz; pclk: 113.31 MHz
	ModeLine "1024x768"  113.31  1024 1096 1208 1392  768 769 772 814  -HSync +Vsync
	# 1152x864 @ 60.00 Hz (GTF) hsync: 53.70 kHz; pclk: 81.62 MHz
	ModeLine "1152x864"  81.62  1152 1216 1336 1520  864 865 868 895  -HSync +Vsync
	# 1152x864 @ 85.00 Hz (GTF) hsync: 77.10 kHz; pclk: 119.65 MHz
	ModeLine "1152x864"  119.65  1152 1224 1352 1552  864 865 868 907  -HSync +Vsync
	# 1152x864 @ 100.00 Hz (GTF) hsync: 91.50 kHz; pclk: 143.47 MHz
	ModeLine "1152x864"  143.47  1152 1232 1360 1568  864 865 868 915  -HSync +Vsync
	# 1280x960 @ 72.00 Hz (GTF) hsync: 72.07 kHz; pclk: 124.54 MHz
	ModeLine "1280x960"  124.54  1280 1368 1504 1728  960 961 964 1001  -HSync +Vsync
	# 1280x960 @ 75.00 Hz (GTF) hsync: 75.15 kHz; pclk: 129.86 MHz
	ModeLine "1280x960"  129.86  1280 1368 1504 1728  960 961 964 1002  -HSync +Vsync
	# 1280x960 @ 100.00 Hz (GTF) hsync: 101.70 kHz; pclk: 178.99 MHz
	ModeLine "1280x960"  178.99  1280 1376 1520 1760  960 961 964 1017  -HSync +Vsync
	# 1280x1024 @ 100.00 Hz (GTF) hsync: 108.50 kHz; pclk: 190.96 MHz
	ModeLine "1280x1024"  190.96  1280 1376 1520 1760  1024 1025 1028 1085  -HSync +Vsync
	# 1400x1050 @ 60.00 Hz (GTF) hsync: 65.22 kHz; pclk: 122.61 MHz
	ModeLine "1400x1050"  122.61  1400 1488 1640 1880  1050 1051 1054 1087  -HSync +Vsync
	# 1400x1050 @ 72.00 Hz (GTF) hsync: 78.77 kHz; pclk: 149.34 MHz
	ModeLine "1400x1050"  149.34  1400 1496 1648 1896  1050 1051 1054 1094  -HSync +Vsync
	# 1400x1050 @ 75.00 Hz (GTF) hsync: 82.20 kHz; pclk: 155.85 MHz
	ModeLine "1400x1050"  155.85  1400 1496 1648 1896  1050 1051 1054 1096  -HSync +Vsync
	# 1400x1050 @ 85.00 Hz (GTF) hsync: 93.76 kHz; pclk: 179.26 MHz
	ModeLine "1400x1050"  179.26  1400 1504 1656 1912  1050 1051 1054 1103  -HSync +Vsync
	# 1400x1050 @ 100.00 Hz (GTF) hsync: 111.20 kHz; pclk: 214.39 MHz
	ModeLine "1400x1050"  214.39  1400 1512 1664 1928  1050 1051 1054 1112  -HSync +Vsync
	# 1600x1200 @ 100.00 Hz (GTF) hsync: 127.10 kHz; pclk: 280.64 MHz
	ModeLine "1600x1200"  280.64  1600 1728 1904 2208  1200 1201 1204 1271  -HSync +Vsync
EndSection
'
fi

# Extract values for display
MODEL="$(awk '/^[	 ]*ModelName/{print;exit}'<<EOF
$MONITOR
EOF
)"

MODEL="${MODEL#*\"}"
MODEL="${MODEL%\"*}"

HREFRESH="$(awk '/^[	 ]*HorizSync/{print $2 $3 $4; exit}'<<EOF
$MONITOR
EOF
)"

VREFRESH="$(awk '/^[	 ]*VertRefresh/{print $2 $3 $4; exit}'<<EOF
$MONITOR
EOF
)"

# Build line of allowed modes
MODES=""
ADDMODE=""
# Use commandline modes if specified
[ -n "$XSCREEN" ] && ADDMODE="-firstmode $XSCREEN"
if [ -z "$NODDC" ]; then
MODES="$(ddcxinfo-knoppix -modes $ADDMODE)"
fi
if [ "$?" != "0" -o -z "$MODES" ]; then
MODES="Modes "
[ -n "$XSCREEN" ] && MODES="$MODES \"$XSCREEN\""
MODES="$MODES \"1024x768\" \"800x600\" \"640x480\""
fi

# Use commandline vertical refresh rate if specified
if [ -n "$XVREFRESH" ]; then
MONITOR="$(sed 's|VertRefresh[	 ].*$|VertRefresh 49.0 - '"$XVREFRESH"'|g;' <<EOF
$MONITOR
EOF
)"
fi

# Use commandline horizontal refresh rate if specified
if [ -n "$XHREFRESH" ]; then
MONITOR="$(sed 's|HorizSync[	 ].*$|HorizSync 28.0 - '"$XHREFRESH"'|g;' <<EOF
$MONITOR
EOF
)"
fi

DEPTH=""
# Use commandline colordepth if specified
if [ -n "$XDEPTH" ]; then
DEPTH='s|DefaultColorDepth[	 ].*$|DefaultColorDepth '"$XDEPTH"'|g;'
fi

# We need to check this because serial mice have long timeouts
SERIALMOUSE="$(ls -l1 /dev/mouse* 2>/dev/null | awk '/ttyS/{print $NF ; exit 0}')"
if [ -n "$SERIALMOUSE" ]; then
SERIALMOUSE="s|/dev/ttyS0|$SERIALMOUSE|g;"
else
SERIALMOUSE='s|^.*InputDevice.*"Serial Mouse".*$|# Serial Mouse not detected|g;'
fi

# PS/2 bug: Some keyboards are incorrectly used as mice in XFree. :-(
[ -f /etc/sysconfig/mouse ] && . /etc/sysconfig/mouse
PSMOUSE="$(ls -l1 /dev/mouse* 2>/dev/null | awk '/psaux/{print $NF ; exit 0}' && echo $XMOUSETYPE | awk '/PS\/2/{print $NF ; exit 0}')"
if [ -n "$PSMOUSE" ]; then
PSMOUSE=""
else
PSMOUSE='s|^.*InputDevice.*"PS/2 Mouse".*$|# PS/2 Mouse not detected|g;'
fi

case "$(cat /proc/modules)" in *usbmouse*|*mousedev*|*hid\ *)
USBMOUSE="" ;;
*)
USBMOUSE='s|^.*InputDevice.*"USB Mouse".*$|# USB Mouse not detected|g;';;
esac

# Kernel 2.6 reports psaux via /dev/input/mice like USB
case "$KVER" in 2.6.*)
if [ -z "$PSMOUSE" ]; then
PSMOUSE='s|^.*InputDevice.*"PS/2 Mouse".*$|# PS/2 Mouse using /dev/input/mice in Kernel 2.6|g;'
USBMOUSE=""
fi
;;
esac

# Write Monitor data now
rm -f "$MONITORTMP"
echo "$MONITOR" >"$MONITORTMP"

# /etc/X11/XF86Config* is the "default" config.
if [ ! -f /etc/XF86Config -a ! -f /etc/X11/XF86Config ]; then
# Remove it, just in case it's a link to CD
rm -f /etc/XF86Config /etc/X11/XF86Config 2>/dev/null
sed -e 's|@@PROTOCOL@@|'"$PROTO"'|g;'"$NOEMU" \
    -e '/@@MONITOR@@/r '"$MONITORTMP" \
    -e 's|@@MONITOR@@||g' \
    -e 's|@@MODES@@|'"$MODES"'|g;'"$DEPTH" \
    -e 's|XkbLayout *"[^"]*"|XkbLayout "'"$XKEYBOARD"'"|g;'"$DEADKEYS" \
    /etc/X11/XF86Config.in > /etc/X11/XF86Config
fi

if [ ! -f /etc/XF86Config-4 -a ! -f /etc/X11/XF86Config-4 ]; then 
rm -f /etc/XF86Config-4 /etc/X11/XF86Config-4 2>/dev/null

# VMWare special handling
VMWARE=""
if [ "$XMODULE" = "vmware" ]; then
VMWARE='s|^.*BusID.*PCI.*$|BusID "PCI:0:15:0"|g;'
DEPTH='s|DefaultColorDepth |# DefaultColorDepth |g;'
fi

# Do NOT use a default colordepth setting if we are using the "fbdev" module
if [ "$XMODULE" = "fbdev" ]; then
DEPTH='s|DefaultColorDepth |# DefaultColorDepth |g;'
fi

# These drivers need the sw_cursor option
SWCURSOR=""
case "$XMODULE" in ati|radeon|nv|trident) SWCURSOR='s|^.*#Option.*"sw_cursor".*$|Option "sw_cursor"|g;';; esac

# We must use NoPM, because some machines freeze if Power management is being activated.

NOPM=""
DPMS=""
checkbootparam noapm && NOPM='Option	"NoPM"	"true"' || DPMS='Option	"DPMS"	"true"'

sed -e 's|@@PROTOCOL@@|'"$PROTO"'|g;'"$NOEMU" \
    -e '/@@MONITOR@@/r '"$MONITORTMP" \
    -e 's|@@MONITOR@@||g' \
    -e 's|@@NOPM@@|'"$NOPM"'|g' \
    -e 's|@@XMODULE@@|'"$XMODULE"'|g;'"$VMWARE""$SERIALMOUSE""$USBMOUSE""$PSMOUSE""$SWCURSOR""$WHEEL" \
    -e 's|@@MODES@@|'"$MODES"'|g;'"$DEPTH" \
    -e 's|"XkbLayout" *"[^"]*"|"XkbLayout" "'"$XKEYBOARD"'"|g;'"$DEADKEYS" \
    /etc/X11/XF86Config-4.in >/etc/X11/XF86Config-4

if [ -n "$DPMS" ]; then
sed -e 's|Identifier[	 ]*"Monitor0"|Identifier	"Monitor0"\
	'"$DPMS"'|g' /etc/X11/XF86Config-4 >/etc/X11/XF86Config-4.new
mv -f /etc/X11/XF86Config-4.new /etc/X11/XF86Config-4
fi

fi

rm -f "$TMPFILE" "$MONITORTMP" 2>/dev/null

# Print info about selected X-Server
[ -n "$XDESC" ] || XDESC="(yet) unknown card"
echo -n " ${GREEN}Video is"
[ -n "$XDESC" ] && echo -n " ${YELLOW}$XDESC${GREEN},"
echo -n " using ${YELLOW}${XSERVER:-generic VESA}"
[ -n "$XMODULE" ] && echo -n "(${MAGENTA}$XMODULE${YELLOW})"
echo " Server${NORMAL}"
echo -n " ${GREEN}Monitor is ${YELLOW}${MODEL:-Generic Monitor}${NORMAL}"
[ -n "$HREFRESH" -a -n "$VREFRESH" ] && echo "${GREEN}, ${GREEN}H:${YELLOW}${HREFRESH}kHz${GREEN}, V:${YELLOW}${VREFRESH}Hz${NORMAL}" || echo ""
[ -n "$XVREFRESH" ] && echo " ${GREEN}Trying specified vrefresh rate of ${YELLOW}${XVREFRESH}Hz.${NORMAL}"
[ -n "$MODES" ] && echo " ${GREEN}Using Modes ${YELLOW}${MODES##Modes }${NORMAL}"
