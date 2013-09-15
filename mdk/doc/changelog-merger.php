#!/usr/bin/php
<?PHP

if ($argc != 4)
{
	echo("usage $argv[0] <development changelog> <release changelog> <merged changelog>\n");
	exit(1);
};

$changelogDevelFile = $argv[1];
$changelogReleaseFile = $argv[2];
$mergedChangelogFile = $argv[3];

if (!is_file($changelogDevelFile) || !file_exists($changelogDevelFile))
{
	echo("ERROR: Development changelog invalid: \"$changelogDevelFile\"\n");
	exit(2);
}

if (!is_file($changelogReleaseFile) || !file_exists($changelogReleaseFile))
{
	echo("ERROR: Release changelog invalid: \"$changelogDevelFile\"\n");
	exit(2);
}

if (file_exists($mergedChangelogFile))
{
	unlink($mergedChangelogFile);
}

//Read all files
$cl_devel = array_reverse(file($changelogDevelFile)); //Read the development version changelog into an array and reverse that array (oldest entry first)

$cl_release = array_reverse(file($changelogReleaseFile)); //Read the release version changelog into an array and reverse that array (oldest entry first)




//Prepare pieces of the files
$min_length = min(count($cl_devel), count($cl_release)); //find out which one contains fewer lines

for ($i=0; $i < $min_length; $i++)
{   //compare both arrays, find the number of the earliest entry which is different in both arrays
	if ($cl_release[$i] != $cl_devel[$i])
	{
		$different = $i;
		break;
	}
}


$common = array_reverse(array_slice($cl_release, 0, $different)); //This part is the same in both arrays, must be appended to the end of the file at the end of this script, each line written in a for loop

$cl_devel_new = array_reverse(array_slice($cl_devel, $different)); //new part of the development log, some lines may be identical to below array, newest entry first
$cl_release_new = array_reverse(array_slice($cl_release, $different)); //new part of the release log, some lines may be identical to above array, newest entry first



//Sort through new parts to get entries by date
//First for the development changelog
$devel_date_list = array(); //make empty list
 
foreach ($cl_devel_new as $cl_devel_new_line_num => $cl_devel_new_line)
{
	if (is_numeric($cl_devel_new_line[0])) //find dates
		$date = $cl_devel_new_line;
	elseif ($cl_devel_new_line[0] == "+") //filter out the lines with the +++++
		continue;
	else
		$devel_date_list[$date][] = $cl_devel_new_line; //put lines to the according dates
}

//Second for the release changelog
$release_date_list = array(); //make empty list
 
foreach ($cl_release_new as $cl_rel_new_line_num => $cl_rel_new_line)
{
	if (is_numeric($cl_rel_new_line[0])) //find dates
		$date = $cl_rel_new_line;
	elseif ($cl_rel_new_line[0] == "+") //filter out the lines with the +++++
		continue;
	else
		$release_date_list[$date][] = $cl_rel_new_line; //put lines to the according dates
}




//Now start the actual merging
$merged_date_list = $release_date_list; //take the 'release changelog new things list' as blueprint for the merged list

foreach ($devel_date_list as $devel_date => $devel_line_list) //go through development changelog dates
{
	if (array_key_exists($devel_date, $merged_date_list)) //if those are already in the new merged list, put all unique lines together
		$merged_date_list[$devel_date] = array_unique(array_merge($merged_date_list[$devel_date], $devel_date_list[$devel_date]));
	else
		$merged_date_list[$devel_date] = $devel_line_list; //if they are new, just add them to the list
}

//Sort the result
krsort($merged_date_list);



//create and open new file in append-mode
$merged_file = fopen($mergedChangelogFile, "a");

//write merged dates and entries
foreach ($merged_date_list as $merged_date => $merged_lines)
{
	fwrite($merged_file, $merged_date);
	foreach ($merged_lines as $merged_line_num => $merged_line)
		fwrite($merged_file, $merged_line);

	fwrite($merged_file, "+++++\n");
}


//write older dates
foreach ($common as $common_line_num => $common_line)
	fwrite($merged_file, $common_line); //append all lines which are common to both versions to the end of the file

fclose($merged_file); //close the file

//remove surplus plusses
$raw = (file($mergedChangelogFile));

$rawlen = count($raw);

$out = array();

for ($i = 0; $i < $rawlen; $i++)
{
	if ($raw[$i][0] == '+' && !is_numeric($raw[$i+1][0]))
	{continue;}
	else
	{$out[] = $raw[$i];}
}


$merged_file = fopen($mergedChangelogFile, "w");

foreach ($out as $nextline)
{
	fwrite($merged_file, $nextline);
}
fclose($merged_file);

?>