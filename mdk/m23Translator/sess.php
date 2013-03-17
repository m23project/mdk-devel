<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for session management on load balanced multi computer environments.
$*/





/**
**name implodeAssoc($glue,$arr)
**description makes a string from an assiciative array
**parameter glue: the string to glue the parts of the array with
**parameter arr: array to implode
**/
function implodeAssoc($glue,$arr)
{
	$keys=array_keys($arr);
	$values=array_values($arr);

	return(implode($glue,$keys).$glue.implode($glue,$values));
};





/**
**name explodeAssoc($glue,$str)
**description makes an associative array from a string
**parameter glue: the string to glue the parts of the array with
**parameter arr: array to explode
**/
function explodeAssoc($glue,$str)
{
	$arr=explode($glue,$str);
	
	$size=count($arr);

	for ($i=0; $i < $size/2; $i++)
		$out[$arr[$i]]=$arr[$i+($size/2)];

	return($out);
};





/**
**name SESS_get()
**description Returns the value of the session identified by the session ID of the client.
**returns string of the session value.
**/
function SESS_get()
{
	include("config.php");
	
	$sessID=SESS_getID(true);
	if (empty($sessID))
		SESS_end();

	$sql="SELECT value, time FROM `".tablePrefix."sessions` WHERE sessid=\"".SESS_getID(true)."\"";
	$res=DB_query($sql);
	
	if (mysql_num_rows($res) > 0)
		{
			$row = mysql_fetch_assoc($res);
			
			//delete session if it is older than two hours
			if ((time() - $row["time"]) > 7200)
				SESS_end();
			else
				return($row[value]);
		}
	else
		return("");
};





/**
**name SESS_end()
**description Deletes the session entry for the client ID.
**/
function SESS_end()
{
	include("config.php");
	$sql="DELETE FROM `".tablePrefix."sessions` WHERE sessid=\"".SESS_getID(true)."\"";
	DB_query($sql);
}





/**
**name SESS_load()
**description Returns the session values or starts a new session.
**returns associative array with the session values
**/
function SESS_load()
{
	include("config.php");
	//try to get the value for the running session
	$sessStr=SESS_get();

	if (empty($sessStr))
		{
			//add a new session entry
			$sql="INSERT INTO `".tablePrefix."sessions` (`sessid` , `value` , `time` ) VALUES ( '".SESS_getID(true)."', 'x', '".time()."');";
			DB_query($sql);
			return(array());
		}
	else
		return(unserialize(stripslashes($sessStr)));
}





/**
**name SESS_save($sess)
**description Saves the session array.
**parameter sess: assoziative array with the session values.
**/
function SESS_save($sess)
{
	$sessStr=mysql_real_escape_string(serialize($sess));
	include("config.php");
	$sql="UPDATE `".tablePrefix."sessions` SET `value` = '$sessStr', `time` = '".time()."' WHERE sessid=\"".SESS_getID(true)."\";";
	DB_query($sql);
};





/**
**name SESS_start()
**description Starts a new session and returns its ID or returns the ID of the current session.
**returns session ID
**/
function SESS_start()
{
	SESS_cleanOld();
	$sessID=SESS_getID(true);
	if (empty($sessID))
		$sessID=md5(uniqid(mt_rand(), true));

	setcookie ( "SESS_ID", $sessID, time()+60*60*2);
	return($sessID);
}





/**
**name SESS_getID($forceShowCookie=false)
**description Returns the session ID from cookie, globals, post or get variable.
**parameter forceShowCookie: set to true, if the ID should be returend even if it was got from a cookie
**returns session ID
**/
function SESS_getID($forceShowCookie=false)
{
	if (isset($_COOKIE["SESS_ID"]))
		if (!$forceShowCookie)
			return("");

	if (isset($GLOBALS["SESS_ID"]))
		return($GLOBALS["SESS_ID"]);
	if (isset($_COOKIE["SESS_ID"]))
		return($_COOKIE["SESS_ID"]);
	if (isset($_POST[SESS_ID]))
		return($_POST[SESS_ID]);
	return("");
};





/**
**name SESS_getIDUrl()
**description Returns a string with session ID and variable name that can be used in an URL or as hidden parameter.
**returns string with session ID and variable name. E.g. SESS_ID=6a4237826ba6d375f3b7d66ae54c2d6
**/
function SESS_getIDUrl()
{
	$sessID = SESS_getID(false);
	
	if (empty($sessID))
		return("");
	else
		return("SESS_ID=$sessID");
};





/**
**name SESS_cleanOld()
**description Deletes session entries older than 2 hours.
**/
function SESS_cleanOld()
{
	include("config.php");
	$sql="DELETE FROM `".tablePrefix."sessions` WHERE `time` < ".(time()-7200);
	DB_query($sql);
};

?>