#!/bin/bash

#Sets global variables and values stored in /tmp/menuVars. To use these variables put ". /tmp/menuVars" in front of your code.
#$1: name of the variable
#$2: value of the variable
menuVarAdd()
{
	varName=$1
	varValue=$2
	sed "/^$varName=/d" /tmp/menuVars > /tmp/menuVars.tmp
	echo "$varName=$varValue" >> /tmp/menuVars.tmp
	mv /tmp/menuVars.tmp /tmp/menuVars
	chmod 666 /tmp/menuVars
}





#removes a global variable from /tmp/menuVars.
#$1: name of the variable
menuVarDel()
{
	varName=$1
	sed "/^$varName=/d" /tmp/menuVars > /tmp/menuVars.tmp
	mv /tmp/menuVars.tmp /tmp/menuVars
	chmod 666 /tmp/menuVars
}


# Resets (deletes) all variable files
menuVarReset()
{
	rm /tmp/allTexFiles /tmp/archTmp /tmp/articleList /tmp/articleMenuTmp /tmp/articlenumbers /tmp/bootCDOption /tmp/bootImageOption /tmp/dodgerFiles /tmp/err /tmp/html2latex /tmp/i18nAllFiles /tmp/i18nFiles /tmp/Installed-Size /tmp/kernelConfigMenu /tmp/kernelDirMenu /tmp/kernelDownloadm23 /tmp/kernelMenu /tmp/kernelMenuBranch /tmp/kernelMenuSelectionm23 /tmp/kernelPackage /tmp/kernelRamDisk /tmp/m23architecture /tmp/m23Combine /tmp/m23deb /tmp/m23Deb /tmp/m23diff /tmp/m23edit /tmp/m23instUpload /tmp/m23kernelBranch /tmp/m23kernelConfigDir /tmp/m23kernelDir /tmp/m23kernelListing /tmp/m23kernelOldConfigFile /tmp/m23kernelStoreDir /tmp/m23kernelType /tmp/m23SF-Upload /tmp/m23targetKernelName /tmp/m23targetModulesDir /tmp/menuDocSel /tmp/menuForkChoice /tmp/menuTranslatorSel /tmp/menuVars /tmp/missing /tmp/screenshot /tmp/startOption /tmp/tempDirs /tmp/tempFiles /tmp/tempMD5Index /tmp/updatedFilelist /tmp/version 2> /dev/null
}



menuVarInit()
{
	if ! test -f /tmp/menuVars
	then
		echo ". /mdk/globalVars" > /tmp/menuVars
		chmod 666 /tmp/menuVars
	fi
}