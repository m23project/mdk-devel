<?php

$I18N_login_name="Login name";
$I18N_password="Password";
$I18N_repeated_password="Password (repeated)";
$I18N_login="Login";
$I18N_register="Register";
$I18N_registerANewAccount="Register a new account";
$I18N_errorUserExists="Error: User name exists!";
$I18N_passwords_dont_match="Error: Passwords don't match!";
$I18N_newUserAdded="New user added";
$I18N_userUnknownOrWrongPassword="Error: User unknown or wrong password!";
$I18N_logout="Logout";
$I18N_badPassword="Error: Bad password!";
$I18N_userNameToShort="Error: User name is to short (3 characters minimum)!";
$I18N_language="Language";
$I18N_fullLanguageName="Full language name";
$I18N_shortLanguageName="Short language name";
$I18N_add="Add";
$I18N_addNewLanguage="Add a new language";
$I18N_overview="Translation overview";
$I18N_languageAdded="Language was added";
$I18N_fullLanguageNameExists="Error: Full language name exists allready!";
$I18N_shortLanguageNameExists="Error: Short language name exists allready!";
$I18N_originalArticle="Original article";
$I18N_createTranslation="Create";
$I18N_viewTranslation="View";
$I18N_notTranslated="Not translated";
$I18N_originalHasChanged="Original article has changed";
$I18N_adjustTranslation="Adjust";
$I18N_translationUpToDate="Translation is up to date";
$I18N_editedBy="Edited by";
$I18N_originalArticlesAreAvailableInTheseLanguages="Original article(s) is/are availabe in these languages";
$I18N_loadOriginalArticle="Load original article";
$I18N_translate="Translate";
$I18N_saveTranslationAndMarkAsUnfinished="Save translation and mark as UNFINISHED";
$I18N_saveTranslationAndMarkAsDone="Save translation and mark as DONE";
$I18N_translation="Translation";
$I18N_targetLanguage="Target language";
$I18N_changeTime="Modification time";
$I18N_thanksForTranslation="Thank you for the translation!";
$I18N_viewDifferencesBetweenTranslatedAndCurrentOriginal="View differences between translated and current original";
$I18N_differencesBetweenTranslatedAndCurrentOriginal="Differences between translated and current original";
$I18N_changeTimeTranslatedOriginal="Translated original modification time";
$I18N_changeTimecurrentOriginal="Current original modification time";
$I18N_administration="Administration";
$I18N_administrator="Administrator";
$I18N_originalAuthor="Original author";
$I18N_translator="Translator";
$I18N_delete="Delete";
$I18N_makeChange="Make Changes";
$I18N_userManagement="User management";
$I18N_loadOldTranslationIntoEditor="Load old translation into the editor";
$I18N_loadTranslatedArticle="Load translation";
$I18N_editOriginal="Change original article";
$I18N_save="Save";
$I18N_originalChanged="Original article was changed";
$I18N_noLanguagesDefined="No languages are defined.";
$I18N_email="eMail";
$I18N_emailIsEmpty="eMail address is empty.";
$I18N_deleteFile="Delete file";
$I18N_fileDeleted="File deleted";
$I18N_help="Help";
$I18N_HELP_showOverview="This page shows an overview of all present translations. The names of the directories are colored green (left column) the file names are in the same row and colored black. The other entries in the rows are translations or original texts. There are colored bubbles in front of these texts.<br>
<ul><li><b>Blue</b>: This is an original text that can be changed by an user with \"original author\" privileges only.</li>
<li><b>Green</b>: This is a translation that should be up to date.</li>  
<li><b>Yellow</b>: The original article has changed and this translation may need an adjustment.</li>  
<li><b>Red</b>: There is no translation for the original text.</li>  
</ul><br>
There are several possible actions (depending from the privileges of the current user and the status of the text):<br>
<ul><li><b>View</b>: Shows the current article in the language of the column.</li>
<li><b>Adjust</b>: Can be used to adjust the translation to the current original text and to make corrections.</li>
<li><b>Create</b>: If there is no translation for a file you can add one by using this link.</li><br></td></tr>";
$I18N_HELP_viewArticle="In this windows you can see the text in the selected language with some additional informations.";
$I18N_HELP_editArticle="This window is divided in to two parts. Above there is a field with the original article. If there is shown no text you can load it after selecting an original language and clicking on \"Load original article\".<br>
Differences between the last translated and the current original article are shown by clicking the appropriate link. This makes it easier for you to see text parts that have changed and have to adjusted in the translation.<br>
The editor window at the bottom shows the translation (if existing). Now you can enter the translation or adjustement of the translation into the editor. It is possible to load a previous tranlslation by selecting the date from the drop down list and clicking on the load button.<br>
Afterwards you can save your translation. You have to choose if the translation is finished (click on \"Save translation and mark as DONE\") or a partly translation only (click on \"Save translation and mark as UNFINISHED\").";
$I18N_HELP_editOriginal="Original articles can be edited by all users with \"Original author\" privileges.";
$I18N_HELP_showAddNew="You can add a new language in this dialog. Afterwards the new language is shown in the translation overview as additional column. This makes it possible to make translations in this language at once.";
//english time format: e.g. 2005-12-24 23:31:59
$I18N_timeFormat="%Y-%m-%d %T";
$I18N_version="Version";
$I18N_help="Help";
$I18N_HELP_introduction="<center><img src=\"gfx/logo.png\"></center><br>
m23/Translator is a wiki like tool to help people to translate texts in different languages over the internet. Translations are done like that:
<ul>
<li>An original author publishes a text (preferable in a language that can be understood by the most users) and makes it visible for all users.</li>
<li>A translator decides to translate this text.
<ul>
	<li>There is no translation for this text in the target language. Thus it has to be added by clicking on \"Create\". An editor window opens that accepts the first translation.</li>
	<li>The translation must be adjusted if there is allready a translation and the original text has changed. This can be done by clicking on \"Adjust\".</li>
</ul>
</ul>

<h4>Add a new language</h4>
If you are able to translate into a new language you can add this language by clicking on \"[Add a new language]\". This new language is available as a new column in the translation overview afterwards. Now translations can be made in this new language.

<h4>Hints for translations</h4>
<ul>
	<li>You should use the same layout (if possible) as the original text in your translation.</li>
	<li>Words in the texts can have special meanings and must not get translated. Alls words that seem \"odd\" should not get translated. In this case you should ask the original author.<br>
	E.g.:
		<ol>
			<li><pre>&lt;center></pre> This is a HTML tag and interpeted by the webbrowser. It must not get translated.</li>
			<li><pre>\$I18N_action=\"Action\";</pre> You only have to translate the word <i>Action</i>, but not <i>I18N_action</i>. The characters \$,=,\" and ; have to be left unchanged.</li>
			<li><pre>&lt;i>\"\$I18N_select\"&lt;/i></pre> You must not change anything here because <i>&lt;i></i> and <i>&lt;/i></i> are HTML tags and <i>\$I18N_select</i> is a placeholder.</li>
			<li><pre>&amp;rarr;</pre> This is a HTML peculiar and must not changed.</li>
			<li><pre>helpInclude(\"statusColors.inc\");</pre> This is a command for the m23 help system and must not be changed either.</li>
		</ol>
	</li>
</ul>
";
?>