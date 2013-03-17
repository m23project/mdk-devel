/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Fre Feb  7 16:55:02 CET 2003
    copyright            : (C) 2003 by Hauke Habermann
    email                : HHabermann@gss-netconcepts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

 Limitations: Scans only hda, no scsi, no other devices.

 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
// #include <mysql/mysql.h>

//CAUTION: use '.' for US and ',' germany
#define CUT '.'

/* //global variables are evil, but I'm using them either ;)
MYSQL mysql;
*/
char cpu[100],
     mhz[100],
     nic[100],
     vga[100],
     scsi[100],
     isa[100],
     mem[100],
     hda[100];
    
int memi;
  /*   
//writes the catched data to the db     
void writeDB(char *partInfo, char *user, char *passw, char *server, char *client)
{
  char sql[12000];
  
  if (!mysql_connect(&mysql,server,user,passw))
  {
      fprintf(stderr, "Failed to connect to database: Error: %s\n",
          mysql_error(&mysql));
      exit(2);    
  }

  mysql_select_db(&mysql,"m23");

  memi=atoi(mem);
  memi/=1000; //convert from kb to mb

  //FIXME: only works if only hda is scanned, value hd
  sprintf(sql,"UPDATE clients SET partitions='%s', cpu='%s', netcards='%s', graficcard='%s', memory='%i', hd='%s' WHERE client='%s'",partInfo,cpu,nic,vga,memi,hda,client);

  mysql_query(&mysql, sql);

  mysql_close(&mysql);
}
   */
//reads info from comand-line-arguments
void readInfo(char *query, char *out)
{
  FILE *pin;
  
  pin=popen(query,"r");

  fgets(out, 99, pin);

  fclose(pin);
}
//creates the  /etc/fstab file or overwrites it if it exists and creates a header in the file
//creation failed return=0 else 1
int createfstabheader()
{
    FILE *fstab;
    fstab=fopen("/home/test/etc/fstab","wt");
    if (fstab==NULL) return -1;
    fprintf(fstab,"%s\n%s\n","# /etc/fstab file system info, created by the m23 project","#  <file system> <mount point>  <type> <options>     <dump>  <pass>");
    fclose(fstab);
    return 1;
}
    

void genParamStr(char *in, char *out)
{
  char *begin, *end,
       partStart[50],
       partEnd[50],
       partFS[50],
       partType[50],
       partNr[4];
  int  partSize;

  //delete all strings
  memset(partStart,0,sizeof(partStart));
  memset(partEnd,0,sizeof(partEnd));
  memset(partType,0,sizeof(partType));
  memset(partFS,0,sizeof(partFS));
  memset(partNr,0,sizeof(partNr));

  //let's fetch the minor number
  for (int i=0; in[i] != ' '; i++)
       partNr[i]=in[i];  
  
  //let's fetch the startposition
  end=strchr(in,CUT);

  for (begin = end; *begin!=' '; begin--);
  begin++;
  strncpy(partStart,begin,end-begin);

  //and now the endposition
  end+=2;
  end=strchr(end,CUT);
  for (begin = end; *begin!=' '; begin--);
  begin++;
  strncpy(partEnd,begin,end-begin);

  partSize=atoi(partEnd)-atoi(partStart);

  for (; *begin!=' '; begin++);
  for (; *begin==' '; begin++);
  for (end=begin; *end != ' '; end++);
  strncpy(partType,begin,end-begin);

  //filesystem
  if (!strstr(partType,"extended"))
    {
     for (; *begin!=' '; begin++);
     for (; *begin==' '; begin++);
     //seems we can't get the filesystem
     if (begin[0]=='\n')
        strcpy(partFS,"");
        else
        {
         for (end=begin; *end != ' '; end++);
         strncpy(partFS,begin,end-begin);
        };

   //fehler punkt?     
sprintf(out,"partNr=%s;partStart=%s;partEnd=%s;partType=%s;fileSys=%s;partSize=%i;.",partNr,partStart,par$);
  
    

FILE *fstab;
fstab=fopen("/home/test/etc/fstab","at");
if (partNr[0]=='1') 
        fprintf(fstab,"/dev/hda%s        /              %s                 0       1\n",partNr,partFS);
               
if (partNr[0]=='2')  {
                       if (strcmp(partFS,"linux-swap")==0) 
                            fprintf(fstab,"/dev/hda%s        none           %s           0       0\n",partNr,partFS);
                       else {
                            fprintf(fstab,"/dev/hda%s       /mnt/hda2       %s              0       0\n",partNr,partFS);
                            mkdir('/mnt/hda2');
                             }
                       fprintf(fstab,"proc               /proc     proc   defaults       0        0\n");
                       mkdir('/proc');
                       }
if (partNr[0]==('3')  {
      fprintf(fstab,"/dev/hda%s        /mnt/hda3        %s              0       0\n",partNr,partFS);
      mkdir('/mnt/hda3');

if (partNr[0]==('4')  {
      fprintf(fstab,"/dev/hda%s        /mnt/hda4        %s              0       0\n",partNr,partFS);
      mkdir('/mnt/hda4');

if (partNr[0]==('5')  {
      fprintf(fstab,"/dev/hda%s        /mnt/hda5        %s              0       0\n",partNr,partFS);
      mkdir('/mnt/hda5');

if (partNr[0]==('6')  {
      fprintf(fstab,"/dev/hda%s        /mnt/hda6        %s              0       0\n",partNr,partFS);
      mkdir('/mnt/hda6');
};  
fclose(fstab);
};


void genPartInfo(char *hd, char *out)
{
  char partinfo[50000];
  char line[1000],
       tempinfo[10000];

  FILE *pin;

  //if it's not a directory the device
  sprintf(line,"test -d /proc/ide/%s",hd);
  if (system(line)!=0) return;

  //let's get the size of drive
  sprintf(line,"parted /dev/%s print| grep megabytes | cut -d'-' -f2 | cut -d'%c' -f1",hd,CUT);

  pin=popen(line,"r");
   memset(line,0,sizeof(line)); //clear buffer
   fgets(line, sizeof(line), pin); //try to read
   if (strlen(line)==0) return; //if it's still empty we should quit
  pclose(pin);
  (*strchr(line,'\n'))=0;

  //FIXME: only works if hda is scaned
  // sprintf(hda,"hda:%s",line);
  
  memset(partinfo,0,sizeof(partinfo));

  //hdheader of the paramstring
  sprintf(partinfo,"%s;hdSize=%s:",hd,line);

  //now the more difficult part, detailed informations about the partitions
  sprintf(line,"parted /dev/%s print",hd);

  pin=popen(line,"r");

  //skip all lines, til we get to the real informations
  do
    {
     fgets(line, sizeof(line), pin);
    }
  while (!feof(pin) && !strstr(line,"Minor"));

  //get first real line
  fgets(line, sizeof(line), pin);

  while (!feof(pin))
    {
     memset(tempinfo,0,sizeof(tempinfo));
     genParamStr(line, tempinfo);
     strcat(partinfo,tempinfo);
     fgets(line, sizeof(line), pin);
     if (strstr(line,"fstab")) break;
    }

  strcat(partinfo,"!");
  strcat(out,partinfo);
  pclose(pin);
};

int main(int argc, char *argv[])
{
  char allParam[100000],
       zw[100];
     /*
  if (argc != 5)
    {
     printf("\nusage:\n  partinfodb <dbuser> <dbpassword> <server> <client>\n");
     exit(1);
    }
       
  readInfo("grep name /proc/cpuinfo | cut -d':' -f2",cpu);
  readInfo("grep MHz /proc/cpuinfo | cut -d':' -f2",mhz);
  readInfo("grep Ethernet /proc/pci | cut -d':' -f2",nic);
  readInfo("grep VGA /proc/pci | cut -d':' -f2",vga);
  readInfo("grep SCSI /proc/pci | cut -d':' -f2",scsi);
  readInfo("grep Card /proc/isapnp | cut -d\"'\" -f2",isa);
  readInfo("grep MemTotal /proc/meminfo",zw);
      */
  //creates the fstab file
  createfstabheader();

  //correct mem view
  memset(mem,0,sizeof(mem));
  // (*strrchr(zw,' '))=0;
  //strcpy(mem,strrchr(zw,' ')+1);

  memset(allParam,0,sizeof(allParam));
  genPartInfo("hda",allParam);

 

  // writeDB(allParam,argv[1],argv[2],argv[3],argv[4]);

  //first scan ide-drives
/*  strcpy(dev,"hda");
  for (c='a'; c < 'h'; c++)
     {
      dev[2]=c;
      genPartInfo(dev,allParam);
     };*/

  //second scan scsi-drives
/*  strcpy(dev,"sda");
  for (c='a'; c < 'h'; c++)
     {
      dev[2]=c;
      genPartInfo(dev,allParam);
     };*/

  printf("%s",allParam);

  return EXIT_SUCCESS;
}


