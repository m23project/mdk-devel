if test -e ~/.mdkScreenshotPWD
then
. ~/.mdkScreenshotPWD
else
	#ask the user if he wants to procede
	startscr=`startscr 2>/dev/null` || startscr=/tmp/lilom23$$
	dialog --backtitle "MDK manual generator - Screenshots" --default-item "no" --clear --title "Start making screenshots"\
			--menu "In order to run this script you need the patched version of khtml2png (can be found in the MDK directory) and KDE3 installed. You have to execute this script under a running X session! The m23 database will be exchanged temporary, so don't abort the screenshot generation! And don't click around during the taking of the screenshots!\n" 14 65 2 \
			"yes"  "start generation of screenshots" \
			"no" "stop right now!" 2> $startscr
	retval=$?
	case $retval in
		1)
		dialog --backtitle i18n_windowheader_ --title "Start making screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
		/mdk/doc/manual/bin/menuManualStart.sh
		exit;;
		255)
		dialog --backtitle "Start making screenshots" --title i18n_windowheader_ --clear --msgbox "Screenshot generation canceled!" 12 41
		/mdk/doc/manual/bin/menuManualStart.sh
		exit;;
	esac
	
	choice=`cat $startscr`
	
	if test $choice == "no"
	then
		/mdk/doc/manual/bin/menuManualStart.sh	
		exit
	fi





	#ask for the m23 web interface username
	username=`username 2>/dev/null` || username=/tmp/username$$
	dialog --backtitle "MDK manual generator - Screenshots" --title "Enter username" --clear \
	--inputbox "Enter the username you are using for accessing the m23 server web interface." 10 51 2> $username
	retval=$?
	case $retval in
		1)
		dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
		/mdk/doc/manual/bin/menuManualStart.sh
		exit;;
		255)
		dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
		/mdk/doc/manual/bin/menuManualStart.sh
		exit;;
	esac




	#ask (twice) for the m23 web interface password
	while `true`
	do
		rootpw=`rootpw 2>/dev/null` || rootpw=/tmp/rootpwtemp$$
		dialog --backtitle "MDK manual generator - Screenshots" --title "Enter password" --clear \
		--passwordbox "Enter the password you are using for accessing the m23 server web interface." 10 51 2> $rootpw
		retval=$?
		case $retval in
			1)
			dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
			exit;;
			255)
			dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
			exit;;
		esac
	
	
		rootpwb=`rootpwb 2>/dev/null` || rootpwb=/tmp/rootpwbtemp$$
		dialog --backtitle "MDK manual generator - Screenshots" --title "Enter password" --clear \
		--passwordbox "Enter the password you are using for accessing the m23 server web interface. Once again for verification." 10 51 2> $rootpwb
		retval=$?
		case $retval in
			1)
			dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
			exit;;
			255)
			dialog --backtitle "MDK manual generator - Screenshots" --title "MDK manual generator - Screenshots" --clear --msgbox "Screenshot generation canceled!" 12 41
			exit;;
		esac
	
		if test `cat $rootpw` != `cat $rootpwb`
		then
		dialog --backtitle "MDK manual generator - Screenshots" --title "Passwords doen't match!" --clear --msgbox "Entered passwords don't match" 12 60
		else
		pwdismatch=0
		break
		fi
	done
	
	export M23PWD="`cat $rootpw`"
	rm /tmp/rootpw*
	export M23USER="`cat $username`"
fi

cd /mdk/doc/manual/bin/
./makeScreenshots.sh