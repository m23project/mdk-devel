<?php

//Called by /mdk/m23Debs/bin/mkm23Deb

include("/m23/inc/capture.php");
include("/m23/inc/db.php");
dbConnect();

//fields to export from sourceslist
$exportFields = array("name", "distr", "release", "description", "list", "desktops", "archs");

if ($argc > 1)
{
	$allowedNames = $argv;
}
else
{
	//names of sourceslists that should get exported
	$allowedNames = array("imaging", "Linux Mint 9 KDE", "HS-Fedora14", "HS-opensuse11.4", "squeeze", "squeeze+libreoffice", "HS-centos62", "precise", "precise+Xorg-updates", "Linux Mint 13 Maya", "wheezy", "trusty", "Linux Mint 17 Qiana", "elementary os", "jessie", "Linux Mint 17.1 Rebecca", "Linux Mint 17.2 Rafaela", "Linux Mint 17.3 Rosa");
}

$exportSql = $fieldSql = "";

//SQL string of all fields 
foreach ($exportFields as $field)
	$fieldSql .= "`$field`, ";
$fieldSql = rtrim($fieldSql,", ");

//create SQL query to export all fields from sourceslist
$exportSql .= "SELECT $fieldSql FROM sourceslist";

$result=db_query($exportSql);

//lock table for write operations
$out = "LOCK TABLES `sourceslist` WRITE;\n";
while ($row = mysqli_fetch_assoc($result))
{
	//check if the sourceslist should get exported
	if (in_array($row["name"],$allowedNames))
		{
			//delete sourceslist with the same name
			$out .= "DELETE FROM `sourceslist` WHERE name=\"$row[name]\";\n";

			//export data
			$out .= "INSERT INTO `sourceslist` ($fieldSql) VALUES (";
			foreach ($exportFields as $field)
				$out .= "'$row[$field]', ";
			$out = rtrim($out,", ");
			$out .= ");\n";
		};
};
$out .= "UNLOCK TABLES;";

echo($out);
?>
