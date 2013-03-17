<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Tools and functions for exporting articles from m23/Translator.
$*/





	include("trans.php");
	include("login.php");
	include("i18n.php");
	include("page.php");
	include("lang.php");
	include("db.php");
	include("sess.php");
	include("config.php");

	DB_connect();





/**
**name SQL_getFiles($addSelects="",$wildcards)
**description Get information about all 
**parameter addSelects: Additional SELECT parameters e.g. translations.article AS text
**parameter wildcard: selects directories, languages and filenames to search for
**returns The result of the MySQL query.
**/
function SQL_getFiles($addSelects="",$wildcards)
{
	include("config.php");

	$wcS=split("/",$wildcards);

	if (count($wcS) != 3)
		exit;

	$addLikes="";

	if (!empty($wcS[0]))
		$addLikes.=" AND ".tablePrefix."directories.name LIKE \"$wcS[0]\"";

	if (!empty($wcS[1]))
		$addLikes.=" AND ".tablePrefix."languages.shortname LIKE \"$wcS[1]\"";

	if (!empty($wcS[2]))
		$addLikes.=" AND ".tablePrefix."files.name LIKE \"$wcS[2]\"";

	$sql="SELECT ".tablePrefix."translations.id AS tID, ".tablePrefix."translations.filenr, ".tablePrefix."languages.shortname AS lang, ".tablePrefix."directories.name AS dir, ".tablePrefix."files.name AS fileName ,".tablePrefix."translations.article AS text $addSelects FROM `".tablePrefix."translations`, `".tablePrefix."languages`, `".tablePrefix."directories`, `".tablePrefix."files` WHERE ".tablePrefix."translations.id IN (SELECT MAX(".tablePrefix."translations.id) AS mid FROM `".tablePrefix."translations`, `".tablePrefix."languages`, `".tablePrefix."directories`, `".tablePrefix."files`  WHERE ".tablePrefix."languages.id = ".tablePrefix."translations.language AND ".tablePrefix."directories.id =  ".tablePrefix."files.directory AND ".tablePrefix."files.id = ".tablePrefix."translations.filenr GROUP BY ".tablePrefix."translations.filenr, ".tablePrefix."languages.shortname) AND ".tablePrefix."languages.id = ".tablePrefix."translations.language AND ".tablePrefix."directories.id = ".tablePrefix."files.directory AND ".tablePrefix."files.id = ".tablePrefix."translations.filenr $addLikes GROUP BY ".tablePrefix."translations.filenr, ".tablePrefix."languages.shortname, ".tablePrefix."translations.id";

	return(DB_query($sql));
}





/**
**name nulldecompress($in)
**description Function that returns the parameter.
**parameter in: Input string
**returns The input string
**/
function nulldecompress($in)
{
	return($in);
};





define(CMD_SHOW,1);
define(CMD_DOWNLOAD,2);

/**
**name show($cmd,$wildcards)
**description Get information about all 
**parameter cmd: Command to execute: CMD_SHOW: list files, CMD_DOWNLOAD: download files
**parameter wildcard: selects directories, languages and filenames to search for
**/
function show($cmd,$wildcards)
{
	include("config.php");

	switch($_POST[compress])
		{
			case "null": $comp=nulldecompress; break;
			case "gz": $comp=gzcompress; break;
			case "bz": $comp=bzcompress; break;
		};

	$arr=array();
	$nr=0;

	switch ($cmd)
		{
			case CMD_SHOW:
				$res=SQL_getFiles("",$wildcards);
				while ($row =  mysql_fetch_assoc($res))
					$arr[$nr++]=array("dir" => $row[dir], "lang" => $row[lang], "name" => $row[fileName]);
				break;
			case CMD_DOWNLOAD:
				$res=SQL_getFiles(",".tablePrefix."translations.article AS text, ".tablePrefix."translations.time AS modTime",$wildcards);
				while ($row =  mysql_fetch_assoc($res))
					$arr[$nr++]=array("dir" => $row[dir], "lang" => $row[lang], "name" => $row[fileName], "text" => stripslashes($row[text]), "modTime" => $row[modTime]);
				break;
		}

	echo($comp(serialize($arr)));
};

switch ($_POST[cmd])
	{
		case "show": show(CMD_SHOW,urldecode($_POST[wc])); break;
		case "download": show(CMD_DOWNLOAD,urldecode($_POST[wc])); break;
	}

?>