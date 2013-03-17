#!/bin/bash
I18N_windowheader=`cat /version.txt`

dialog --backtitle "$I18N_windowheader" --default-item "de" --clear --title "$I18N_windowheader" --menu "Installation language/Installationssprache" 9 60 2 \
		"de" "Deutsch" \
		"en" "English" 2> /tmp/dialog.value

langFile="/bin/installI18N.`cat /tmp/dialog.value`"

. /bin/m23InstallerBase.inc
. $langFile

lang=`cat /tmp/dialog.value`
keyFile="/$lang.key"
langCode=$(echo $lang | awk '{print($0"_"toupper($0)".UTF-8");}')

if [ -f $keyFile ]
then
	#load German keyboard layout if "de" is selected
	/bin/loadkmap < $keyFile 2> /dev/null
fi

export LC_ALL=$langCode 2> /dev/null
export LANG=$langCode 2> /dev/null

/etc/init.d/m23hwdetect

startBaseInstallation
