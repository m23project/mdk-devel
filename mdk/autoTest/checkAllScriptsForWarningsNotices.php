#!/usr/bin/php
<?php

include('/m23/inc/db.php');
include('/m23/inc/vm.php');
include('/m23/inc/helper.php');
include("/m23/inc/i18n.php");
include("/m23/inc/html.php");
include('/m23/inc/client.php');
include('/m23/inc/checks.php');
include_once('/m23/inc/capture.php');
include('/m23/inc/server.php');
include('/m23/inc/packages.php');
include_once('/m23/inc/CMessageManager.php');
include_once('/m23/inc/CChecks.php');
include_once('/m23/inc/CClient.php');
include_once('/m23/inc/CFDiskIO.php');
include_once('/m23/inc/CFDiskBasic.php');
include_once('/m23/inc/CSystemProxy.php');

$GLOBALS["m23_language"] = 'en';

dbConnect();

$logFile = '/tmp/checkAllScriptsForWarningsNotices.log';

$client = 'scriptcheckclient';

$clientO = new CClient($client, CClient::CHECKMODE_MUSTEXIST);

// CLIENT_clone($src, $dst)
// distr beim Client wechseln



$result = DB_query("SELECT DISTINCT `package`, `params` FROM `clientjobs` ORDER BY package, params");
while ($data = mysqli_fetch_array($result))
{
	DB_query("DELETE FROM clientjobs WHERE client = '$client';");

	PKG_addJob($client, $data['package'], 0 ,$data['params']);

	$errorLines = HELPER_grep($clientO->getClientCurrentWorkPHP(), '</b>');
	print($data['package'].' => '.strlen($errorLines)."\n");

	file_put_contents($logFile, $errorLines, FILE_APPEND);
}

?>

sort -u /tmp/checkAllScriptsForWarningsNotices.log