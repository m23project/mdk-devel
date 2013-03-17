#!/usr/bin/php

<?php

	$indexFile="/tmp/knxidx";
	$knoppixPath="http://developer.linuxtag.net/knoppix/i386/";
	$search=$argv[1];

	if ($argc != 2)
		{
			fputs(STDERR,"usage: $argv[0] <package search string>\n");
			exit(24);
		};
	
	//check, if the index file has to be downloaded
	// - doesn't exists
	// - is older than 1 hour
	if (!file_exists($indexFile) || ((time() - filectime($indexFile)) > 3600))
		exec("wget -q $knoppixPath -O$indexFile");

	$pipe=popen("grep $search $indexFile | grep deb | cut -d'\"' -f8","r");
	
	$i=0;
	while ($line = fgets($pipe))
		{
			//$line=hwdata-knoppix_0.107-5_all.deb
			$temp=split("_",$line);

			$fullVersionNr=$temp[1];
			//$fullVersionNr=0.107-5
			
			$temp=split("-",$fullVersionNr);
			$patchLevel=$temp[1];
			//$patchLevel=5
			$temp=$temp[0];
			$temp=split("\.",$temp);
			$major=$temp[0];
			//$major=0
			$minor=$temp[1];
			//$minor=107
			
			//fill with leading '0' to make every number 6 digits long
			$patchLevel=str_repeat("0",6-strlen($patchLevel)).$patchLevel;
			$major=str_repeat("0",6-strlen($major)).$major;
			$minor=str_repeat("0",6-strlen($minor)).$minor;
			
			//store the full version number + appended file name in the array
			$arr[$i++]="$major$minor$patchLevel###$line";
		};

	pclose($pipe);
	
	if (count($arr) <= 0)
		{
			fputs(STDERR,"No matching ($search) debs found\n");
			exit(23);
		};

	//entry with the highest full version number stands on top
	rsort($arr);
	$temp=split("###",$arr[0]);
	
	exec("rm $search* 2> /dev/null
	wget $knoppixPath/$temp[1]");
	
	print($temp[1]);
?>