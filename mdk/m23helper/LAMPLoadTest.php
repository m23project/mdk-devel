<?php

/*
	Load test for LAMP servers. Designed for XAMPP.
	Put the script into /opt/lampp/htdocs/xampp and call it with https://<IP>/xampp/stress.php
	E.g.: siege https://<IP>/xampp/LAMPLoadTest.php -l/tmp/siege.log
	Copyright (C) 2012 Hauke Goos-Habermann

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program; see the file COPYING.  If not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/





/**
**name dbConnect()
**description connects to the m23 database
**/
function dbConnect()
{
	$dbConnection=mysql_pconnect("localhost","root","")
		or die ("Could not connect to database server!");

	DB_query('CREATE DATABASE IF NOT EXISTS `hwpartner` ;');

	mysql_select_db("hwpartner",$dbConnection)
			or die ("Could not select database!");
};






/**
**name DB_query($sql)
**description makes a query and returns the default error message if an error occurs
**parameter sql: sql query
**/
function DB_query($sql)
{
	$result = mysql_query($sql)
		or die ("DB_query: Could not execute SQL statement: $sql ERROR:".mysql_error());
	return($result);
}





/**
**name newTab($tab)
**description Creates a new DB table.
**parameter tab: Name of the table to create.
**/
function newTab($tab)
{
	DB_query("CREATE TABLE `hwpartner`.`$tab` (
`id` BIGINT NOT NULL AUTO_INCREMENT ,
`blob` LONGTEXT NOT NULL ,
`md5` LONGTEXT NOT NULL ,
PRIMARY KEY ( `id` )
) ENGINE = InnoDB;");
}





/**
**name writeAndCheck($tab)
**description Writes random data to the DB and a file and checks the integrity.
**parameter tab: Name of the table.
**/
function writeAndCheck($tab)
{
	$fName = "/tmp/$tab.blob";

	//Generate the random data
	echo("<h1>Filling the table \"$tab\"</h1>");

	$blob = openssl_random_pseudo_bytes(131072);
	echo("Random data generated: ".strlen($blob)." bytes<br>");
	
	$blobDB = base64_encode($blob);
	echo("Random data converted to base 64<br>");
	
	$md5 = md5($blob);
	echo("MD5 sum generated<br>");

	//Write it to a file
	file_put_contents($fName,$blob);
	echo("Random data file written<br>");

	//Write it into the DB
	DB_query("INSERT INTO `hwpartner`.`$tab` (`blob`, `md5`) VALUES ('$blobDB','$md5');");
	echo("Random data inserted to DB<br>");

	//Get it back from the DB
	$fblob = file_get_contents($fName);
	echo("Got random data from file<br>");

	$res = DB_query("SELECT * FROM `$tab` WHERE `md5` = '$md5';");
	$dblobA = mysql_fetch_row($res);
	echo("Got random data from DB<br>");

	$dblob = base64_decode($dblobA[1]);
	echo("Base 64 data converted back<br>");

	echo("Checking MD5 of DB: ");
	if (md5($dblob) == $md5)
		echo("<b>ok</b><br>");
	else
		echo("<b>failed</b><br>");

	echo("Checking MD5 of file: ");
	if (md5($fblob) == $md5)
		echo("<b>ok</b><br>");
	else
		echo("<b>failed</b><br>");

	unlink($fName);
	echo("Deleted file<br>");
	
	echo($blobDB);
}





/**
**name cleanOldTables()
**description Drops old tables.
**/
function cleanOldTables()
{
	$maxAmount = 250;

	//Get the amount of tables in the DB
	$res = DB_query("SELECT count(*) from information_schema.tables WHERE table_schema = 'hwpartner';");
	$tempA = mysql_fetch_row($res);
	$tableAmount = $tempA[0];

	//If there are too many tables => delete some
	if ($tableAmount > $maxAmount)
	{
		//Figure out the amount of tables to be deleted
		$delamount = $tableAmount - $maxAmount;

		//Get the table names of the old tables
		$res = DB_query("SELECT TABLE_NAME from information_schema.tables WHERE table_schema = 'hwpartner' ORDER BY TABLE_NAME LIMIT $delamount");

		//Drop the unneded tables
		while ($tempA = mysql_fetch_row($res))
			DB_query("DROP TABLE `$tempA[0]`;");
	}
}





dbConnect();

//Generate an unique name for the table/file
$tab = 'hwp'.microtime(true);
newTab($tab);

//Write/read some data into/from the DB/file.
$amount = rand (10, 20);
for ($i = 0; $i < $amount; $i++)
	writeAndCheck($tab);
	
cleanOldTables();

?>