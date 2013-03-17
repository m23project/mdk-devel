<?php

include("html.php");
include("db.php");
include("login.php");
include("sess.php");




/**
**name NEXT_finished()
**description Tries to delete setup.php.
**returns false if the old page shouldn't be redrawn.
**/
function NEXT_finished()
{
	//unlink("setup.php");
	if (file_exists("setup.php"))
		echo("<h3>Error: Could not delete setup.php. You have to delete it by hand.</h3>");
	else
		echo("<h3>setup.php was delete. Have fun!</h3>");
	$GLOBALS[_SESSION] = array();
	session_destroy();
};





/**
**name NEXT_addAdmin()
**description Adds an administrator account.
**returns false if the old page shouldn't be redrawn.
**/
function NEXT_addAdmin()
{
	include("config.php");

	DB_connect();
	LOGIN_new($_POST[ED_name],$_POST[ED_pass],"",PERM_ADMIN);
	jumpPage("finished");
	return(false);
};





/**
**name NEXT_welcome()
**description Jumps to the next page after the welcome screen.
**returns false if the old page shouldn't be redrawn.
**/
function NEXT_welcome()
{
	jumpPage("checkConfigAccess");
	return(false);
}





/**
**name NEXT_checkConfigAccess()
**description Jumps to the next page after the check for write access on config.php.
**returns false if the old page shouldn't be redrawn otherwise true.
**/
function NEXT_checkConfigAccess()
{
	if (CHECK_config() || ($_POST[CB_view]=="yes"))
		{
			if ($_POST[CB_view]=="yes")
				$GLOBALS[_SESSION][viewConfig]=true;
			jumpPage("dbSetup");
			return(false);
		}
	else
		return(true);
}





/**
**name CHECK_config()
**description Checks for write permission on config.php.
**returns true, if config.php is writable. otherwise false.
**/
function CHECK_config()
{
	clearstatcache();
	return(is_writable("config.php"));
};





/**
**name NEXT_dbSetup()
**description Checks the entered values, adds a new database, tables and edits config.php.
**returns false if the old page shouldn't be redrawn otherwise true.
**/
function NEXT_dbSetup()
{
	$error=false;
	$addDb=false;
	
	//check if all needed values are set
	if (empty($_POST[ED_DBhost])) {echo("<h3>You have to enter the database server!</h3>"); $error=true;}
	if (empty($_POST[ED_DBuser])) {echo("<h3>You have to enter the database user!</h3>"); $error=true;}
	if (empty($_POST[ED_DBpass])) {echo("<h3>You have to enter the database user password!</h3>"); $error=true;}
	if (empty($_POST[ED_DBname])) {echo("<h3>You have to enter the name of the database!</h3>"); $error=true;}

	//check settings for adding a new database
	if (!empty($_POST[ED_DBadmin]) || !empty($_POST[ED_DBadminPass]))
		{
			if (empty($_POST[ED_DBadmin])) {echo("<h3>You have to enter the name of the database administrator if you want to add a new database!</h3>"); $error=true;}
			if (empty($_POST[ED_DBadminPass])) {echo("<h3>You have to enter the password for the database administrator if you want to add a new database!</h3>"); $error=true;}
			$addDb=true;
		};

	if ($error)
		return(true);

	$createTablesSQL="
	CREATE TABLE `$_POST[ED_DBpref]directories` (
	`id` int(11) NOT NULL auto_increment,
	`name` varchar(255) NOT NULL default '',
	PRIMARY KEY  (`id`)
	) TYPE=MyISAM;
	
	CREATE TABLE `$_POST[ED_DBpref]files` (
	`id` int(11) NOT NULL auto_increment,
	`name` varchar(255) NOT NULL default '',
	`directory` int(11) NOT NULL default '0',
	PRIMARY KEY  (`id`)
	) TYPE=MyISAM;

	CREATE TABLE `$_POST[ED_DBpref]languages` (
	`id` int(11) NOT NULL auto_increment,
	`name` varchar(255) NOT NULL default '',
	`shortname` char(3) NOT NULL default '',
	PRIMARY KEY  (`id`)
	) TYPE=MyISAM;

	CREATE TABLE `$_POST[ED_DBpref]logins` (
	`id` int(11) NOT NULL auto_increment,
	`name` varchar(255) NOT NULL default '',
	`password` varchar(255) NOT NULL default '',
	`email` text NOT NULL,
	`permissions` int(11) NOT NULL default '0',
	PRIMARY KEY  (`id`)
	) TYPE=MyISAM;

	CREATE TABLE `$_POST[ED_DBpref]translations` (
	`id` int(11) NOT NULL auto_increment,
	`filenr` int(11) NOT NULL default '0',
	`derivedfrom` int(11) NOT NULL default '0',
	`editedby` int(11) NOT NULL default '0',
	`article` longtext NOT NULL,
	`status` int(11) NOT NULL default '0',
	`language` int(11) NOT NULL default '0',
	`derivedlanguage` int(11) NOT NULL default '0',
	`time` int(11) default NULL,
	PRIMARY KEY  (`id`)
	) TYPE=MyISAM;

	CREATE TABLE `$_POST[ED_DBpref]sessions` (
	`id` INT NOT NULL AUTO_INCREMENT ,
	`sessid` VARCHAR( 255 ) NOT NULL ,
	`value` LONGTEXT NOT NULL ,
	`time` INT NOT NULL ,
	PRIMARY KEY ( `id` )
	) TYPE = MYISAM ;";
	
	
	if ($addDb)
	{
		DB_connect($_POST[ED_DBhost],$_POST[ED_DBadmin],$_POST[ED_DBadminPass]);
		$res=DB_query("CREATE DATABASE `$_POST[ED_DBname]`");
		
		//if the database could be created
		if (!($res===false))
		{
			//make a connection to the new database
			DB_connect($_POST[ED_DBhost],$_POST[ED_DBadmin],$_POST[ED_DBadminPass],$_POST[ED_DBname]);
			//add tables with the administrator account
			$createTableRes=true;
			foreach (split(";",$createTablesSQL) as $sql)
				if (!empty($sql))
					if (DB_query($sql) === false)
						{
							$createTableRes=false;
							break;
						}
		};
	};

	//try to add the tables with the user account
	if (!$createTableRes)
	{
		DB_connect($_POST[ED_DBhost],$_POST[ED_DBuser],$_POST[ED_DBpass],$_POST[ED_DBname]);

		$createTableRes=true;
		foreach (split(";",$createTablesSQL) as $sql)
			if (!empty($sql))
				if (DB_query($sql) === false)
					return(true);
	};

	//make changes on config.php
	$file=fopen("config.php","r");
	$fcontent=fread($file,10000);
	fclose($file);
	
	foreach (split("\n",$fcontent) as $line)
	{
		if (strstr($line,"\$dbPass="))
			$line="\$dbPass=\"$_POST[ED_DBpass]\";";
		elseif (strstr($line,"\$dbUser="))
			$line="\$dbUser=\"$_POST[ED_DBuser]\";";
		elseif (strstr($line,"\$dbHost="))
			$line="\$dbHost=\"$_POST[ED_DBhost]\";";
		elseif (strstr($line,"\$dbName="))
			$line="\$dbName=\"$_POST[ED_DBname]\";";
		elseif (strstr($line,"\$tablePrefix="))
			$line="\$tablePrefix=\"$_POST[ED_DBpref]\";";
		$out.="$line\n";
	}

	if ($GLOBALS[_SESSION][viewConfig])
		{
			$out=str_replace("<","&lt;",$out);
			echo("<h3>Copy the following lines to your config.php</h3>
			<pre>$out</pre>");
		}
	else
		{
			$file=fopen("config.php","w");
			fwrite($file,$out);
			fclose($file);
		
			chmod("config.php",0444);
		};

	jumpPage("addAdmin");
	return(false);
};





/**
**name showPage($title,$text,$nextFkt)
**description Shows a page with title, HTML body and next button. If the button is pressed a given function is called.
**parameter title: title of the page
**parameter text: HTML body
**parameter nextFkt: function called when next button is pressed
**/
function showPage($title,$text,$nextFkt)
{
	$draw=true;

	if (isset($_POST[BUT_next]))
		{
			unset($_POST[BUT_next]);
			$draw=$nextFkt();
		};

	if (!$draw)
		return;

	HTML_showFormHeader();
	echo("<center><span class=\"title\">$title</span></center>\n");
	HTML_showTableHeader(true);
	echo("
	<tr>
		<td>$text</td>
	</tr>
	<tr>
		<td align=\"right\">
			<input type=\"submit\" name=\"BUT_next\" value=\"next >>\">
		</td>
	</tr>
	");

	HTML_showTableEnd(true);
	HTML_showFormEnd("<input type=\"hidden\" name=\"SESSION\" value=\"".urlencode(serialize($GLOBALS[_SESSION]))."\">");
};





/**
**name jumpPage($page)
**description Jumps to a given page.
**parameter page: name of the page
**/
function jumpPage($page)
{
	$GLOBALS[_SESSION][page]=$page;
	switch ($page)
		{
			case "welcome": 
			{
				showPage("Welcome to the installation of m23/Translator","<center><img src=\"gfx/logo.png\"></center><br>In the following screens you will set up the database and other settings for running m23/Translator. Click on <i>\"next\"</i> to continue.<br>",NEXT_welcome);
				break;
			}
			case "checkConfigAccess": 
			{
				showPage("m23/Translator: Check file permissions",CHECK_config() ? "File permissions on \"config.php\" are ok. Click on <i>\"next\"</i> to continue.<br>" : "Make sure the file \"config.php\" is writable by the user running your websever. E.g. you can set write access on the console with:<br><br> <code>chmod 666 config.php</code><br><br>

				Check here <INPUT type=\"checkbox\" name=\"CB_view\" value=\"yes\">if you prefer to edit config.php by hand and view the contents of the generated config.php.<br><br>
				Click on <i>\"next\"</i> if you made writable \"config.php\" or want to view the contents.<br><br>
				",NEXT_checkConfigAccess);
				break;
			}
			case "dbSetup":
			{
				showPage("m23/Translator: Database settings", "
				<table>
				<tr>
					<td colspan=\"3\">Please enter the following necessary values:</td>
				</tr>
				<tr>
					<td>Database server</td>
					<td><INPUT type=\"text\" name=\"ED_DBhost\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBhost]\"></td>
					<td>Hostname or IP of the MySQL server.</td>
				</tr>
				<tr>
					<td>Database user</td>
					<td><INPUT type=\"text\" name=\"ED_DBuser\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBuser]\"></td>
					<td>Username on the server who has INSERT, UPDATE, DELETE, SELECT, CREATE TABLE capabilities.</td>
				</tr>
				<tr>
					<td>Database password</td>
					<td><INPUT type=\"text\" name=\"ED_DBpass\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBpass]\"></td>
					<td>Password for this user.</td>
				</tr>
				<tr>
					<td>Database name</td>
					<td><INPUT type=\"text\" name=\"ED_DBname\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBname]\"></td>
					<td>Name of the database to use.</td>
				</tr>
				<tr>
					<td>Table prefix</td>
					<td><INPUT type=\"text\" name=\"ED_DBpref\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBpref]\"></td>
					<td>If you have only one database you can add a prefix before all tables of m23/Translator. So these tables don't get mixed with other existing tables.</td>
				</tr>
				<tr>
					<td colspan=\"3\"><hr></td>
				</tr>
				<tr>
					<td colspan=\"3\">You only need to enter the following values if you want the setup to create a new database for m23/Translator:</td>
				</tr>
				<tr>
					<td>Database administrator user</td>
					<td><INPUT type=\"text\" name=\"ED_DBadmin\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBadmin]\"></td>
					<td>Username of an user who can add new databases.</td>
				</tr>
				<tr>
					<td>Database administrator password</td>
					<td><INPUT type=\"text\" name=\"ED_DBadminPass\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_DBadminPass]\"></td>
					<td>Password for the administrator account.</td>
				</tr>
				</table>
				",NEXT_dbSetup);
				break;
			}
			case "addAdmin": 
			{
				showPage("m23/Translator: Create the administrator account","The administrator account is used to change user privileges or delete user accounts.
				<table>
				<tr>
					<td>Administrator name</td>
					<td><INPUT type=\"text\" name=\"ED_name\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_name]\"></td>
					<td>Name of the administrator.</td>
				</tr>
				<tr>
					<td>Administrator's passwords</td>
					<td><INPUT type=\"text\" name=\"ED_pass\" size=\"30\" maxlength=\"255\" value=\"$_POST[ED_pass]\"></td>
					<td>Password for the administrator account.</td>
				</tr>
				</table>
				Click on <i>\"next\"</i> to continue.<br>",NEXT_addAdmin);
				break;
			}
			case "finished": 
			{
				showPage("m23/Translator: Setup finished","The installation of m23/Translator has been finished. For security reasons the file setup.php should be deleted. If this script has the needed permissions it can try to do it for you. If it fails you should delete setup.php by hand.<br>
				Click on <i>\"next\"</i> to delete setup.php.<br>",NEXT_finished);
				break;
			}
			default:
				$GLOBALS[_SESSION] = array();
		};
};




$_SESSION = unserialize(urldecode($_POST[SESSION]));
global $_SESSION;

if (!isset($_SESSION[page]))
	$_SESSION[page]="welcome";

echo("<html>
<head>
	<title>m23/Translator setup</title>
	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">
	<meta http-equiv=\"expires\" content=\"0\">
	<link rel=\"stylesheet\" type=\"text/css\" href=\"css/index.css\">
</head>
<body>
");

jumpPage($_SESSION[page]);
?>