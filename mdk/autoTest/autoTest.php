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




if (!isset($argv[1]) || !file_exists($argv[1]))
	die("$argv[0] <Test description file (.m23test)> <parameters>\n");

$xmlFile = $argv[1];
array_shift($argv);
$AutoTestO = new CAutoTest($xmlFile, $argv);
global $AutoTestO;

$AutoTestO->run();
?>