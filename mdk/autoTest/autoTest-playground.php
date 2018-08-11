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

	$answer='|{Warte|minutes}';
	
	if (strpos($answer, '|{') === 0)
	{
		// eg. $answer = '|{Warte|minutes}';
	
		
		print_r(explode('|', $answer));
		
		print($answer);
	}


?>