<?php

include("db.php");
include("login.php");
include("i18n.php");
include("page.php");
include("lang.php");
include("trans.php");
include("diff.php");
include("html.php");
include("sess.php");
include("config.php");

DB_connect();

//try to fetch the session ID or start a new session
$SESS_ID = SESS_start();
global $SESS_ID;

//load old session
$_SESSION = SESS_load();
global $_SESSION;

if (!isset($_SESSION[lang]))
	I18N_loadLanguage($defaultLanguage,true);

if (isset($_GET[page]))
	$_SESSION[page] = $_GET[page];

if (!isset($_SESSION[page]))
	$_SESSION[page]="login";

INDEX_showPage($_SESSION[page]);

SESS_save($_SESSION);


function INDEX_showPage($page)
{
	if ($_SESSION[login] == "yes")
		switch ($page)
		{
			case "login": LOGIN_showLogin(); break;
			case "register": LOGIN_showRegister(); break;
			case "main": PAGE_showMain(); break;
			case "newLang": PAGE_showMain("newLang"); break;
			case "help": PAGE_showMain("help"); break;
			case "viewArticle": PAGE_showMain("viewArticle"); break;
			case "editArticle": PAGE_showMain("editArticle"); break;
			case "editOriginal": 
				if (LOGIN_checkPermission(PERM_UPLOADER))
					PAGE_showMain("editOriginal");
				break;
			case "logout": LOGIN_logout(); PAGE_reloadIndexPHP(); exit;
			case "diffArticles": DIFF_showDiff(); break;
			case "administration":
				if (LOGIN_checkPermission(PERM_ADMIN))
					PAGE_showMain("administration");
				break;
			case "deleteFile":
				if (LOGIN_checkPermission(PERM_ADMIN))
					PAGE_showMain("deleteFile");
				break;
		}
	else
		switch ($page)
		{
			case "login": LOGIN_showLogin(); break;
			case "register": LOGIN_showRegister(); break;
			default: 
				{
					echo("<h3>Access violation!</h3>");
					$_SESSION[page]="login";
					PAGE_reloadIndexPHP();
				};
		}
}

echo("<html>
<head>
	<title>m23/Translator</title>
	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">
	<meta http-equiv=\"expires\" content=\"0\">
	<link rel=\"stylesheet\" type=\"text/css\" href=\"css/index.css\">
</head>
<body>
");

?>

</body>
</html>