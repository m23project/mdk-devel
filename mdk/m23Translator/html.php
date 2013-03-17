<?PHP
/*$mdocInfo
 Author: Hauke Goos-Habermann (HHabermann@pc-kiel.de)
 Description: functions for generating often used HTML code
$*/





/**
**name HTML_listSelection($selName,$list,&$first,$firstName="")
**description shows a selection with options stored in an array
**parameter selName: name of the selection
**parameter list: array with the entries. The array can be a simple numeric array or an associative array with discrete entries for the shown name and the value. e.g. : $l[name0]="public"; $l[val0]="internal"; $l[name1]="public1"; $l[val1]="internal1"; public and public1 will be shown the user in the webbrowser, while internal and internal1 are the values that are transfered to the server.
**parameter first: entry that should be shown first (this is the internal value and NOT the name shown to the user). the first value from the list will be written to $first. set first to "false" to disable writing the first entry.
**parameter firstName: if you want to use the associative array variant and a first value, you need to set the name that should be shown to the user. This name is stored in firstName
**/
function HTML_listSelection($selName,$list,&$first,$firstName="")
{
	$out="<SELECT name=\"$selName\" size=\"1\">\n";

	if (!isset($list[name0]))
		{
			$normalArray = true;
			$amount = count($list);
		}
	else
		{
			$normalArray = false;
			$amount = count($list) / 2;
		}

	if (!empty($first))
		{
			if (!empty($firstName))
				$out.="<option value=\"$first\">$firstName</option>\n";
			else
				{
					if ($normalArray)
						$out.="<option value=\"$first\">$first</option>\n";
					else
						{
							for ($i=0; $i < $amount; $i++)
								if ($list["val$i"] == $first)
									{
										$out.="<option value=\"$first\">".$list["name$i"]."</option>\n";
										break;
									};
						};
				};
		};

	for ($i=0; $i < $amount; $i++)
		if ((($first != $list[$i]) && $normalArray) ||
			(($first != $list["val$i"]) && !$normalArray))
			{
				if ($normalArray)
					$out.="<option value=\"".$list[$i]."\">".$list[$i]."</option>\n";
				else
					$out.="<option value=\"".$list["val$i"]."\">".$list["name$i"]."</option>\n";
			};

	$out.="</SELECT>\n";

	if ((!($first === false)) && empty($first))
		if ($normalArray)
			$first = $list[0];
		else
			$first = $list[val0];
	
	return($out);
}





/**
**name HTML_showTableHeader($centerTable)
**description prints the header of a shadowed table
**parameter centerTable: set to true if the table should be centered vertically
**/
function HTML_showTableHeader($centerTable=false)
{
if ($centerTable)
echo("<table width=\"100%\" height=\"100%\">
	<tr valign=\"center\" height=\"100%\">
		<td>");
		
	echo("
<table align=\"center\"><tr><td><div class=\"subtable_shadow\">
<table class=\"subtable\" align=\"center\">
	<tr>
		<td>
");
}





/**
**name HTML_showTableEnd($centerTable)
**description prints the end of a shadowed table
**parameter centerTable: set to true if the table should be centered vertically
**/
function HTML_showTableEnd($centerTable=false)
{
echo("
		</td>
	</tr>
</table></div></td><tr></table>
");
if ($centerTable)
	echo("</td>
	</tr>
	</table>");
};





/**
**name HTML_showFormHeader($addAction="",$method="post")
**description Shows the header of a formular
**parameter addAction: set it, if additional parameters to index.php should be used
**parameter method: default is POST, but you can set it to GET
**/
function HTML_showFormHeader($addAction="",$method="post",$extra="")
{
echo("<form method=\"$method\" $addAction>
	<input type=\"hidden\" name=\"SESS_ID\" value=\"".SESS_getID(true)."\">
	$extra
	");
}





/**
**name HTML_showFormEnd()
**description Shows the end of a formular
**/
function HTML_showFormEnd($extra="")
{
echo("$extra
</form>
	");
}
?>