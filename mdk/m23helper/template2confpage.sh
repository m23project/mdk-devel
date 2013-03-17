#!/bin/bash

mkdir -p out

ls *.templates 2> /dev/null | while read templateFile
do
	package=`echo $templateFile | cut -d'_' -f1`
	outFile="out/$package""OptionPage.php"
	
	echo "> $package"

cat $templateFile | awk -vPACKAGE="$package" '

function output()
{
	if (length(var) > 0)
	{
		#Print the type of the template
		print("$elem[\""var"\"][\"type\"]=\""type"\";");

		#Print options for select and multiselect templates
		if (type == "select" || type == "multiselect")
		{
			#Print possible values for select/multiselect templates
			amount=split(choices,tmp,", ");
			for (i=1; i < amount; i++)
			{
				print("$elem[\""var"\"][\"choices\"]["i"]=\""tmp[i]"\";");
			}

			#Print possible German translation for select/multiselect templates
			amount=split(choicesde,tmp,", ");
			for (i=1; i < amount; i++)
			{
				print("$elem[\""var"\"][\"choicesde\"]["i"]=\""tmp[i]"\";");
			}

			#Print possible French translation for select/multiselect templates
			amount=split(choicesfr,tmp,", ");
			for (i=1; i < amount; i++)
			{
				print("$elem[\""var"\"][\"choicesfr\"]["i"]=\""tmp[i]"\";");
			}
		}

		#Add descriptions
		print("$elem[\""var"\"][\"description\"]=\""description"\";");
		print("$elem[\""var"\"][\"descriptionde\"]=\""descriptionde"\";");
		print("$elem[\""var"\"][\"descriptionfr\"]=\""descriptionfr"\";");

		#Add default value
		print("$elem[\""var"\"][\"default\"]=\""default2"\";");

		#Clear all values
		default2=description=descriptionde=descriptionfr=choicesde=choicesfr=choices=type="";
		descriptionadd=descriptiondeadd=descriptionfradd=0;
	}
}





BEGIN {
	print("<?php\n\
include (\"/m23/inc/packages.php\");\n\
include (\"/m23/inc/checks.php\");\n\
include (\"/m23/inc/client.php\");\n\
include (\"/m23/inc/capture.php\");\n\n\
$params = PKG_OptionPageHeader2(\""PACKAGE"\");\n");
}





#Make " safe by converting to \" in the input
{
	gsub("\"","\\\"");
}





#Description lines start with a " ". So disable adding of description lines, if there is no " " at the beginning.
/^[^ ]+/ {
	descriptionadd=descriptiondeadd=descriptionfradd=0
}





#Get the variable name
/Template:/ {
	output();
	split($0,tmp," ");
	var=tmp[2];
}





#Get the type of the template (string, boolean, select or password)
/Type:/ {
	split($0,tmp," ");
	type=tmp[2];
	descriptionadd=descriptiondeadd=descriptionfradd=0
}





#Get possible values for select/multiselect templates
/Choices:/ {
	gsub("Choices: ","");
	choices=$0;
	descriptionadd=descriptiondeadd=descriptionfradd=0
}

#Get possible German translation for select/multiselect templates
/Choices-de/ {
	gsub("Choices-de.*: ","");
	choicesde=$0;
	descriptionadd=descriptiondeadd=descriptionfradd=0
}

#Get possible French translation for select/multiselect templates
/Choices-fr/ {
	gsub("Choices-fr.*: ","");
	choicesfr=$0;
	descriptionadd=descriptiondeadd=descriptionfradd=0
}





#Check if there is a description and remove "Description*" from the first line
/Description:/ {
	gsub("Description: ","");
	descriptionadd=1
}

#Start of German description
/Description-de/ {
	gsub("Description-de.*: ","");
	descriptiondeadd=1
}

#Start of French description
/Description-fr/ {
	gsub("Description-fr.*: ","");
	descriptionfradd=1
}

#Add the description lines to the correct languages
{
	if (descriptionadd == 1)
		description=description$0"\n"

	if (descriptiondeadd == 1)
		descriptionde=descriptionde$0"\n"

	if (descriptionfradd == 1)
		descriptionfr=descriptionfr$0"\n"
}





#Get the default value
/Default:/ {
	gsub("Default: ","");
	default2=$0
}





END {
	output();
	print("PKG_OptionPageTail2($elem);\n\
?>");
}' > $outFile

done