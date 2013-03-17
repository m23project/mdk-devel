<?PHP
	include('/m23/inc/db.php');
	include('/m23/inc/fdisk.php');
	include('/m23/inc/remotevar.php');
	include('/m23/inc/client.php');
	include('/m23/inc/sourceslist.php');
	include_once('/m23/inc/messageReceive.php');
	include('/m23/inc/packages.php');
	include_once('/m23/inc/dhcp.php');
	include_once('/m23/inc/capture.php');
	include_once('/m23/inc/helper.php');
	include_once('/m23/inc/update.php');
	include_once('/m23/inc/edit.php');
	include_once('/m23/inc/ldap.php');
	include_once('/m23/inc/assimilate.php');
	include_once('/m23/inc/i18n.php');
	include_once('/m23/inc/imaging.php');
	include_once('/m23/inc/server.php');
	include_once('/m23/inc/raidlvm.php');
	include_once('/m23/inc/vm.php');
	include_once('/m23/inc/checks.php');

	dbConnect();

//***********************************
//* Enter code between these blocks *
//***********************************

	print("VM_activateNetboot: ".VM_activateNetboot("m23vmclient", true));

//***********************************
//* Enter code between these blocks *
//***********************************
?>