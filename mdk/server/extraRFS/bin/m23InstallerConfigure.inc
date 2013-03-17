downloadFiles="mdk.tb2 m23.tb2 m23MySQL-Init.sh m23captured.sql.bz2 m23.sql.bz2"


#sets some initial values
initInstall()
{
echo -n "php4-mysql php4 mysql-client mysql-common mysql-server apache apache-common squid squidclient tftpd-hpa etherwake sudo wget dhcp3-server bzip2 bind libapache-mod-ssl iputils-ping dpkg-dev libncurses5-dev mknbi debootstrap putty-tools reprepro syslinux php4-cli screen pwgen" > /m23installer.packages

	export DEBIAN_FRONTEND=noninteractive

	#make sure we have a new package list and debconf is installed
	apt-get update
	apt-get install -y --force-yes debconf dialog

	#save the debconf settings file
	cat >> /tmp/debconf-setting << "EOFDEBCONFSETTINGS"
apache apache/enable-suexec boolean true
samba-common samba-common/encrypt_passwords boolean true
samba-common samba-common/codepage string
samba-common samba-common/character_set string
samba-common samba-common/do_debconf boolean true
samba-common samba-common/dhcp boolean false
samba-common samba-common/workgroup string
mysql-server mysql-server/fix_privileges_warning note
mysql-server mysql-server/mysql_install_db_notes note
mysql-server mysql-server/nis_warning note
mysql-server mysql-server/postrm_remove_database boolean false
mysql-server mysql-server/skip_networking boolean false
mysql-server mysql-server/start_on_boot boolean true
mysql-server mysql-server/want_chroot boolean false
squid squid-cgi/cachemgr note
squid squid/fix_lines boolean true
squid squid/http_anonymizer note
squid squid/old_version boolean
dhcp3-server dhcp3-server/config_warn note
dhcp3-server dhcp3-server/interfaces string
dhcp3-server dhcp3-server/new_auth_behavior note
EOFDEBCONFSETTINGS
};



#sets DebConf settings and installs needed packages
installPackages()
{
	export DEBIAN_FRONTEND=noninteractive
	
	#set debconf settings
	debconf-set-selections /tmp/debconf-setting

	#install needed packages
	apt-get install -y --force-yes `cat /m23installer.packages`
};



#shows a message
showMessage()
{
	#$1 message text
	echo "$1"
};




#show values from MySQL debian.cnf file
getValForMySQL()
{
	#$1 variable name
	grep "^$1" /etc/mysql/debian.cnf | tr -d [:blank:] | cut -d'=' -f2 | head -1
};



#preconfigures LaTeX + Imagamagick (set debconf settings + add package)
preconfigLaTeX()
{
	cat >> /tmp/debconf-setting << "EOFDEBCONFSETTINGS"
tetex-bin tetex-bin/hyphen multiselect french[=patois], german[=austrian-alte_Rechtschreibung]
tetex-base tetex-base/olddat boolean true
tetex-bin tetex-bin/oldcfg boolean true
tetex-bin tetex-bin/upd_map boolean true
tetex-bin tetex-bin/fmtutil boolean true
tetex-bin tetex-bin/use_debconf boolean true
tetex-bin tetex-bin/groupname string users
tetex-bin tetex-bin/userperm boolean false
tetex-bin tetex-bin/groupperm boolean true
tetex-bin tetex-bin/lsr-perms boolean true
EOFDEBCONFSETTINGS

	echo -n " tetex-base tetex-bin tetex-extra imagemagick " >> /m23installer.packages
}



#preconfigures BackupPC (set debconf settings + add package)
preconfigBackupPC()
{
	cat >> /tmp/debconf-setting << "EOFDEBCONFSETTINGS"
backuppc backuppc/tmppass password
backuppc backuppc/add-lines boolean true
EOFDEBCONFSETTINGS

	echo -n " backuppc " >> /m23installer.packages
}



#configures BackupPC
configBackupPC()
{
	#use the same password file as the m23 administration interface
	sed 's/[^#]AuthUserFile/#AuthUserFile/g' /etc/backuppc/apache.conf | awk '/AuthType/ {print("\tAuthUserFile /m23/etc/.htpasswd")}
{print $0}'> /tmp/apache.conf

	mv /tmp/apache.conf /etc/backuppc/apache.conf
	chown root.root /etc/backuppc/apache.conf
	chmod 644 /etc/backuppc/apache.conf
	
	#add all admins of the m23 administration interface to the list of the BackupPC admins
	for admin in `cut -d':' -f1 /m23/etc/.htpasswd`
do
	echo "<?
include(\"/m23/inc/backup.php\");
include(\"/m23/inc/edit.php\");
include(\"/m23/inc/server.php\");
include(\"/m23/inc/db.php\");
BACKUP_addAdmin(\"$admin\");
?>" | php
done

	/etc/init.d/backuppc restart
}



#preconfigures DVD/CD burn tools (add packages)
preconfigBurn()
{
	echo -n " dvd+rw-tools mkisofs cdrdao cdrecord " >> /m23installer.packages
}




#check if MySQL is activated in php.ini and activate it, if not
configurePHP()
{
	showMessage "$I18N_Configure PHP"

	#activate MySQL support in php.ini
	if test `grep -c "^extension=mysql.so" /etc/php4/apache/php.ini` -eq 0
	then
		echo "extension=mysql.so" >> /etc/php4/apache/php.ini
	fi



	#activate register_globals
	sed 's/register_globals = Off/register_globals = On/g' /etc/php4/apache/php.ini > /tmp/php.ini

	mv /tmp/php.ini /etc/php4/apache/php.ini
	chown root.root /etc/php4/apache/php.ini
	chmod 644 /etc/php4/apache/php.ini
}



#configure Apache
configureApache()
{
	#set DocumentRoot to /m23/data+scripts
	#set index.php as index file
	#set php is now a interpreted extension
	showMessage "$I18N_Configure Apache"
sed 's/DocumentRoot \/var\/www/DocumentRoot \/m23\/data+scripts/' /etc/apache/httpd.conf | sed 's/<Directory \/var\/www\/>/<Directory \/m23\/data+scripts>/' | sed 's/  DirectoryIndex/  DirectoryIndex index.php/' | sed  's/#AddType application\/x-httpd-php .php/AddType application\/x-httpd-php .php/' | sed 's/# LoadModule php4_module \/usr\/lib\/apache\/1.3\/libphp4.so/LoadModule php4_module \/usr\/lib\/apache\/1.3\/libphp4.so/' | sed 's/AllowOverride None/AllowOverride AuthConfig/g' > /tmp/httpd.conf
	mv /tmp/httpd.conf /etc/apache/
	chown root.root /etc/apache/httpd.conf
	chmod 644 /etc/apache/httpd.conf
	
	/etc/init.d/apache stop
	/etc/init.d/apache start
}



#configure sudo
configureSudo()
{
	showMessage "$I18N_Configure sudo"
	if test `grep -c "^www-data          ALL=NOPASSWD:ALL" /etc/sudoers` -eq 0
	then
		echo "www-data          ALL=NOPASSWD:ALL" >> /etc/sudoers
	fi
}



#configure inetd.conf for tftpd
configureTFTP()
{
	sed 's/^tftp/# tftp/g' /etc/inetd.conf > /tmp/inetd.conf
	echo "tftp dgram udp wait root /usr/sbin/in.tftpd /usr/sbin/in.tftpd -s /m23/tftp -r blksize" >> /tmp/inetd.conf
	mv /tmp/inetd.conf /etc
	chown root.root /etc/inetd.conf
	chmod 644 /etc/inetd.conf
	echo '#Defaults for tftpd-hpa
RUN_DAEMON="yes"
OPTIONS="-l -s /m23/tftp"
' > /etc/default/tftpd-hpa
	/etc/init.d/inetd restart
}



#delete downloaded files
cleanUpDownloaded()
{
	cd /
	rm $downloadFiles /m23InstallerConfigure.inc /m23Installer.conf /installI18N /m23install.err /tmp/m23Installer.configure /m23installer.packages 2> /dev/null
}



#configure choosen 
configureChoosen()
{
	#try to run preconfigure functions
	for option in `cat /tmp/m23Installer.configure`
	do
		option=`echo $option | cut -d'"' -f2`
		#pipe error messages to file
		preconfig$option 2> /m23install.err
		#if not "command not fould = 127"
		if test $? -ne 127
		then
			#show the error messages on stderr
			cat /m23install.err > /dev/stderr
		fi
	done
	
	installPackages
	extractBaseFiles

	#try to run configure functions
	for option in `cat /tmp/m23Installer.configure`
	do
		option=`echo $option | cut -d'"' -f2`
		configure$option 2> /m23install.err
		if test $? -ne 127
		then
			cat /m23install.err > /dev/stderr
		fi
	done

	/m23/bin/fix-permissions
	/etc/init.d/apache start
}