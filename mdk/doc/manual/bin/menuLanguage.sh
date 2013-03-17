#let you select a language and stores the language short code in /tmp/m23language

m23language=`m23language 2>/dev/null` || m23language=/tmp/lilom23$$
dialog --backtitle "MDK manual generator" --default-item "de" --clear --title "Select language" \
        --menu "Select the language you want to generate the manual component for." 12 65 4 \
        "de"  "german" \
        "en"  "english" \
		"fr"  "french" \
		"de en fr" "all" 2> $m23language
retval=$?
case $retval in
    1)
       dialog --backtitle "MDK manual generator" --title "MDK manual generator" --clear --msgbox "Language selection aborted!" 12 41
       exit;;
    255)
       dialog --backtitle "MDK manual generator" --title "MDK manual generator" --clear --msgbox "Language selection aborted!" 12 41
       exit;;
esac

cp $m23language /tmp/m23language
