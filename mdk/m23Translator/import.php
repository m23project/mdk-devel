<?php
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: Tool and funtions for importing articles in m23/Translator.
$*/





	include("trans.php");
	include("login.php");
	include("i18n.php");
	include("page.php");
	include("lang.php");
	include("db.php");
	include("config.php");

	DB_connect();

	if (LOGIN_check($_POST[u],$_POST[p]) == LOGINERR_USERVALID)
		{
			$original = $_POST[t] == "o";
			$lID=LANG_getID( $_POST[l]);

			if ($lID == LANGERR_LANDDOESNTEXIST)
				{
					echo("Error: language doesn't exists!\n");
					exit(2);
				}

			$uID=LOGIN_getUserID($_POST[u]);
			
			if ($original)
				{
					if (!LOGIN_checkPermission(PERM_UPLOADER,$_POST[u]))
						{
							echo("Error: You don't have needed original author privileges!\n");
							exit(3);
						}

					$derivedfrom = -1;
					$status = TRANS_ORIGINAL;
					TRANS_createDirectory($_POST[d]);
				}
			elseif (!LOGIN_checkPermission(PERM_TRANSLATOR,$_POST[u]))
					{
						echo("Error: You don't have needed translator privileges!\n");
						exit(4);
					}
			
			echo("Directory: $_POST[d]\n");
				
	
			for ($fileNr=0; $fileNr < $_POST[fileNr]; $fileNr++)
			{
				//make sure the file exists
				if ($original)
					{
						TRANS_touchFile($_POST["f$fileNr"],$_POST[d]);
						$fID=TRANS_getFileID($_POST["f$fileNr"],$_POST[d]);
						TRANS_addOriginal($fID, $uID, $lID, urldecode($_POST["a$fileNr"]));
						echo("\t".$_POST["f$fileNr"]." was uploaded as original\n");
					}
				else
					{
						$fID=TRANS_getFileID($_POST["f$fileNr"],$_POST[d]);
						//get all original articles and pick the one with the highest tID
						$originals=TRANS_getOriginals($fID);
						$tIDs=array_keys($originals);
						rsort($tIDs);

						TRANS_addTranslation($fID, $tIDs[0], $_POST[u], $lID, LANG_getID($originals[$tIDs[0]]), urldecode($_POST["a$fileNr"]),true);
						echo("\t".$_POST["f$fileNr"]." was uploaded as translation\n");
					}
			};
		}
	else
		{
			echo("Access denied\n");
			exit(5);
		};
?>