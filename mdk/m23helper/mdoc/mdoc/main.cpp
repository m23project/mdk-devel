/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sam Mai 17 10:03:17 CEST 2003
    copyright            : (C) 2003 by Hauke Goos-Habermann
    email                : hhabermann@gss-netconcepts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STARTNEWFILE "section"
#define STARTNEWBLOCK "subsection"
#define MDOCVER "0.4.8"

//convert special characters so latex can use them
void convertChar(char *in)
{
 char buffer[10000];

 memset(buffer,0,sizeof(buffer));

 for (unsigned int i=0; i < strlen(in); i++)
  {
    switch(in[i])
      {
        case '_':
          {
           strcat(buffer,"\\_");
           break;
          }
        case '&':
          {
           strcat(buffer,"\\&");
           break;
          }
        case '$':
          {
           strcat(buffer,"\\$");
           break;
          }
        default:
          {
           buffer[strlen(buffer)]=in[i];
           break;
          };
      };
  };
 strcpy(in,buffer);
}


//checks if the file has the mdoc information block where you can leave comments and stores in the documentation file
bool copyMdocInfo(char *fileName, FILE *outFile)
{
  FILE *pin,
       *file;
  char cmd[100],
       line[10000],
       *elem;
  bool reading=false;     

  memset(cmd,0,sizeof(cmd));
  memset(line,0,sizeof(line));

  sprintf(cmd,"grep \"\\\\\\*\\$mdocInfo\" %s",fileName);

  pin=popen(cmd,"r");

  memset(cmd,0,sizeof(cmd));

  fgets(cmd, sizeof(cmd), pin);

  pclose(pin);

  if (strlen(cmd)>0)
    {
     file=fopen(fileName,"r");

     while (!feof(file))
           {
            fgets(line,sizeof(line), file);

            //start reading
            if (strstr(line, "/*$mdocInfo")!=0)
                {
                 reading=true;
                 //skip the "/**mdocInfo" line
                 fgets(line,sizeof(line), file);
                }
                
            //stop reading
            if (strstr(line, "$*/")!=0)
               {
                fclose(file);
                break;
               };    

            if (reading)
               {
                elem=strchr(line,'\n');
                if (elem != NULL)
                    *elem=0;
                    
                convertChar(line);
                fprintf(outFile,"%s\\\\",line);
               }

            
           }    
    }
}

//checks is a file has usable coments
bool hasComments(char *fileName)
{
  FILE *pin;
  char cmd[100];

  memset(cmd,0,sizeof(cmd));
  
  sprintf(cmd,"grep \"\\*\\*name\" %s",fileName);  

  pin=popen(cmd,"r");

  memset(cmd,0,sizeof(cmd));

  fgets(cmd, sizeof(cmd), pin);

  pclose(pin);

  return(strlen(cmd)>0);  
}





//adds a parameter to a variable
void addNewParam(char *dest,char *line, char *pre, char *post)
{
 char buffer[20000];
 unsigned int pos=0,
              start;
 register unsigned int i;              

 memset(buffer,0,sizeof(buffer));

 //eliminate the newline
 line[strlen(line)-1]=0;

 //find the place to start copying letters from
 start=strchr(line,' ')-line;

 //line seems to begin with a blank letter, so search for the second blank to search for the comment
 if (start < 6)
   start=strchr(line+6,' ')-line;
 

 for (i=start; i < strlen(line); i++)
  {
   buffer[pos]=line[i];
   pos++;
  };

 if (pre != NULL)
     strcat(dest,pre);
 strcat(dest,buffer);
 if (post != NULL)
     strcat(dest,post);
}





//writes the header for the tex file
void writeHeader(FILE *file)
{  
 fprintf(file,"\\documentclass[10pt,a4paper]{book}\n\
\\usepackage[latin1]{inputenc}\n\
\\usepackage{amsmath}\n\
\\usepackage{amsfonts}\n\
\\usepackage{amssymb}\n\
\\begin{document}\n\
\n");
}





//writes the tail for the tex file
void writeTail(FILE *file)
{
 fprintf(file,"\\end{document}\n");
}




//writes a info block to the tex file
void writeBlock(FILE *file,char *name,char *description,char *parameter, char *returns)
{
 char texparam[5000];
    
 convertChar(name);
 convertChar(description);
 convertChar(parameter);
 convertChar(returns);

 memset(texparam,0,sizeof(texparam)); 

 if (strlen(parameter) > 0)
 sprintf(texparam,"\\begin{itemize}\n\
%s\n\
\\end{itemize}\n",parameter);


  
 fprintf(file,"\\%s%s\n\
\\textbf{Description:}\\\\\n\
%s\n\
%s\n\
\\textbf{Return value:}\\\\\n\
%s\n",STARTNEWBLOCK,name,description,texparam,returns);
}




//scans a file for mdoc lines
void scanFile(char *fileName, FILE *outFile)
{
 FILE *file;
 char line[5000],
      description[50000],
      name[1000],
      parameter[10000],
      returns[5000];
      
 bool reading=false;

 memset(name,0,sizeof(name));
 memset(parameter,0,sizeof(parameter));
 memset(description,0,sizeof(description));
 memset(returns,0,sizeof(returns)); 

 file=fopen(fileName,"rt");
 if (file==NULL)
  return;

 convertChar(fileName);
 fprintf(outFile,"\\%s{%s}\n",STARTNEWFILE,fileName);

 //scan for the mdoc info block
 copyMdocInfo(fileName,outFile);

 while (!feof(file))
  {
   fgets(line,sizeof(line), file);

   //start reading
   if (strstr(line, "/**")!=0)
       reading=true;
       
   //try to detect the kind of information
   //**name name of function
   //**parameter parameter
   //**description description
   
   if (reading)
      {
       if (strstr(line,"**name"))
            {
             addNewParam(name,line,"{","}");
            };
       if (strstr(line,"**parameter"))
            {
             addNewParam(parameter,line,"\\item","\n");
            };
       if (strstr(line,"**description"))
            {
             addNewParam(description,line,NULL,"\n");
            };
       if (strstr(line,"**returns"))
            {
             addNewParam(returns,line,NULL,"\\\\\n");
            };

            
        }
        



   //stop reading
   if (strstr(line, "**/")!=0)
      {
       writeBlock(outFile,name,description,parameter,returns);
       memset(name,0,sizeof(name));
       memset(parameter,0,sizeof(parameter));
       memset(description,0,sizeof(description));
       reading=false;
      }; 

  }; 

 fprintf(outFile,"\\newpage\n");
 fclose(file);
}




void scanFiles(char *dir, char *outFileName)
{
  FILE *outFile,
       *pin;
  char fileName[1000],
       cmd[200],
       *c;
  
  outFile=fopen(outFileName,"w");

//  writeHeader(outFile);

  sprintf(cmd,"find %s -type f",dir);
  
  pin=popen(cmd,"r");

  while (!feof(pin))
    {
     memset(fileName,0,sizeof(fileName)); 
     fgets(fileName,sizeof(fileName),pin);

     if (feof(pin))
        break;
     
     c=strchr(fileName,'\n');
     if (c!=NULL)
        (*c)='\0';

     printf("checking for comments: %s \n",fileName);
             
     if (hasComments(fileName))
        {
         printf("scanning file: %s \n",fileName);
         scanFile(fileName,outFile);         
        }; 
    };

//  writeTail(outFile);
    
  fclose(outFile);
  
  pclose(pin);

  printf("done\noutput written to: %s \n",outFileName);
}





//shows a help screen
void showHelp()
{
  printf("usage:  mdoc <start directory> <tex output file>\n\
\tstart directory: directory to start search for files\n\
\ttex output file: filename the latex output file should be saved to\n");
}




int main(int argc, char *argv[])
{
  printf("mDoc %s - the automatic documentation tool\n\n",MDOCVER);
  if (argc != 3)
    {
      showHelp();
      return(23);
    }
    
  scanFiles(argv[1],argv[2]);

  return EXIT_SUCCESS;
}
