
BEGIN{
	PARAMOPEN=0;
	RETURNSOPEN=0;
	MDOCINFOSTART=0;
	FILENAMESHOWN=0;
	fname=""
}

#set if a mdoc entry ends
/\$\*\//{
MDOCINFOSTART=0;
}


{
	#translation table for converting special characters to LaTeX
	gsub("\\&","\\\\&");
	gsub("[$]","\\$");
	gsub("[_]","\\_");
	gsub("[#]","\\#");
	gsub("[>]","$>$");
	gsub("[<]","$<$");
	
	#make a new section if a new file is scanned
	if (FILENAME != fname)
		{
			#close opened lists
			if (PARAMOPEN || RETURNSOPEN)
			{
				print("\\end{itemize}");
				PARAMOPEN=0;
				RETURNSOPEN=0;
			}

			fname=FILENAME;
			#set back the variables
			FILENAMESHOWN=0;
			PARAMOPEN=0;
			RETURNSOPEN=0;
			MDOCINFOSTART=0;
		}
	
# 	if (MDOCINFOSTART)
# 		{
# 			#show the filename as a new section if it has not been show before
# 			if (!FILENAMESHOWN)
# 			{
# 				print("\n\\newpage\\section{"FILENAME"}");
# 				FILENAMESHOWN=1;
# 			}	
# 
# 			#show mdoc header information
# 			print($0"\\\\");
# 		}
}

#set if a mdoc entry starts
# /\/*\$mdocInfo/{
# MDOCINFOSTART=1;
# }


/^\\#[ ]?name.*/{
	if (PARAMOPEN || RETURNSOPEN)
		{
			print("\\end{itemize}");
			PARAMOPEN=0;
			RETURNSOPEN=0;
		}
	print("\n\\subsection{"$3"}")
}

/^\\#[ ]?description/{
	if (PARAMOPEN || RETURNSOPEN)
		{
			print("\\end{itemize}");
			PARAMOPEN=0;
			RETURNSOPEN=0;
		}
	split($0, arr, /^\\#[ ]?[a-z]*[ ]?/);
	print("\\textbf{Description:} "arr[2])"\\\\"
}

/^\\#[ ]?parameter/{
	if (RETURNSOPEN)
		{
			print("\\end{itemize}");
			RETURNSOPEN=0;
		}
		
	if (!PARAMOPEN)
		{
			print("\\textbf{Parameter:}\n\\begin{itemize}");
			PARAMOPEN=1;
		}
	split($0, arr, /^\\#[ ]?[a-z]*[ ]?/);
	print("\\item "arr[2])
}

/^\\#[ ]?returns/{
	if (PARAMOPEN)
		{
			print("\\end{itemize}");
			PARAMOPEN=0;
		}

	if (!RETURNSOPEN)
		{
			print("\\textbf{Returns:}\n\\begin{itemize}");
			RETURNSOPEN=1;
		}
	split($0, arr, /^\\#[ ]?[a-z]*[ ]?/);
	print("\\item "arr[2])
}

END{
	if (PARAMOPEN || RETURNSOPEN)
	{
		print("\\end{itemize}");
		PARAMOPEN=0;
		RETURNSOPEN=0;
	}
}