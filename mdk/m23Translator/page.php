<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for showing different pages.
$*/





/**
**name PAGE_showMain($subpage)
**description Shows the main page with a subpage in the lower part of the screen.
**parameter subpage: name of the sub page
**/
function PAGE_showMain($subpage="")
{
	include("config.php");
	$i18n=I18N_loadLanguage();

	//check which subpage function should be used
	if (empty($subpage))
		$_SESSION[page]="main";
	else
		$_SESSION[page]=$subpage;

	switch ($subpage)
	{
		case "newLang": $fkt = LANG_showAddNew; $help=$i18n[I18N_HELP_showAddNew]; break;
		case "viewArticle": $fkt = TRANS_viewArticle; $help=$i18n[I18N_HELP_viewArticle]; break;
		case "editArticle": $fkt = TRANS_editArticle; $help=$i18n[I18N_HELP_editArticle];  break;
		case "editOriginal": $fkt = TRANS_editOriginal; $help=$i18n[I18N_HELP_editOriginal]; break;
		case "help": $fkt = null; $help=$i18n[I18N_HELP_introduction]; break;
		case "administration": $fkt = LOGIN_showUsers; break;
		case "deleteFile": $fkt = TRANS_delete; break;
		case "main":
		default: $fkt = TRANS_showOverview; $help=$i18n[I18N_HELP_showOverview];
	};


	$i18n=I18N_loadLanguage();
	
	if (isset($_POST[BUT_logout]))
		{
			 LOGIN_logout();
			 INDEX_showPage("login");
			 return;
		};

	if (LOGIN_checkPermission(PERM_ADMIN))
		$menuAdd="<a href=\"index.php?page=administration&".SESS_getIDUrl()."\">[$i18n[I18N_administration]]</a>";
	
	//show menu and page
	HTML_showFormHeader(strip_tags(SID));
	echo("
	<table width=\"100%\">
		<tr>
			<td align=\"right\">
				$menuAdd
				<a href=\"index.php?page=main&".SESS_getIDUrl()."\">[$i18n[I18N_overview]]</a>
				<a href=\"index.php?page=newLang&".SESS_getIDUrl()."\">[$i18n[I18N_addNewLanguage]]</a>
				<a href=\"index.php?page=help&".SESS_getIDUrl()."\">[$i18n[I18N_help]]</a>
				<a href=\"index.php?page=logout&".SESS_getIDUrl()."\">[$i18n[I18N_logout] ($_SESSION[userName])]</a>
			</td>
		</tr>
		<tr>
			<td>");

		if ($fkt != null)
			$fkt();

		if (!empty($help))
			PAGE_help($help);

	echo("
			</td>
		</tr>
	</table>
	");
	HTML_showFormEnd();
};





/**
**name PAGE_reloadIndexPHP()
**description Reloads the main page without any GET parameter.
**/
function PAGE_reloadIndexPHP()
{
echo("<html>
<head>
<meta http-equiv=\"refresh\" content=\"0; URL=index.php\">
</head>
<body>
<a href=\"index.php\">Return to the login screen</a>
</body>
</html>");
};





/**
**name PAGE_help($message)
**description Shows a box with a help message.
**parameter message: message text
**/
function PAGE_help($message)
{
	$i18n=I18N_loadLanguage();

	echo("<br><CENTER>
	<table class=\"infotable\" width=\"50%\" align=\"center\" border=\"0\" cellspacing=\"5\">
	<tr><td><span class=\"subhighlight_yellow\">$i18n[I18N_help]</span></td></tr>
	<tr><td><br>$message<br></td></tr>
	</table></CENTER><br>");
};
?>