for LANG in `cat /tmp/m23language`
do
	echo "=========================================="
	echo "generating screenshots for language: $LANG"
	echo "=========================================="

	#set language for screenshots
	echo -n $LANG > /m23/tmp/screenshot.lang

	mkdir -p /mdk/doc/manual/screenshots/$LANG
	cd /mdk/doc/manual/screenshots/$LANG
	
	echo "Recreate (a)ll or (m)issing screenshots?"
	read am
	if [ $am = "a" ]
	then
		rm *
	fi

	wget --http-user=$M23USER --http-passwd=$M23PWD "http://127.0.0.1/m23admin/misc/prepareManual.php" -O /dev/null -o /dev/null

	#select cropping for x direction
	case "$LANG" in 
		"de" )  cropX=145 ;; 
		"en" )  cropX=100 ;;
	esac

	cp /mdk/doc/manual/screenshots/empty.png client_directConnection.png
	cp /mdk/doc/manual/screenshots/empty.png serverRestore.png

	#server
	../../bin/kh2p "page=htaccess" htaccess.png
	../../bin/kh2p "page=update" serverupdate.png
	../../bin/kh2p "page=server_daemonsAndPrograms" daemonsAndPrograms.png
	../../bin/kh2p "page=ldapSettings" ldapSettings.png
	../../bin/kh2p "page=manageImageFiles" manageImageFiles.png
	../../bin/kh2p "page=manageGPGKeys" manageGPGKeysDialog.png #new 10.1
	../../bin/kh2p "page=serverBackup" serverBackup.png #new 10.1
	../../bin/kh2p "page=serverBackupList" serverBackupList.png #new 10.1
	../../bin/kh2p "page=downloadVBoxAddons" downloadVBoxAddons.png #new 10.3

	#clients
	../../bin/kh2p "page=clientsoverview" clients_overview.png
	
		#control center
		../../bin/kh2p "page=clientdetails&client=m23demoClNt3&id=156" clientdetails.png
			../../bin/kh2p "page=clientinfo&client=m23demoClNt3&id=156&infoType=hardware" clientinfo_hardware.png
			../../bin/kh2p "ED_search=mozilla&BUT_search=Suchen&page=clientpackages&firstrun=muh&HID_key=mozilla&client=m23demoClNt3&CB_count=3" clients_packages.png
			../../bin/kh2p "page=packageBuilder" packageBuilder.png
			../../bin/kh2p "page=clientinfo&client=m23demoClNt3&id=156&infoType=clientLog" clientinfo_clientLog.png
			../../bin/kh2p "page=clientinfo&client=m23demoClNt2&id=155&infoType=addToGroup" clientinfo_addToGroup.png
			../../bin/kh2p "page=clientinfo&client=m23demoClNt3&id=156&infoType=delFromGroup" clientinfo_delFromGroup.png
			../../bin/kh2p "page=recoverclient&client=m23demoClNt3&id=156" clients_recover.png
			../../bin/kh2p "page=rescueclient&deactivate=0&id=156&client=m23demoClNt3" rescue_client.png
			../../bin/kh2p "page=clientstatus&id=156&client=m23demoClNt3" client_status.png
			../../bin/kh2p "page=clientdebug&id=156&client=m23demoClNt3" client_debug.png
			../../bin/kh2p "page=backup&client=m23demoClNt3&id=156client_backup" client_backup.png
			../../bin/kh2p "page=changeJobs&client=m23demoClNt3&id=156" client_changejobs.png
			../../bin/kh2p "page=createImage&client=m23demoClNt&id=154" client_createImage.png
			../../bin/kh2p "page=editclient&client=m23demoClNt3&id=156" edit_client.png
	
	../../bin/kh2p "page=addclient" client_add.png
	cp /mdk/doc/manual/screenshots/empty.png externalDHCP.png
	../../bin/kh2p "page=assimilate" client_assimilate.png
	../../bin/kh2p "page=clientsoverview&action=setup" clients_setup.png
		#client setup and fdisk pages
		../../bin/kh2p "page=fdisk&client=m23demoClNt" fdisk.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=1" fdisk-automatic.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=2" fdisk-existing.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=3" fdisk-extended0.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=4" fdisk-extended1.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=5" fdisk-extended2.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=6" fdisk-extended3.png
		../../bin/kh2p "page=fdisk&client=m23demoClNt&CAPstep=10" RAID_add.png
		
		#select distribution kernel
		../../bin/kh2p "page=clientdistr&client=m23demoClNt&CAPstep=2" client_distr.png
		
	../../bin/kh2p "page=clientsoverview&action=delete" clients_delete.png

	../../bin/kh2p "page=addvmclient&screenshot=1" createVM.png

	#groups
	../../bin/kh2p "page=groupsoverview" groups_overview.png
	../../bin/kh2p "page=creategroup" create_group.png

	#packages
	../../bin/kh2p "page=installpackages&client=m23demoClNt3&id=156&CAPstep=0" install_packages.png
	../../bin/kh2p "page=installpackages&client=m23demoClNt3&id=156&CAPstep=1&action=deinstall" deinstall_packages.png
	../../bin/kh2p "page=installpackages&client=m23demoClNt3&id=156&action=update&CAPstep=2" update_packages.png
	../../bin/kh2p "page=installpackages&CAPstep=3&action=packageSelection" editPackageSelection.png
	../../bin/kh2p "page=clientsourceslist" client_sourceslist.png
	../../bin/kh2p "page=scriptEditor&screenshot=yes" scriptEditor.png
	../../bin/kh2p "page=poolBuilder&CAPstep=1" poolBuilderCreateEditDelete.png
	../../bin/kh2p "page=poolBuilder&CAPstep=2" poolBuilderCreateIndex.png
	../../bin/kh2p "page=poolBuilder&CAPstep=3" poolBuilderStartDownload.png
	../../bin/kh2p "page=poolBuilder&CAPstep=4" poolBuilderDownloadStatus.png
	../../bin/kh2p "page=poolBuilder&CAPstep=12" poolBuilderReadCD.png
	../../bin/kh2p "page=poolBuilder&CAPstep=5" poolBuilderShowSourcesList.png
	../../bin/kh2p "page=poolBuilder&CAPstep=11" poolBuilderSelectPackageSourcesAndPackages.png


	#mass tools
	../../bin/kh2p "page=addclient&CAPstep=1" clientBuilder.png
	../../bin/kh2p "page=addclient&CAPstep=2" diskDefine.png
	../../bin/kh2p "page=massInstall&CAPstep=0" mi_step0.png
	../../bin/kh2p "page=massInstall&CAPstep=1" mi_step1.png
	../../bin/kh2p "page=massInstall&CAPstep=2" mi_step2.png
	../../bin/kh2p "page=massInstall&CAPstep=3" mi_step3.png
	../../bin/kh2p "page=massInstall&CAPstep=4" mi_step4.png
	
	#tools
	../../bin/kh2p "page=makeBootDisk" makeBootDisk.png
	../../bin/kh2p "page=makeBootCD" makeBootCD.png
	
	#plugins
	../../bin/kh2p "page=plgoverview" plgoverview.png
	../../bin/kh2p "page=plginstall" plginstall.png
	
	rm /m23/tmp/screenshot.lang
done

#/mdk/doc/manual/bin/restoreCurrentDB
