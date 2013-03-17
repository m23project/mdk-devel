<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Functions for handling logins e.g. create new accounts.
$*/





define(LOGINERR_USERVALID,0);
define(LOGINERR_USERUNKNOWN,1);
define(LOGINERR_WRONGPASSWORD,2);
define(LOGINERR_USEREXISTS,3);
define(LOGINERR_USERADDED,4);
define(LOGINERR_PASSWORDSDONTMATCH,5);
define(LOGINERR_BADPASSWORD,6);
define(LOGINERR_USERNAMETOSHORT,7);
define(LOGINERR_EMAILEMPTY,8);

//user can make translations and update in transTool
define(PERM_TRANSLATOR,1);
//user can upload original texts
define(PERM_UPLOADER,2);
define(PERM_ADMIN,4);


/**
**name LOGIN_new($name,$password)
**description Stores a new user in the DB.
**parameter name: name of the user
**parameter password: password for the account
**returns LOGINERR_USERADDED: if the user could be added
**returns LOGINERR_USEREXISTS: if the username exists allready
**returns LOGINERR_PASSWORDSDONTMATCH: there are entered two passwords that don't match.
**returns LOGINERR_BADPASSWORD: password is empty
**returns LOGINERR_USERNAMETOSHORT: the username is to short
**returns LOGINERR_EMAILEMPTY: email address is empty
**/
function LOGIN_new($name,$password,$password2="",$perm=PERM_TRANSLATOR,$email)
{
	include("config.php");

	if (empty($password))
		return(LOGINERR_BADPASSWORD);

	if (strlen($name) < 3)
		return(LOGINERR_USERNAMETOSHORT);

	if (empty($email))
		return(LOGINERR_EMAILEMPTY);

	if (!empty($password2) && $password != $password2)
		return(LOGINERR_PASSWORDSDONTMATCH);

	if (LOGIN_check($name,"") == LOGINERR_USERUNKNOWN)
		{
			$name = mysql_real_escape_string($name);
			$password = mysql_real_escape_string($password);
			$sql = "INSERT INTO `".tablePrefix."logins` (`name` , `password`, `permissions`, `email`) VALUES ('$name', MD5( '$password' ),'$perm','$email')";
			DB_query($sql);
			return(LOGINERR_USERADDED);
		}
	else
		return(LOGINERR_USEREXISTS);
	
};





/**
**name LOGIN_check($name,$password)
**description Checks if a user name/password combination is valid (password is set) or a user name exists (password is empty).
**parameter name: name of the user
**parameter password: password for the account
**returns LOGINERR_USERUNKNOWN: the user is unknown
**returns LOGINERR_USERVALID: user name/password combination is valid
**returns LOGINERR_WRONGPASSWORD: user is valid, but password doesn't match
**/
function LOGIN_check($name,$password)
{
	include("config.php");

	$name = mysql_real_escape_string($name);
	$password = mysql_real_escape_string($password);

	$sql = "SELECT password FROM `".tablePrefix."logins` WHERE name = \"$name\"";
	$res = DB_query($sql);

	$row = mysql_fetch_assoc($res);
	if (empty($row[password]) || empty($name))
		return(LOGINERR_USERUNKNOWN);
		
	if (md5($password) == $row[password])
		return(LOGINERR_USERVALID);
	else
		return(LOGINERR_WRONGPASSWORD);
};





/**
**name LOGIN_showLogin()
**description Shows a login page, checks user name and password, can jump to the register page and jumps to the page "main" on login success.
**/
function LOGIN_showLogin()
{
	include("config.php");

	$_SESSION[page]="login";
	$i18n=I18N_loadLanguage(defaultLanguage);

	if (isset($_POST[BUT_registerPage]))
		{
			LOGIN_showRegister();
			return;
		};

	if (isset($_POST[BUT_login]))
		{
			$return = LOGIN_check($_POST[ED_userName],$_POST[ED_password]);
			if ($return != LOGINERR_USERVALID)
				{
					echo("<h3>$i18n[I18N_userUnknownOrWrongPassword]</h3>");
				}
			else
				{
					$_SESSION[lang] = $_POST[SEL_language];
					I18N_loadLanguage($_SESSION[lang],true);
					$_SESSION[login] = "yes";
					$_SESSION[userName] = $_POST[ED_userName];
					INDEX_showPage("main");
					return;
				}
		};

	$uiLanguages=unserialize(uiLanguages);
	$i=0;
	//build HTML selection for possible languages
	foreach (array_keys($uiLanguages) as $key)
	{
		$languageList["name$i"]=$uiLanguages[$key];
		$languageList["val$i"]=$key;
		$i++;
	};

	$first = defaultLanguage;
	$firstName = $uiLanguages[defaultLanguage];
	$htmlLanguageList = HTML_listSelection("SEL_language",$languageList,$first,$firstName);

	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);
	echo("
	<tr>
		<td colspan=\"2\"><img src=\"gfx/logo.png\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_login_name]</td>
		<td><INPUT type=\"text\" name=\"ED_userName\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_password]</td>
		<td><INPUT type=\"password\" name=\"ED_password\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_language]</td>
		<td>$htmlLanguageList</td>
	</tr>
	<tr>
		<td colspan=\"2\" align=\"center\">
			<INPUT type=\"submit\" name=\"BUT_login\" value=\"$i18n[I18N_login]\">
			<INPUT type=\"submit\" name=\"BUT_registerPage\" value=\"$i18n[I18N_register]\">
		</td>
	</tr>
	<tr>
		<td colspan=\"2\" align=\"right\">
			$i18n[I18N_version]: $version
		</td>
	</tr>
	");
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};





/**
**name LOGIN_showRegister()
**description Shows a user registration page, checks if both passwords are identically, registers new user names and can jump to the login page after successfull registration.
**/
function LOGIN_showRegister()
{
	include("config.php");

	$_SESSION[page]="register";
	$i18n=I18N_loadLanguage(defaultLanguage);

	if (isset($_POST[BUT_register]))
		{
			$return=LOGIN_new($_POST[ED_userName],$_POST[ED_password],$_POST[ED_password2],PERM_TRANSLATOR,$_POST[ED_email]);
			
			switch ($return)
			{
				case LOGINERR_USEREXISTS: 
					{
						echo("<h3>$i18n[I18N_errorUserExists]</h3>");
						break;
					};

				case LOGINERR_PASSWORDSDONTMATCH:
					{
						echo("<h3>$i18n[I18N_passwords_dont_match]</h3>");
						break;
					};

				case LOGINERR_BADPASSWORD:
					{
						echo("<h3>$i18n[I18N_badPassword]</h3>");
						break;
					};
					
				case LOGINERR_USERNAMETOSHORT:
					{
						echo("<h3>$i18n[I18N_userNameToShort]</h3>");
						break;
					};
					
				case LOGINERR_USERADDED:
					{
						echo("<h3>$i18n[I18N_newUserAdded]</h3>");
						LOGIN_showLogin();
						return;
					};
				case LOGINERR_EMAILEMPTY:
					{
						echo("<h3>$i18n[I18N_emailIsEmpty]</h3>");
						LOGIN_showLogin();
						return;
					};

			}
		};

	echo("<center><span class=\"title\">$i18n[I18N_registerANewAccount]</span></center>\n");

	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);
	echo("
	<tr>
		<td>$i18n[I18N_login_name]</td>
		<td><INPUT type=\"text\" name=\"ED_userName\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_password]</td>
		<td><INPUT type=\"password\" name=\"ED_password\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_repeated_password]</td>
		<td><INPUT type=\"password\" name=\"ED_password2\"></td>
	</tr>
	<tr>
		<td>$i18n[I18N_email]</td>
		<td><INPUT type=\"text\" name=\"ED_email\"></td>
	</tr>
	<tr>
		<td colspan=\"2\" align=\"center\">
			<INPUT type=\"submit\" name=\"BUT_register\" value=\"$i18n[I18N_register]\">
		</td>
	</tr>
	");
	HTML_showTableEnd(true);
	HTML_showFormEnd();
};





/**
**name LOGIN_logout()
**description Logs out the user
**/
function LOGIN_logout()
{
	include("config.php");
	$_SESSION = array();
	SESS_end();
};





/**
**name LOGIN_getUserParams($userName,$type)
**description Returns the value of a parameter for a selected user name.
**parameter userName: the name of the user
**returns the value of the parameter
**/
function LOGIN_getUserParams($userName,$type)
{
	include("config.php");

	$sql="SELECT `$type` FROM `".tablePrefix."logins` WHERE `name` = '$userName'";
	$res=DB_query($sql);
	$row = mysql_fetch_assoc($res);
	return($row[$type]);
}





/**
**name LOGIN_getUserID($userName)
**description Returns the user ID for a selected user name.
**parameter userName: the name of the user
**returns user ID
**/
function LOGIN_getUserID($userName)
{
	return(LOGIN_getUserParams($userName,"id"));
}





/**
**name LOGIN_checkPermission($perm,$userName="")
**description Checks if the user has a selected permission.
**parameter perm: permission value to check
**parameter userName: the name of the user
**returns true, if the user has the selected permission. false, if not.
**/
function LOGIN_checkPermission($perm,$userName="")
{
	if (empty($userName))
		$userName = $_SESSION[userName];

	return((LOGIN_getUserParams($userName,"permissions") & $perm) == $perm);
}





/**
**name LOGIN_showUsers()
**description Shows a list of the user with the possibillity to change permissions and delete users.
**/
function LOGIN_showUsers()
{
	include("config.php");
	$i18n=I18N_loadLanguage();
	
	if (isset($_POST[BUT_change]))
		{
			$sql="SELECT id FROM `".tablePrefix."logins`";
			$res=DB_query($sql);
			while ($row = mysql_fetch_assoc($res))
			{
				//calculate new permission value
				$perm = $_POST["CB_$row[id]trans"] + $_POST["CB_$row[id]orig"] + $_POST["CB_$row[id]admin"];

				//update the permissions
				$updateSQL="UPDATE `".tablePrefix."logins` SET `permissions` = '$perm' WHERE `id` =$row[id]";
				DB_query($updateSQL);

				//check, if the user should be deleted
				if ($_POST["CB_$row[id]del"] == 1)
					{
						$deleteSQL="DELETE FROM `".tablePrefix."logins` WHERE `id` =$row[id]";
						DB_query($deleteSQL);
					}
			};
		};

	$sql="SELECT id ,name, (permissions & ".PERM_ADMIN.") = ".PERM_ADMIN." AS admin, (permissions & ".PERM_UPLOADER.") = ".PERM_UPLOADER." AS original, (permissions & ".PERM_TRANSLATOR.") = ".PERM_TRANSLATOR." AS translator FROM `".tablePrefix."logins`";
	$res=DB_query($sql);

	echo("<center><span class=\"title\">$i18n[I18N_userManagement]</span></center>\n");

	HTML_showFormHeader(strip_tags(SID));
	HTML_showTableHeader(true);

	echo("<tr>
		<td><span class=\"subhighlight\">$i18n[I18N_login_name]</span></td>
		<td><span class=\"subhighlight\">$i18n[I18N_translator]</span></td>
		<td><span class=\"subhighlight\">$i18n[I18N_originalAuthor]</span></td>
		<td><span class=\"subhighlight\">$i18n[I18N_administrator]</span></td>
		<td><span class=\"subhighlight\">$i18n[I18N_delete]</span></td>
		</tr>");

	while ($row = mysql_fetch_assoc($res))
	{
		$checkT = $checkA = $checkO = "";

		if ($row[translator])
			$checkT="checked";
		if ($row[admin])
			$checkA="checked";
		if ($row[original])
			$checkO="checked";

		echo("<tr>
		<td>$row[name]</td>
		<td align=\"center\"><input type=\"checkbox\" name=\"CB_$row[id]trans\" value=\"".PERM_TRANSLATOR."\" $checkT></td>
		<td align=\"center\"><input type=\"checkbox\" name=\"CB_$row[id]orig\" value=\"".PERM_UPLOADER."\" $checkO></td>
		<td align=\"center\"><input type=\"checkbox\" name=\"CB_$row[id]admin\" value=\"".PERM_ADMIN."\" $checkA></td>
		<td align=\"center\"><input type=\"checkbox\" name=\"CB_$row[id]del\" value=\"1\"></td>
		</tr>");
	};

	echo("<tr>
		<td colspan=\"5\" align=\"center\">
			<input type=\"submit\" name=\"BUT_change\" value=\"$i18n[I18N_makeChange]\">
		</td>
		</tr>");

	HTML_showTableEnd(true);
	HTML_showFormEnd();
};
?>