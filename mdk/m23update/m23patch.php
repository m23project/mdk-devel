<?php

	include("/home/groups/m/m2/m23/db.php");

	//retrurns the contents of a file
	function rFile($fName)
	{
		$text="";

		if (file_exists($fName))
			{
				$file=fopen($fName,"r");
				$text=fread($file,100000);
				fclose($file);
			};

		return($text);
	};

	$patchDir="/home/groups/m/m2/m23/htdocs/m23patch/0.5.0";
// 	$patchDir="/matrix23/arbeit/wwwTests/0.5.0";

	chdir($patchDir);

	$pipe=popen("find -type f -printf \"%f\\n\" | grep info | sort","r");

	switch ($_GET[action])
		{
			case "info":
				{
					echo("<ul>");
					break;
				}

			case "cmd":
				{
					echo("#!/bin/sh\n");
					break;
				}
		}

	$start = $end = "";

	while ($fileName=fgets($pipe))
		{
			$temp=split("\.",$fileName);
			$ver=$temp[0];

			if ($ver > $_GET[patch])
				{
					switch ($_GET[action])
						{
							case "cmd":
								{
									$start.="\n".rFile($ver.".start");
									$end.="\n".rFile($ver.".end");
									break;
								};

							case "info":
								{
									$infoFile=$ver.".info";
									$infoText="<br>".rFile($infoFile);
									echo("<li>$fileName $infoText</li>\n");
									break;
								}
						}
				};
		};
		
	switch ($_GET[action])
		{
			case "info":
				{
					echo("</ul>");
					break;
				}

			case "cmd":
				{
					echo("cd /
$start

if test `grep http://m23.sourceforge.net/m23inst/ /etc/apt/sources.list | grep ^deb -c` -eq 0
then
	echo \"deb http://m23.sourceforge.net/m23inst/ ./\" >> /etc/apt/sources.list
fi

if [ `grep \"Acquire::http::Proxy\" /etc/apt/apt.conf -c` -gt 0 ]
then
 proxyIP=`grep \"Acquire::http::Proxy\" /etc/apt/apt.conf | cut -d'\"' -f2 | cut -d'/' -f3 | cut -d':' -f1`
 ping -c 1 \"\$proxyIP\" > /dev/null
 if [ $? -ne 0 ]
 then
  grep -v \"Acquire::http::Proxy\" /etc/apt/apt.conf > /tmp/apt.conf
  cat /tmp/apt.conf > /etc/apt/apt.conf
  rm /tmp/apt.conf
 fi
fi



export DEBIAN_FRONTEND=noninteractive
apt-get -y --force-yes update


for pkg in `dpkg --get-selections | grep m23 | grep -v  m23hwscanner | grep -v m23-initscripts | grep -v deinstall$ | tr -d [:blank:] | sed 's/install$//g' | awk -v ORS='' '{print($0\" \")}'`
do
	apt-get -y --force-yes install \$pkg
	while `test $? -ne 0`
	do
		apt-get -y --force-yes install \$pkg
	done
done


$end\n");
		
			$dbConnection=dbConnect();
			
			if ($dbConnection === false)
				return(false);

			$sql="INSERT INTO `m23update` (`oldversion` , `newversion` , `time`, `type`, `host` )
VALUES ('".$_GET['patch']."', '$ver', '".time()."','m23patch','".getenv("REMOTE_ADDR")."');";

			$result=mysql_query($sql);

			mysql_close($dbConnection);
			break;
		};
	};
		
	pclose($pipe);
?>