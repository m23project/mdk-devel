#!/usr/bin/php
<?php

include('/m23/inc/db.php');
include('/m23/inc/vm.php');
include('/m23/inc/helper.php');
include("/m23/inc/i18n.php");
include('/m23/inc/client.php');
include('/m23/inc/checks.php');
include('/m23/inc/autoTest.php');
include('/m23/inc/CAutoTest.php');
include('/m23/inc/CMessageManager.php');
include('/m23/inc/CChecks.php');
include('/m23/inc/CSystemProxy.php');
include('/m23/inc/server.php');
include('/m23/inc/sourceslist.php');
include("/m23/inc/capture.php");

dbConnect();


define('SV_UCS_SCR', '1m23server-auf-UCS-installieren.m23test');
define('SV_DEB_SCR', '1m23server-auf-debian-installieren.m23test');
define('SV_ISO_SCR', '1m23server-iso-install.m23test');
define('SV_NONE_SCR', false);
define('CL_DISTR_INST_SCR', '1m23client-distro-install.m23test');

define('M23SERVER_SSH_PASSWORD', 'test');
define('M23SERVER_ISO', '/crypto/iso/m23server_19.1_rock-devel.iso');




class CATSG
{
	private $servers = array(),
			$clients = array(),
			$desktopPickAmount = 2,
			$clientArch = 'amd64',
			$webinterfaceLangArray = array(),
			$webinterfaceLangActive = 0;








/**
**name CATSG::nextWebinterfaceLang()
**description Sets the language of the m23 webinterface to the next (or first) available language in the language array.
**/
	private function nextWebinterfaceLang()
	{
		$this->webinterfaceLangActive ++;

		if (!isset($this->webinterfaceLangArray[$this->webinterfaceLangActive]))
			$this->webinterfaceLangActive = 0;
	}





/**
**name CATSG::getWebinterfaceLang()
**description Gets the language of the m23 webinterface.
**/
	private function getWebinterfaceLang()
	{
		return($this->webinterfaceLangArray[$this->webinterfaceLangActive]);
	}





/**
**name CATSG::getEnvironmentWebinterfaceLang()
**description Returnes command line environment variable to set the language of the m23 webinterface.
**returns Command line environment to set the language of the m23 webinterface.
**/
	private function getEnvironmentWebinterfaceLang()
	{
		return("AT_WEBLANG='".$this->getWebinterfaceLang()."'");
	}





/**
**name CATSG::initWebinterfaceLangArray()
**description Generates an array with information about the languages of the m23 webinterface.
**returns Array with information about the languages of the m23 webinterface.
**/
	private function initLangArray()
	{
		$i = 0;
	
		foreach (I18N_getAllCachedLanguages(true) as $sl => $ll)
			$this->webinterfaceLangArray[$i++] = $sl;
	}





/**
**name CATSG::initServerArray()
**description Generates an array with information about m23 server targets.
**returns Array with information about m23 server targets.
**/
	private function initServerArray()
	{
		$i = 0;
		
		// UCS VMs with snapshots
		$this->servers[$i]['name'] = 'UCS 4.3 - Lokales Testrepo';
		$this->servers[$i]['ip'] = '192.168.1.143';
		$this->servers[$i++]['scr'] = SV_UCS_SCR;
		
		$this->servers[$i]['name'] = 'UCS 4.4 - Lokales Testrepo';
		$this->servers[$i]['ip'] = '192.168.1.144';
		$this->servers[$i++]['scr'] = SV_UCS_SCR;
		
		// Debian VMs with snapshots
		$this->servers[$i]['name'] = 'Debian9-amd64';
		$this->servers[$i]['ip'] = '192.168.1.96';
		$this->servers[$i++]['scr'] = SV_DEB_SCR;
		
		$this->servers[$i]['name'] = 'Debian9-i386';
		$this->servers[$i]['ip'] = '192.168.1.93';
		$this->servers[$i++]['scr'] = SV_DEB_SCR;
		
		// m23 server installation ISO
		$this->servers[$i]['name'] = 'atISOm23Server';
		$this->servers[$i]['ip'] = '192.168.1.24';
		$this->servers[$i]['iso'] = '/crypto/iso/m23server_19.1_rock-devel.iso';
		$this->servers[$i++]['scr'] = SV_ISO_SCR;

		// local m23 server
		$this->servers[$i]['name'] = 'local';
		$this->servers[$i]['ip'] = false;
		$this->servers[$i++]['scr'] = SV_NONE_SCR;
	}





/**
**name CATSG::nextClientsArray()
**description Generates an array with information about client distributions including available desktops.
**returns Array with information about client distributions including available desktops.
**/
	private function nextClientsArray()
	{
		$i = 0;
		
		$sourceNamesA = SRCLST_getExportedListNames();
		shuffle($sourceNamesA);
		
		foreach ($sourceNamesA as $sourceName)
		{
			// Filter out all halfSister distributions and elementary OS
			if ((strpos($sourceName, 'imaging') === 0) ||
				(strpos($sourceName, 'HS-') === 0) ||
				(strpos($sourceName, 'devuan') === 0) ||
				(strpos($sourceName, 'elementary') === 0))
				continue;

			$d = 0;
			$this->clients[$i]['name'] = $sourceName;
			$this->clients[$i]['desktops'] = array();
			$this->clients[$i]['scr'] = CL_DISTR_INST_SCR;
		
			foreach (SRCLST_getDesktopList($sourceName) as $desktop)
			{
			
				if (('Textmode' == $desktop) || ('X' == $desktop) || ('' == $desktop)) continue;

// 				print("desktop: $desktop\n");

				$this->clients[$i]['desktops'][$d++] = $desktop;
			}
			
			$i++;
		}
	}





/**
**name CATSG::pickDesktops($client)
**description Randomly picks desktops from the list of desktops that are available in the distribution.
**parameter client: Array with client distribution information
**returns Array with randomly picked desktops from the list of desktops that are available in the distribution.
**/
	private function pickDesktops($client)
	{
		$out = array();
	
		// Make a copy of the desktops array to shuffle the entries
		$desktopsA = $client['desktops'];
		shuffle($desktopsA);

		// Adjust the amount of entries to pick to the amount of available entries
		$pickAmount = count($client['desktops']) < $this->desktopPickAmount ? count($client['desktops']) : $this->desktopPickAmount;
		
		return(array_chunk($desktopsA, $pickAmount)[0]);
	}





/**
**name CATSG::simplifyNames($in, $removeDigits)
**description Simplifies server, sourceslist and desktop names.
**parameter in: Input string.
**parameter removeDigits: Set to true, if digits (with th exception that "3D" should not be changed) should be removed.
**returns Simplified input string.
**/
	private function simplifyNames($in, $removeDigits)
	{
		// Define replacement rules
		$fromToA = array('-' => '', ' ' => '', '.' => '', 'Ubuntu' => '', 'Debian' => 'deb', 'LinuxMint' => 'LM', 'Mint' => '', 'atISOm23Server' => 'iso', 'amd64' => 'x64', 'i386' => 'x32');

		// Use all replacement rules
		foreach ($fromToA as $from => $to)
			$in = str_replace($from, $to, $in);

		// Optionally remove all digits
		if ($removeDigits)
		{
			// Save the string "3D" from losing the "3"
			$in = str_replace('3D', '#D', $in);
			$in = preg_replace('/[0-9]*/', '', $in);
			$in = str_replace('#D', '3D', $in);
		}

		return($in);
	}





/**
**name CATSG::getVMName($server, $sourceName, $desktop)
**description Generates a (unique) name for the VM.
**parameter server: Name of the m23 server.
**parameter sourceName: Name of the sources list.
**parameter desktop: Name of the desktop.
**returns VM name.
**/
	private function getVMName($server, $sourceName, $desktop)
	{
		$server = $this->simplifyNames($server, false);
		$sourceName = $this->simplifyNames($sourceName, false);
		$desktop = $this->simplifyNames($desktop, true);
	
		return($this->simplifyNames("aT$server-$sourceName$desktop".$this->clientArch.$this->getWebinterfaceLang(), false));
	}





/**
**name CATSG::getFileName($serverName, $ext)
**description Generic filename generator.
**parameter serverName: Name of the m23 server.
**parameter ext: File name extension.
**returns File name with extension.
**/
	private function getFileName($serverName, $ext)
	{
		$serverName = str_replace(' ', '', $serverName);
		return("aTS-$serverName.$ext");
	}





/**
**name CATSG::getLogFile($serverName)
**description Get the log filename.
**parameter serverName: Name of the m23 server.
**returns Log filename.
**/
	private function getLogFile($serverName)
	{
		return($this->getFileName($serverName, 'log'));
	}





/**
**name CATSG::getStopFile($serverName)
**description Get the name of the stop file.
**parameter serverName: Name of the m23 server.
**returns Name of the stop file.
**/
	private function getStopFile($serverName)
	{
		return($this->getFileName($serverName, 'stop'));
	}





/**
**name CATSG::getBashFile($serverName)
**description Get the name of the BASH file that contains the full test set for an m23 server.
**parameter serverName: Name of the m23 server.
**returns BASH filename.
**/
	private function getBashFile($serverName)
	{
		return($this->getFileName($serverName, 'sh'));
	}





/**
**name CATSG::log($bash, $serverName, $exit = false)
**description Logs BASH code with the resulting exit and (optionally) exits when a failure occurs.
**parameter bash: BASH code to log.
**parameter serverName: Name of the m23 server.
**parameter exit: If set to true, the BASH script will exit when a failure exit is emitted.
**returns BASH filename.
**/
	private function log($bash, $serverName, $heading, $exit = false)
	{
// 		print("#1$bash#2\n");

		$pad = str_repeat('#',strlen("### $heading"));
		
		$heading = "$pad\n### $heading\n$pad\n";
	
		if ($exit)
			$exit = 'exit $ret; ';
		else
			$exit = ' ';
	
		return("$heading$bash\n".'ret=$?; date +"%Y_%m_%d-%H_%M" >> '.$this->getLogFile($serverName).'; echo -n "'.$bash.' => " >> '.$this->getLogFile($serverName).';if [ $ret -ne 0 ]; then echo FAIL >> '.$this->getLogFile($serverName).';'.$exit.'else echo OK >> '.$this->getLogFile($serverName).'; fi
			');
	}





/**
**name CATSG::toggleClientArch()
**description Toggles the client's architecture from amd64 to i386 and reverse.
**/
	private function toggleClientArch()
	{
		if ($this->clientArch == 'amd64')
			$this->clientArch = 'i386';
		else
			$this->clientArch = 'amd64';
	}





/**
**name CATSG::getClientArchEnvironmentVariable()
**description Returnes command line environment variable to create an i386 client or nothing, if the client's architecture should be amd64.
**returns Command line environment variable to create an i386 client or nothing, if the client's architecture should be amd64.
**/
	private function getClientArchEnvironmentVariable()
	{
		if ($this->clientArch == 'i386')
			return("AT_CLARCH='i386'");
		else
			return('');
	}





/**
**name CATSG::generate()
**description Generates BASH scripts (seperated by m23 serverss) to run the tests for different distributions and desktops.
**/
	public function generate()
	{
		// Run thru all m23 server profiles
		foreach ($this->servers as $server)
		{
			$this->nextClientsArray();

			$logFile = $this->getLogFile($server['name']);
			$stopFile = $this->getStopFile($server['name']);

			$bashFile = $this->getBashFile($server['name']);
			echo("$bashFile\n");
			
			$localm23 = false;

			// Header of the BASH file
			$allBash = "#!/bin/bash\nLC_ALL=C\n#AT_debug=1\n";

			// BASH code for restoring or ISO-installing the m23 server
			switch($server['scr'])
			{
				case SV_ISO_SCR:
					$bash = "./autoTest.php $server[scr] '$server[name]' ".M23SERVER_ISO." $server[ip] ";
					break;
				case SV_NONE_SCR:
					$bash = false;
					$localm23 = true;
					break;
				default:
					$bash = "./autoTest.php $server[scr] '$server[name]' $server[ip] ";
			}

			// Skip creation of server VM, if the local m23 server is used
			if ($bash != false)
				$allBash .= $this->log($bash, $server['name'], $server['name'], true);

			// Run thru the sources lists and clients desktops
			foreach ($this->clients as $client)
			{
				foreach ($this->pickDesktops($client) as $desktop)
				{
					$vmName = $this->getVMName($server['name'], $client['name'], $desktop);

					// Use credentials if a m23 server should be used that is not located on the local machine
					if ($localm23)
						$serverCredentials = $this->getEnvironmentWebinterfaceLang().' '.$this->getClientArchEnvironmentVariable();
					else
						$serverCredentials = $this->getEnvironmentWebinterfaceLang().' '.$this->getClientArchEnvironmentVariable()." AT_M23_SSH_PASSWORD='".M23SERVER_SSH_PASSWORD."' TEST_M23_BASE_URL='https://god:m23@$server[ip]/m23admin'";

					$bash = "$serverCredentials ./autoTest.php '$client[scr]' '$vmName' '$client[name]' '$desktop'";

					$allBash .= "\n\nif [ $(grep '$vmName' '$logFile' | grep -c 'OK$') -eq 0 ]
then
".$this->log($bash, $server['name'], $vmName)."
fi
if [ -f '$stopFile' ]
then
	exit 0
fi
\n";

					$this->toggleClientArch();
					$this->nextWebinterfaceLang();
				}
			}

			// Write script file and make it executable
			file_put_contents($bashFile, $allBash);
			chmod($bashFile, 500);
		}
	}
	
	
	public function __construct()
	{
		$this->initLangArray();
		$this->initServerArray();
	}
}


$AutoTestScriptGeneratorO = new CATSG();
$AutoTestScriptGeneratorO->generate();
?>