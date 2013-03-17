<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for managing different languages for the articles.
$*/





define(LANGERR_LANDDOESNTEXIST,-1);
define(LANGERR_ADDED,0);
define(LANGERR_NAMEEXISTS,1);
define(LANGERR_SHORTNAMEEXISTS,2);
define(LANGERR_NAMEANDSHORTNAMEEXISTS,3);





/**
**name LANG_addNew($name,$shortName)
**description Checks if a user name/password combination is valid (password is set) or a user name exists (password is empty).
**parameter name: full name of the language
**parameter shortName: short name of the language
**returns LANGERR_ADDED: if the language was be added
**returns LANGERR_NAMEEXISTS: long laguage name exists
**returns LANGERR_SHORTNAMEEXISTS: short laguage name exists
**/
function LANG_addNew($name,$shortName)
{
	include("config.php");
	$name = mysql_real_escape_string($name);
	$shortName = mysql_real_escape_string($shortName);

	//try to fetch an entry with the name and the short name
	$sql="SELECT name, shortname  FROM `".tablePrefix."languages` WHERE `shortname` = '$shortName' OR `name` LIKE '$name'";
	$res = DB_query($sql);
	$row = mysql_fetch_assoc($res);

	$err=LANGERR_ADDED;

	//if the name exists add the name error code to the error variable
	if ($row[name] == $name)
		$err+=LANGERR_NAMEEXISTS;

	if ($row[shortname] == $shortName)
		$err+=LANGERR_SHORTNAMEEXISTS;

	if ($err != LANGERR_ADDED)
		return($err);

	$sql="INSERT INTO `".tablePrefix."languages` (`name` , `shortname`) VALUES ('$name', '$shortName')";
	DB_query($sql);
};





/**
**name LANG_getLongName($lID)
**description Returns the long name of a language.
**parameter lID: language ID
**returns long name of the language: if it the lID is valid
**returns LANGERR_LANDDOESNTEXIST: on invalid lID
**/
function LANG_getLongName($lID)
{
	include("config.php");

	$sql="SELECT name  FROM `".tablePrefix."languages` WHERE `id` = '$lID'";
	$res = DB_query($sql);
	$row = mysql_fetch_assoc($res);
	
	if (empty($row[name]))
		return(LANGERR_LANDDOESNTEXIST);
	else
		return($row[name]);
};





/**
**name LANG_getID($lang)
**description Returns the ID of a language for a selected language name or short name.
**parameter lang: language name or short name
**returns LANGERR_LANDDOESNTEXIST: if the language couldn't be found
**returns language ID: on success
**/
function LANG_getID($lang)
{
	include("config.php");

	$sql="SELECT id  FROM `".tablePrefix."languages` WHERE `shortname` = '$lang' OR `name` LIKE '$lang'";
	$res = DB_query($sql);
	$row = mysql_fetch_assoc($res);
	
	if (empty($row[id]))
		return(LANGERR_LANDDOESNTEXIST);
	else
		return($row[id]);
};





/**
**name LANG_getAll()
**description Returns an array with the language short names as array keys and the corresponding long names as values
**returns array
**/

function LANG_getAll()
{
	include("config.php");

	$sql="SELECT shortname, name  FROM `".tablePrefix."languages`";
	$res = DB_query($sql);
	
	
	while ($row = mysql_fetch_assoc($res))
		$out[$row[shortname]] = $row[name];
	
	return($out);
};





/**
**name LANG_showAddNew()
**description Shows a dialog to add a new language.
**/
function LANG_showAddNew()
{
	$i18n=I18N_loadLanguage();
	
	if (isset($_POST[BUT_addLanguage]))
		{
			$ret = LANG_addNew($_POST[ED_fullName],$_POST[ED_shortName]);
			
			if ($ret != LANGERR_ADDED)
				{
					if (($ret & LANGERR_NAMEEXISTS) == LANGERR_NAMEEXISTS)
						echo("<h3>$i18n[I18N_fullLanguageNameExists]</h3>");

					if (($ret & LANGERR_SHORTNAMEEXISTS) == LANGERR_SHORTNAMEEXISTS)
						echo("<h3>$i18n[I18N_shortLanguageNameExists]</h3>");
				}
			else
				{
					echo("<h3>$i18n[I18N_languageAdded]</h3>");
					return;
				};
		};

	HTML_showFormHeader(strip_tags(SID));
	echo("<center><span class=\"title\">$i18n[I18N_addNewLanguage]</span></center>");
	HTML_showTableHeader(true);
	echo("
	<tr>
		<td>$i18n[I18N_fullLanguageName]</td>
		<td><INPUT type=\"text\" name=\"ED_fullName\" maxlength=\"254\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_shortLanguageName]</td>
		<td><INPUT type=\"text\" name=\"ED_shortName\" size=\"2\" maxlength=\"2\"></td>
	</tr>
	<tr>
		<td colspan=\"2\" align=\"center\">
			<INPUT type=\"submit\" name=\"BUT_addLanguage\" value=\"$i18n[I18N_add]\">
		</td>
	</tr>
	");
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};
?>