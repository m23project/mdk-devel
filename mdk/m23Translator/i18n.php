<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for internationalisation support.
$*/





/**
**name I18N_loadLanguage($lang="",$force=false)
**description Reads the I18N keys and their native translations from the language file or the cache in the $_SESSION array.
**parameter lang: language short name
**parameter force: set to true, if the (re)reading of the language file should be forced. Otherwise the file is read if the values aren't cached in the $_SESSION array.
**returns Returns an array with all I18N symbols as keys and their native translations as values.
**/
function I18N_loadLanguage($lang="",$force=false)
{
	include("config.php");

	if (!empty($lang))
		$_SESSION[lang]=$lang;

	if (empty($_SESSION[lang]))
		$_SESSION[lang]="de";

	if (!isset($_SESSION['i18n']) || $force)
		{
			include("i18n/$_SESSION[lang].php");
			$_SESSION['i18n']=array();
			$vars=get_defined_vars();
			foreach (array_keys($vars) as $key)
				if (!(strpos($key,"I18N_")===FALSE))
					$_SESSION['i18n'][$key]=$vars[$key];
		};
	
	return($_SESSION['i18n']);
};
?>