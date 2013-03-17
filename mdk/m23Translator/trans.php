<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for managing translations (articles).
$*/





define(TRANS_NOTFOUND,-1);
define(TRANS_ORIGINAL,0);
define(TRANS_ORIGINALHASCHANGED,1);
define(TRANS_NOTTRANSLATED,2);
define(TRANS_UPTODATE,3);

define(DIR_NONEXISTENT,-1);
define(DIR_ADDED,0);
define(DIR_ADDERROR,1);

define(FILE_NONEXISTENT,-1);
define(FILE_ADDED,0);
define(FILE_EXISTENT,1);





/**
**name TRANS_delete()
**description Deletes a file together with all its articles.
**parameter $_GET[fID]: file ID of the file to delete
**/
function TRANS_delete()
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	$sql="DELETE FROM ".tablePrefix."translations WHERE filenr=\"$_GET[fID]\"";
	db_query($sql);
	$sql="DELETE FROM ".tablePrefix."files WHERE id=\"$_GET[fID]\"";
	db_query($sql);

	echo("<h3>$i18n[I18N_fileDeleted]<h3>");
};





/**
**name TRANS_createDirectory($directory)
**description Creates a new directory.
**parameter diretory: name of the directory
**returns DIR_ADDED: if the directory could be added 
**returns DIR_ADDERROR: on creation error
**/
function TRANS_createDirectory($directory)
{
	include("config.php");

	if (TRANS_getDirectoryID($directory) == DIR_NONEXISTENT)
		{
			$sql="INSERT INTO `".tablePrefix."directories` (`name`) VALUES ('$directory')";
			DB_query($sql);
			return(DIR_ADDED);
		}
	else
		return(DIR_ADDERROR);
};





/**
**name TRANS_getDirectoryID($directory)
**description Returns the ID of a directory.
**parameter diretory: name of the directory
**returns DIR_NONEXISTENT: if the directory doesn't exists
**returns ID number of the directory
**/
function TRANS_getDirectoryID($directory)
{
	include("config.php");

	$sql="SELECT id FROM `".tablePrefix."directories` WHERE name=\"$directory\"";
	$res=DB_query($sql);
	$row = mysql_fetch_assoc($res);
	if (!$row)
		return(DIR_NONEXISTENT);
	else
		return($row[id]);
};





/**
**name TRANS_touchFile($fileName,$directory)
**description Creates an empty file in a directory.
**parameter fileName: name of the file
**parameter diretory: name of the directory
**returns DIR_NONEXISTENT: if the directory doesn't exists
**returns FILE_ADDED: if the file could be added
**returns FILE_EXISTENT: if there is allready a file with the same name in the same directory
**/
function TRANS_touchFile($fileName,$directory)
{
	include("config.php");

	$dID=TRANS_getDirectoryID($directory);
	if ($dID==DIR_NONEXISTENT)
		return(DIR_NONEXISTENT);

	if (TRANS_getFileID($fileName,$directory) == FILE_NONEXISTENT)
		{
			$sql="INSERT INTO `".tablePrefix."files` (`name` , `directory` ) VALUES ('$fileName', '$dID')";
			DB_query($sql);
			return(FILE_ADDED);
		}
	else
		return(FILE_EXISTENT);
};





/**
**name TRANS_getFileID($fileName,$directory)
**description Returns the ID of a directory.
**parameter fileName: name of the file
**parameter diretory: name of the directory
**returns FILE_NONEXISTENT: if the file doesn't exists
**returns ID number of the directory
**/
function TRANS_getFileID($fileName,$directory)
{
	include("config.php");

	$dID=TRANS_getDirectoryID($directory);

	$sql="SELECT id FROM `".tablePrefix."files` WHERE name=\"$fileName\" AND directory=\"$dID\"";
	$res=DB_query($sql);
	$row = mysql_fetch_assoc($res);
	if (!$row)
		return(FILE_NONEXISTENT);
	else
		return($row[id]);
};





/**
**name TRANS_addTranslation($fID,$derivedfrom,$userName,$lID,$derivedlanguage,$article,$translationDone)
**description Adds a new original article
**parameter fID: file of the translation should be added
**parameter derivedfrom: tID of the article this translation is derived from
**parameter userName: name of the user who adds the translation
**parameter lID: language ID
**parameter derivedlanguage: lID of the article the article is derived from
**parameter article: article text
**parameter translationDone: set to true, is the translation is finished. false, of not
**returns nothing
**/
function TRANS_addTranslation($fID, $derivedfrom, $userName, $lID, $derivedlanguage, $article, $translationDone)
{
	include("config.php");

	$uID=LOGIN_getUserID($userName);
	$article = mysql_real_escape_string($article);

	if($translationDone)
		$status = TRANS_UPTODATE;
	else
		$status = TRANS_ORIGINALHASCHANGED;

	$sql="INSERT INTO `".tablePrefix."translations` (`filenr` , `derivedfrom` , `editedby` , `article` , `status` , `language`, `derivedlanguage`, `time`) VALUES ('$fID', '$derivedfrom', '$uID', '$article', '$status', '$lID','$derivedlanguage', '".time()."')";
	DB_query($sql);
};





/**
**name TRANS_addOriginal($fileName, $directory, $userName, $language, $article)
**description Adds a new original article
**parameter fileName: name of the file
**parameter diretory: name of the directory
**parameter userName: name of the user who adds the article
**parameter language: language or short language name
**parameter article: article text
**/
function TRANS_addOriginal($fID, $uID, $lID, $article)
{
	include("config.php");

	$article = mysql_real_escape_string($article);

	$sql="INSERT INTO `".tablePrefix."translations` (`filenr` , `derivedfrom` , `editedby` , `article` , `status` , `language`, `derivedlanguage`, `time`) VALUES ('$fID', '-1', '$uID', '$article', '".TRANS_ORIGINAL."', '$lID', '-1','".time()."')";
	DB_query($sql);

	//update all articles that were diverted from the previous version of this original text
	if ($status == TRANS_ORIGINAL)
	{
		$sql="UPDATE `".tablePrefix."translations` SET `status` = '".TRANS_ORIGINALHASCHANGED."' WHERE `derivedlanguage` = $lID";
		DB_query($sql);
	};
};





/**
**name TRANS_showOverview()
**description Shows an overview over all translations with action links.
**/
function TRANS_showOverview()
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	$langs = LANG_getAll();
	if (count($langs) == 0)
		{
			echo("<h3>$i18n[I18N_noLanguagesDefined]</h3>");
			return;
		};

	$langKeys = array_values($langs);

	echo("<center><span class=\"title\">$i18n[I18N_overview]</span></center>\n");
	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);

	$sql="SELECT ".tablePrefix."directories.name AS dir, ".tablePrefix."files.name AS file, ".tablePrefix."files.id AS fID, ".tablePrefix."directories.id FROM ".tablePrefix."files, ".tablePrefix."directories WHERE ".tablePrefix."files.directory = ".tablePrefix."directories.id ORDER BY ".tablePrefix."directories.name, ".tablePrefix."files.name";

	$res=DB_query($sql);
	$curDir="";

	while ($row =  mysql_fetch_assoc($res))
		{
			if ($curDir != $row["dir"])
				{
					$curDir = $row["dir"];
					echo("<tr>
					<td><span class=\"subhighlight_green\">$curDir</span></td>\n");

					foreach (array_values($langs) as $langFullName)
						echo("<td><span class=\"subhighlight\">$langFullName</span></td>\n");
					echo("</tr>");
				}

			TRANS_showArticleActions($row[fID],$row[dID],$langKeys,$row["file"]);
		};
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};





/**
**name TRANS_showArticleActions($fileName,$directory,$languages)
**description Shows links with possible actions for a selected file in a selected directory.
**parameter fileName: name of the file
**parameter diretory: name of the directory
**parameter languages: array that contains all language short names
**/
function TRANS_showArticleActions($fID,$dID,$languages,$fileName)
{
	$i18n=I18N_loadLanguage();
	include("config.php");

	if (LOGIN_checkPermission(PERM_ADMIN))
		$link="<br>
		&rArr; <a href=\"index.php?page=deleteFile&fID=$fID&".SESS_getIDUrl()."\">$i18n[I18N_deleteFile]
		</a>";

	echo("<tr><td valign=\"top\">$fileName $link</td>");
	foreach ($languages as $language)
	{
		$lID=LANG_getID($language);
		$sql = "SELECT status, id, derivedfrom, time FROM ".tablePrefix."translations WHERE filenr = $fID AND language = $lID ORDER BY id DESC LIMIT 0,1";

		$res = DB_query($sql);

		$row=array();
		if (mysql_num_rows($res) > 0)
			$row = mysql_fetch_assoc($res);
		else
			$row[status]=TRANS_NOTTRANSLATED;

		//get the last changing time
		if ($row["time"] > 0)
			$changingTime = strftime($i18n[I18N_timeFormat],$row["time"]);
		else
			$changingTime = "-";
			

		switch ($row[status])
		{
			case TRANS_ORIGINAL:
			{
				$link= "<img src=\"gfx/status/blue.png\" border=0 title=\"$i18n[I18N_originalArticle]\"> $changingTime<br>
						&rArr; <a href=\"index.php?page=viewArticle&tID=$row[id]&".SESS_getIDUrl()."\">
							$i18n[I18N_viewTranslation]
						</a>";
				if (LOGIN_checkPermission(PERM_UPLOADER))
					$link.="<br>
					&rArr; <a href=\"index.php?page=editOriginal&tID=$row[id]&".SESS_getIDUrl()."\">	$i18n[I18N_adjustTranslation]
					</a>";
				
				break;
			};

			case TRANS_UPTODATE:
			{
				$link= "<img src=\"gfx/status/green.png\" border=0 title=\"$i18n[I18N_translationUpToDate]\"> $changingTime<br>
					&rArr; <a href=\"index.php?page=viewArticle&tID=$row[id]&".SESS_getIDUrl()."\">
						$i18n[I18N_viewTranslation]
					</a><br>
					&rArr; <a href=\"index.php?page=editArticle&fID=$fID&lID=$lID&dID=$dID&tID=$row[derivedfrom]&".SESS_getIDUrl()."\">	$i18n[I18N_adjustTranslation]
					</a>";
				break;
			};

			case TRANS_ORIGINALHASCHANGED:
			{
				$link= "<img src=\"gfx/status/yellow.png\" border=0 title=\"$i18n[I18N_originalHasChanged]\"> $changingTime<br>
				&rArr; <a href=\"index.php?page=editArticle&fID=$fID&lID=$lID&dID=$dID&tID=$row[derivedfrom]&".SESS_getIDUrl()."\">
							$i18n[I18N_adjustTranslation]
						</a>";
				break;
			};

			case TRANS_NOTTRANSLATED:
			default:
			{
				$link= "<img src=\"gfx/status/red.png\" border=0 title=\"$i18n[I18N_notTranslated]\"> -<br>
				 &rArr; <a href=\"index.php?page=editArticle&fID=$fID&lID=$lID&dID=$dID&tID=$row[derivedfrom]&".SESS_getIDUrl()."\">	$i18n[I18N_createTranslation]
				</a>";
				break;
			};
		}

		echo("<td valign=\"top\">$link</td>");
	};
		
	echo("</tr>");
};





/**
**name TRANS_viewArticle()
**description Shows an article viewer.
**parameter $_GET[tID]: translation ID of the article
**/
function TRANS_viewArticle()
{
	TRANS_showEditor($_GET[tID],true);
};





/**
**name TRANS_editArticle()
**description Shows an article editor/translator.
**parameter $_GET[fID]: file ID of the article
**parameter $_GET[dID]: directory ID of the article
**parameter $_GET[lID]: language ID of the article
**/
function TRANS_editArticle()
{
	TRANS_showEditor($_GET[tID],false,$_GET[fID],$_GET[dID],$_GET[lID]);
};





/**
**name TRANS_getOriginals($fID)
**description Returns informations about original articles for a given file.
**parameter $_GET[fID]: file ID of the article
**returns array with translation IDs as keys and the language full names as values.
**/
function TRANS_getOriginals($fID)
{
	include("config.php");

	$sql="SELECT MAX(".tablePrefix."translations.id) AS tID, ".tablePrefix."languages.name AS lang FROM ".tablePrefix."files, ".tablePrefix."translations, ".tablePrefix."languages WHERE ".tablePrefix."files.id = \"$fID\" AND ".tablePrefix."files.id = ".tablePrefix."translations.filenr AND ".tablePrefix."translations.language = ".tablePrefix."languages.id GROUP BY lang";

	$res=DB_query($sql);
	while ($row = mysql_fetch_assoc($res))
		$out[$row[tID]]=$row[lang];

	return($out);
};





/**
**name TRANS_getOldTranslations($fID,$lID)
**description Returns an array with tIDs and times of old articles by a given fID and lID.
**parameter fID: file ID for searching the articles
**parameter lID: language ID for searching the articles
**returns array with tIDs as keys and times in human readable format as values
**/
function TRANS_getOldTranslations($fID,$lID)
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	$sql="SELECT `id`,`time` FROM `".tablePrefix."translations` WHERE `filenr` = $fID AND `language` = $lID ORDER BY time DESC";
	$out=array();

	$res=DB_query($sql);
	while ($row = mysql_fetch_assoc($res))
		$out[$row[id]]=strftime($i18n[I18N_timeFormat],$row["time"]);

	return($out);
};





/**
**name TRANS_getArticleInformation($tID)
**description Returns informations about the article.
**parameter tID: translation ID of the article
**returns array[fileName]: file name of the article
**returns array[derivedfrom]: the article was derived from this other article
**returns array[userName]: the user name that the article posted
**returns array[article]: article text
**returns array[status]: status number of this article
**returns array[lang]: language of the article
**/
function TRANS_getArticleInformation($tID)
{
	include("config.php");

	$sql="SELECT ".tablePrefix."files.name AS fileName, ".tablePrefix."translations.language, ".tablePrefix."translations.filenr, ".tablePrefix."translations.derivedfrom, ".tablePrefix."translations.time, ".tablePrefix."logins.name AS userName, ".tablePrefix."translations.article, ".tablePrefix."translations.status, ".tablePrefix."languages.name AS lang FROM ".tablePrefix."translations, ".tablePrefix."languages, ".tablePrefix."logins, ".tablePrefix."files WHERE ".tablePrefix."translations.id = $tID AND ".tablePrefix."translations.language = ".tablePrefix."languages.id AND ".tablePrefix."translations.editedby = ".tablePrefix."logins.id AND ".tablePrefix."files.id = ".tablePrefix."translations.filenr";
	$res=DB_query($sql);

	if (!($res === false))
		{
			$row = mysql_fetch_assoc($res);
			$row[article]=stripslashes($row[article]);
			if (empty($row[userName]))
				$row[userName]="-";
		
			return($row);
		}
	else
		return(array());
}






/**
**name TRANS_getNewestTID($fID,$lID)
**description Returns the tID of the newest translation of a file for a given language.
**parameter fID: file ID of the article
**parameter lID: language ID of the article
**returns TRANS_NOTFOUND: if there couldn't be found a matching translation
**returns translation ID of the article
**/
function TRANS_getNewestTID($fID,$lID)
{
	include("config.php");

	$sql="SELECT MAX(".tablePrefix."translations.id) as id  FROM ".tablePrefix."translations WHERE ".tablePrefix."translations.filenr = \"$fID\" AND ".tablePrefix."translations.language = \"$lID\"";
	$res=DB_query($sql);

	$row = mysql_fetch_assoc($res);
	if ((mysql_num_rows($res) > 0) && (!empty($row[id])))
		return($row[id]);
	else
		return(TRANS_NOTFOUND);
}





/**
**name TRANS_getNewestOriginal($tID)
**description Returns the tID of the newest article matching the file and language ID of the selected translation.
**parameter tID: translation ID of the article.
**returns translation ID
**/
function TRANS_getNewestOriginal($tID)
{
	include("config.php");

	$sql="SELECT `filenr` , `language` FROM `".tablePrefix."translations` WHERE `id` = $tID";
	$res=DB_query($sql);

	$row = mysql_fetch_assoc($res);
	return(TRANS_getNewestTID($row[filenr],$row[language]));
}





/**
**name TRANS_getFileName($fID)
**description Returns the file name of a file ID.
**parameter fID: file ID of the article
**returns file name
**/
function TRANS_getFileName($fID)
{
	include("config.php");

	$sql="SELECT name AS fileName FROM ".tablePrefix."files WHERE id = \"$fID\"";
	$res=DB_query($sql);
	$row = mysql_fetch_assoc($res);
	return($row[fileName]);
};





/**
**name TRANS_editOriginal()
**description Shows an editor for editing an original article.
**/
function TRANS_editOriginal()
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	if (isset($_POST[origtID]))
		$origtID = $_POST[origtID];
	else
		$origtID = $_GET[tID];

	$original=TRANS_getArticleInformation($origtID);

	if (isset($_POST[TB_translation]))
		$originalText = $_POST[TB_original];
	else
		$originalText = $original[article];


	if (isset($_POST[BUT_save]))
		{
			$uID=LOGIN_getUserID($_SESSION[userName]);
			TRANS_addOriginal($original[filenr], $uID, $original[language], $_POST[TB_original]);
			echo("<h3>$i18n[I18N_originalChanged]</h3><br>");
			return;
		};

	echo("<center><span class=\"title\">$i18n[I18N_editOriginal]: $original[fileName]</span></center>\n");
	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);
		
	echo("
	<input type=\"hidden\" name=\"origtID\" value=\"$origtID\">
	<tr>
		<td align=\"right\">
			<span class=\"subhighlight_red\">$i18n[I18N_language]:</span> $original[lang]
			<span class=\"subhighlight_red\">$i18n[I18N_editedBy]:</span> $original[userName]
			<span class=\"subhighlight_red\">$i18n[I18N_changeTime]:</span> "
			.strftime($i18n[I18N_timeFormat],$original["time"])."
		</td>
	</tr>
	<tr>
		<td>
			<textarea cols=\"85\" rows=\"25\" name=\"TB_original\">$original[article]</textarea>
		</td>
	</tr>
	<tr>
		<td align=\"center\">
			<INPUT type=\"submit\" name=\"BUT_save\" value=\"$i18n[I18N_save]\">
		</td>
	</tr>
	");
	
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};





/**
**name RANS_showEditor($origtID,$readOnly,$origfID=-1, $origdID=-1 ,$editlID=-1)
**description Shows a translation viewer or translator.
**parameter origtID: translation ID of the original article
**parameter readOnly: set to true if a viewer should be shown or false for a translator
**parameter origfID: file ID of the original article
**parameter origdID: directory ID of the original article
**parameter editlID: language ID the article should ge translated to
**/
function TRANS_showEditor($origtID,$readOnly,$origfID=-1, $origdID=-1 ,$editlID=-1)
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	//check if we have a read only editor after pressing a submit button
	if (isset($_POST[readOnlyEditor]))
		$readOnly = $_POST[readOnlyEditor] == "yes";

	if ($readOnly)
		{
			$readOnly="true";
			$title=$i18n[I18N_viewTranslation];
			$original=TRANS_getArticleInformation($origtID);
			$editorRows=30;
		}
	else
		{
			$editorRows=15;
			$title=$i18n[I18N_translate];
			$readOnly="false";
			$addHTMLTop="";
			
			//check if we have to fetch the original file, directory and translation ID
			if (isset($_POST[origfID]))
				$origfID = $_POST[origfID];
			if (isset($_POST[origdID]))
				$origdID = $_POST[origdID];
			if (isset($_POST[origtID]))
				$origtID = $_POST[origtID];
			else
				$origtID = TRANS_getNewestOriginal($origtID);
				
			//check if we have to fetch the edit language ID
			if (isset($_POST[editlID]))
				$editlID = $_POST[editlID];

//>>>>>>>>>>>original
			$org_lID_language=TRANS_getOriginals($origfID);


			//check if a new original article should be loaded
			if (isset($_POST[BUT_loadOriginalArticle]))
				{
					$origtID = $_POST[SEL_language];
					$firstName = $org_lID_language[$origtID];
					$first = $origtID;
				};

			//make a selection of all language the oroginal article is available in
			$i=0;
			foreach (array_keys($org_lID_language) as $key)
				{
					$languageList["name$i"]=$org_lID_language[$key];
					$languageList["val$i"]=$key;
					$i++;
				};


			//add the selection of available original languages and a load button
			$addHTMLTop.= "$i18n[I18N_originalArticlesAreAvailableInTheseLanguages] "
			.HTML_listSelection("SEL_language",$languageList,$first,$firstName).
			"<input type=\"submit\" name=\"BUT_loadOriginalArticle\" value=\"$i18n[I18N_loadOriginalArticle]\">";

//>>>>>>>>>>>translation
			//get the newest tID and informations about the translation
			$newestTranslationTID=TRANS_getNewestTID($origfID,$editlID);
			$translation = TRANS_getArticleInformation($newestTranslationTID);
			//get information about old translations
			$old_tID_time=TRANS_getOldTranslations($origfID,$editlID);

			$i=0;
			foreach (array_keys($old_tID_time) as $key)
				{
					$oldTranslationsList["name$i"]=$old_tID_time[$key];
					$oldTranslationsList["val$i"]=$key;
					$i++;
				};

			//check if the article in translation editor was changed (e.g. by hitting the load original article button).
			if (isset($_POST[TB_translation]))
				//load the article from POST
				$translatedText = $_POST[TB_translation];
			else
				{
					if ($newestTranslationTID != TRANS_NOTFOUND)
						$translatedText = $translation[article];
				};

			if (isset($_POST[BUT_loadTranslatedArticle]))
				{
					$firstTranslation = $newestTranslationTID = $_POST[SEL_oldTranslation];
					$firstNameTranslation = $old_tID_time[$firstTranslation];
					$translation = TRANS_getArticleInformation($newestTranslationTID);
					$translatedText = $translation[article];
				};

			$translationLang=LANG_getLongName($editlID);

			$hidden="
				<input type=\"hidden\" name=\"origtID\" value=\"$origtID\">
				<input type=\"hidden\" name=\"readOnlyEditor\" value=\"no\">
				<input type=\"hidden\" name=\"origfID\" value=\"$origfID\">
				<input type=\"hidden\" name=\"origdID\" value=\"$origdID\">
				<input type=\"hidden\" name=\"editlID\" value=\"$editlID\">
				";
			

			//get the informations from the original article
			if ($origtID > 0)
				$original=TRANS_getArticleInformation($origtID);

			if (!isset($original[fileName]))
				$original[fileName]=TRANS_getFileName($origfID);

			//generate srting with access time
			if (empty($translation["time"]))
				$translationTime = "-";
			else
				$translationTime = strftime($i18n[I18N_timeFormat],$translation["time"]);
			
			if (isset($_POST[BUT_saveUnfinished]) || isset($_POST[BUT_saveDone]))
				{
					//if the translation is complete, the derivedfrom tID must be the same as the loaded original text
					if (isset($_POST[BUT_saveDone]))
						$newDerivedfrom = $origtID;
					else
					//if not: use the derivedfrom tID from the old translation
						$newDerivedfrom = $translation[derivedfrom];

					TRANS_addTranslation($origfID,$newDerivedfrom,$_SESSION[userName],$editlID,$original[language],$translatedText,isset($_POST[BUT_saveDone]));

					echo("<h3>$i18n[I18N_thanksForTranslation]</h3>");
					return;
				};

			$addHTMLEnd="
			<tr>
				<td align=\"center\"><br><br>
					&rArr; <a href=\"index.php?page=diffArticles&fromtID=$translation[derivedfrom]&totID=$origtID&".SESS_getIDUrl()."\" target=\"_blank\"> $i18n[I18N_viewDifferencesBetweenTranslatedAndCurrentOriginal]</a><br><br>
				</td>
			</tr>
			<tr>
				<td align=\"right\">
					<span class=\"title\">$i18n[I18N_translation]: $original[fileName]</span>
				</td>
			</tr>
			<tr>
				<td>$i18n[I18N_loadOldTranslationIntoEditor] "
			.HTML_listSelection("SEL_oldTranslation",$oldTranslationsList,$firstTranslation,$firstNameTranslation).
			"<input type=\"submit\" name=\"BUT_loadTranslatedArticle\" value=\"$i18n[I18N_loadTranslatedArticle]\"> 
				</td>
			</tr>
			<tr>
				<td align=\"right\">
					<span class=\"subhighlight_red\">$i18n[I18N_language]:</span> $translationLang
					<span class=\"subhighlight_red\">$i18n[I18N_editedBy]:</span> $translation[userName]
					<span class=\"subhighlight_red\">$i18n[I18N_changeTime]:</span> $translationTime
				</td>
			</tr>
			<tr>
				<td>
					<textarea cols=\"85\" rows=\"$editorRows\" name=\"TB_translation\">$translatedText</textarea>
				</td>
			</tr>
			<tr>
				<td align=\"right\">
					<INPUT type=\"submit\" name=\"BUT_saveUnfinished\" value=\"$i18n[I18N_saveTranslationAndMarkAsUnfinished]\">
					<INPUT type=\"submit\" name=\"BUT_saveDone\" value=\"$i18n[I18N_saveTranslationAndMarkAsDone]\">
				</td>
			</tr>
			";
		};


	echo("<center><span class=\"title\">$title: $original[fileName]</span></center>\n");
	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);

	echo("
	$hidden
	$addHTMLTop
	<tr>
		<td align=\"right\">
			<span class=\"subhighlight_red\">$i18n[I18N_language]:</span> $original[lang]
			<span class=\"subhighlight_red\">$i18n[I18N_editedBy]:</span> $original[userName]
			<span class=\"subhighlight_red\">$i18n[I18N_changeTime]:</span> "
						.strftime($i18n[I18N_timeFormat],$original["time"])."
		</td>
	</tr>
	<tr>
		<td>
			<textarea cols=\"85\" rows=\"$editorRows\" readonly=\"$readOnly\">$original[article]</textarea>
		</td>
	</tr>
	$addHTMLEnd
	");
	
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};
?>