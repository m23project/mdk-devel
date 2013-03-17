<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for connecting to a database and executing queries.
$*/





/**
**name DB_connect()
**description Connects to the data base.
**/
function DB_connect($dbHost="",$dbUser="",$dbPass="",$dbName="")
{
	if (empty($dbHost))
		{
			include("config.php");
			$dbHost=dbHost;
			$dbUser=dbUser;
			$dbPass=dbPass;
			$dbName=dbName;
		}
		

	$dbConnection=mysql_connect($dbHost,$dbUser,$dbPass);

	if (!$dbConnection)
		{
			echo("<h3>Could not connect to database server!</h3>");
			return(false);
		};


	if (!empty($dbName))
	{
		$res = mysql_select_db($dbName,$dbConnection);
		if (!$res)
		{
			echo("<h3>Could not select database!</h3>");
			return(false);
		};
	}

	return(true);
};





/**
**name DB_query($sql)
**description Executes a SQL query.
**parameter sql: SQL query string
**returns Return value of mysql_query (can be used with different functions)
**/
function DB_query($sql)
{
	$result=mysql_query($sql);
	if ($result===false)
		{
			echo("<h3>Could not execute SQL statement: $sql ERROR:".mysql_error()."</h3>");
			return(false);
		};
	return($result);
}

?>