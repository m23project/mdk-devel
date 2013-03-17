#!/usr/bin/php
<?php

if ($argc < 2)
{
	echo("Usage $argv[0] <random key>");
	exit(1);
}


$randomKey = $argv[1];

require('/tmp/accountData.php');
include('/m23/inc/mail.php');

//Calculate the encryption key
$key = MAIL_getKeyFromeMailAddress($randomKey);

//Build an MD5 sum over important values
$accountData['md5'] = md5($accountData['afclient'].$accountData['m23adminPass'].$accountData['sshKey'].$accountData['accountName']);

//Compress and encrypt the data
$msg = MAIL_AESencode($key,gzcompress(serialize($accountData),9));

//Make it fit into an eMail
$out = wordwrap(base64_encode($msg),75,"\n",true);

//Show the block
echo("####### m23 remote administration service - Account information begin #####

$out

####### m23 remote administration service - Account information end #######
");

?>