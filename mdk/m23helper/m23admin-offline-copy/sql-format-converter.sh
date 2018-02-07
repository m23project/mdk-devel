#!/bin/bash

echo 'TRUNCATE `clientjobs`;' > /tmp/m23-new.sql

gunzip -c all.sql.gz | awk -vFS=', ' '
{
	showLine=1
}

/INSERT INTO `clientjobs`/{
	if ($7 == "NULL")
		temp="\"\""
	else
		temp=$7
	
	print($1", "$2", "$3", \"\", "$4", "$5", "$6", "temp", "$8)
	
	showLine=0
}


/*/INSERT INTO `clients`/{
print($1", "$2", "$3", "$4", "$5", "$6", "$7)
/*", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23", "$24", "$25", "$26", "$27", "$28", "$29", 0, "$30", "$31", "$32", "$33", "$34", "$35", "$36", "$37", "$38", "$39", "$40", "$41)*/
	showLine=0
}*/


{
	if (showLine == 1)
		print($0)
}
' >> /tmp/m23-new.sql

