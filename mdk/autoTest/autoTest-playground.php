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


/*$serverNameOrIP = '192.168.1.143';
$clientNameOrIP = 'seltestXX';*/
// 
// $cmds = '
// for i in `seq 1 9`
// do
// 	echo $i $(hostname)
// done
// whoami
// 
// ';
// 
// $u = 'https://god:m23@192.168.1.143/m23admin';
// 
// $ret = preg_match('/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/', $u, $ips);
// print(serialize($ret));
// print_r($ips);

// print(AUTOTEST_sshTunnelOverServer($serverNameOrIP, $clientNameOrIP, 'cat /etc/issue'));

$AutoTestO = new CAutoTest('bla', $argv);


?>