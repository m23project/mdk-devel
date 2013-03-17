/***************************************************************************
	main.cpp	-	description
	---------------------------
		begin							: Fre Feb	7 16:55:02 CET 2003
		copyright						: (C) 2003 by Hauke Habermann
		email							: HHabermann@pc-kiel.de
 ***************************************************************************/

/****************************************************************************
*																		 	*
*	 This program is free software; you can redistribute it and/or modify	*
*	 it under the terms of the GNU General Public License as published by	*
*	 the Free Software Foundation; either version 2 of the License, or		*
*	 (at your option) any later version.									*
*																			*
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FSTABFILE "/tmp/fstab"
#define LILOFILE "/tmp/lilo.conf"

//#define FSTABFILE "/etc/fstab"
//#define LILOFILE "/etc/lilo.conf"


char CUT,
		 rootDevice[10],
		 liloDevice[10];
				
int memi,
		cdromNr=0;

#define HD 1
#define CDROM 2
	
//checks if a fat/fat32/ntfs partition is bootable
bool checkBootable(char *dev)
{
	char cmd[500],
		 minor,
		 disk[5];
	int len;
	FILE *pin;

	memset(disk,0,sizeof(disk));
	memset(cmd,0,sizeof(cmd));

	len=strlen(dev);
	
	strncpy(disk, dev, len-1);
	minor=dev[len-1];
	
	sprintf(cmd,"parted -s /dev/%s print %c | grep Flags",disk,minor);
	puts(cmd);

	pin=popen(cmd,"r");
	fgets(cmd, sizeof(cmd), pin);
	pclose(pin);

	return (strstr(cmd,"boot")!=0);
}





//gets all cdrom drives and generates an append line for the lilo.conf
//all cdrom are set to ide-scsi emulation
void getCdroms(char *out)
{
	FILE *pin;
	char line[500],
			 *elem;

	pin=popen("for i in `find /proc/ide/ -name media`\n\
do\n\
 media=`grep cdrom $i`\n\
if test -z $media\n\
 then\n\
 false\n\
 else\n\
 echo $i | cut -d'/' -f5\n\
 fi\n\
done","r");

strcpy(out,"append=\"");

	do
		{
		 memset(line,0,sizeof(line));
		 fgets(line, sizeof(line), pin);
		 
		 elem=strchr(line,'\n');
		 if (elem != NULL)
				(*elem)='\0';
				else
				break;
				
		 strcat(out,line);
		 strcat(out,"=scsi-ide ");

		} while (!feof(pin));
		
	//strip last blank
	if (out[strlen(out)-1]!='\"')
			out[strlen(out)-1]=0;
					
	strcat(out,"\"");
	pclose(pin);
};





//returnes the type of a selected IDE device
int getMediaType(char *dev)
{
 FILE *pin;
 char line[100],
			*elem;
 int	type=0;		 

 if (dev[0]=='h') 
		 sprintf(line,"cat /proc/ide/%s/media",dev);

 if (dev[0]=='s')
		 sprintf(line,"dmesg | grep Attached | grep %s | cut -d' ' -f3",dev);
		 
 
 pin=popen(line,"r");

 memset(line,0,sizeof(line));
 
 fgets(line,sizeof(line),pin);

 elem=strchr(line,'\n');
 if (elem != NULL)
		(*elem)='\0';

 if (strcmp(line,"disk")==0)
		type=HD;
		else
		if ((strcmp(line,"cdrom")==0) || (strcmp(line,"CD-ROM")==0))
			type=CDROM; 

 fclose(pin);
 
 return(type);
};





//adds a windows system to the lilo.conf
int addWindows(char *dev, char *hd)
{
 FILE *lilo;

 //if the partition isn't bootable, there shouldn't be an entry
 if (checkBootable(dev)==false)
		return(0);
 
 lilo=fopen(LILOFILE,"at");
 if (lilo==NULL) return -1;

 lilo=fopen(LILOFILE,"at");

 fprintf(lilo,"\nother=/dev/%s\nlabel=Windows(%s)\ntable=/dev/%s\n",dev,dev,hd);

 fclose(lilo);
 
 return(1);
};





//adds a linux system to the lilo.conf
int addLinux(char *dev, char *fs, bool rootDev)
{
	char cmd[100],
		 label[50];

	FILE *lilo;
	lilo=fopen(LILOFILE,"at");
	if (lilo==NULL) return -1;
 
	//make mount dir
	mkdir("/tmp/mounttest",777);
 
	//mount dev
	sprintf(cmd,"mount -t %s /dev/%s /tmp/mounttest", fs, dev);
	system(cmd);

	lilo=fopen(LILOFILE,"at");

	//if its our rootdevice select the special name
	if (rootDev)
		 strcpy(label,"m23angelOne");
		 else
		 sprintf(label,"Linux(%s)",dev);	

		 
	//check if we have a bootable linux system 
	if (system("test -f /tmp/mounttest/vmlinuz || test\
-L /tmp/mounttest/vmlinuz")==0)
		{
			fprintf(lilo,"\nimage=/vmlinuz\nlabel=%s\nread-only\nroot=/dev/%s\n",
					label,dev) ;
			//check if there is a initial ramdisk		 
			if (system("test -f /tmp/mounttest/initrd.img")==0)
				fprintf(lilo,"%s\n",
				"initrd=/initrd.img");
		}

	rmdir("/tmp/mounttest");
		
	system("umount /tmp/mounttest");

	fclose(lilo);

	return(0);
};





/*creates the	/etc/fstab file or overwrites it if it exists and creates a
header in the file
creation failed return=0 else 1*/
int createfstabheader()
{
	FILE *fstab;
	fstab=fopen(FSTABFILE,"wt");
	if (fstab==NULL)
		{
		 fprintf(stderr,"could not write to: %s\n",FSTABFILE);
		 exit(10);
		}; 

	fprintf(fstab,"%s\n%s\n",
			"# /etc/fstab file system info, created by the m23 project",
			"#<file system> <mount point> <type> <options> <dump> <pass>");
	fprintf(fstab,"proc /proc proc defaults 0 0\n");
	mkdir("/proc",777);
	fclose(fstab);
	return 1;
}





//creates the header for the lilo.conf file
int createliloheader()
{
	//hdc=ide-scsi
	FILE *lilo;
	char append[5000];

	memset(append,0,sizeof(append));
	getCdroms(append);

	lilo=fopen(LILOFILE,"wt");
	if (lilo==NULL)
		{
			fprintf(stderr,"could not write to: %s\n",LILOFILE);
			exit(20);
		};
		
	fprintf(lilo,"%s\n%s\nboot=/dev/%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
	"#/etc/lilo.conf file for bootloader, created by the m23 project",
	"lba32",
	liloDevice,
	"install=/boot/boot-menu.b",
	"map=/boot/map",
	"prompt",
	"delay=20",
	"timeout=150",
		append,
	"default=m23angelOne");

	fclose(lilo);
	return 1;
}





//adds a cdrom drive to fstab file
void addCdrom(char *dev)
{
	FILE *fstab,
		 *icon;
	char dir[500],
			 line[1000];

	fstab=fopen(FSTABFILE,"at");	

	//fstab file can't be opened, so quit procedure
	if (fstab == NULL)
			return;	

	//write to the fstab file
	fprintf(fstab,"/dev/%s /mnt/cdrom%i auto ro,noauto,user,exec 0 0\n",
			dev,cdromNr);

	fclose(fstab);

	//create the mount dir for the cdrom
	memset(dir,0,sizeof(dir));	//delete string
	sprintf(dir,"/mnt/cdrom%i",cdromNr);
	mkdir(dir,555);

	sprintf(line,"chmod 555 %s",dir);
	system(line);

	memset(dir,0,sizeof(dir));	//delete string
	//string of folder where icon will be stored
	sprintf(dir,"/usr/m23/skel/Desktop/DVD-CD%i.desktop",cdromNr);
	//create and open icon file
	icon=fopen(dir,"wt");

	//icon file can't be opened, so quit procedure
	if (icon == NULL)
			return;

	fprintf(icon,"[Desktop Action Eject]\n\
Exec=kdeeject \%v\n\
Name=Eject\n\
Name[de]=Auswerfen\n\
\n\
[Desktop Entry]\n\
Actions=Eject\n\
Dev=/dev/scd%i\n\
Encoding=UTF-8\n\
FSType=Default\n\
Icon=cdrom_mount\n\
MountPoint=/mnt/cdrom%i\n\
ReadOnly=true\n\
Type=FSDevice\n\
UnmountIcon=cdrom_unmount\n",cdromNr,cdromNr);

	cdromNr++;
	
	fclose(icon);
};




void genParamStr(char *in, char *hd )
{
	char *begin,
		 *end,
		 partStart[50],
		 partEnd[50],
		 partFS[50],
		 partType[50],
		 partNr[4],
		 dev[10];

	int partSize;
	char mountpoint[50];
	char iconfolder[50];	

	//delete all strings
	memset(partStart,0,sizeof(partStart));
	memset(partEnd,0,sizeof(partEnd));
	memset(partType,0,sizeof(partType));
	memset(partFS,0,sizeof(partFS));
	memset(partNr,0,sizeof(partNr));
	memset(mountpoint,0,sizeof(mountpoint));

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
			};

	memset(dev,0,sizeof(dev));
	//build device
	sprintf(dev,"%s%s",hd,partNr);

	//fat32 has to be mounted as vfat
	if (strstr(partFS,"fat32")!=0)
		strcpy(partFS,"vfat");
	
	if (strlen(partFS)==0)
		return;

	//write entries for fstab
	FILE *icon;
	FILE *fstab;
	fstab=fopen(FSTABFILE,"at");

	if (strcmp(partFS,"linux-swap")==0)	 
			fprintf(fstab,"/dev/%s%s none swap sw 0 0\n",hd,partNr);
			else
			{
				if (strcmp(dev,rootDevice)==0)
					{
						fprintf(fstab,"/dev/%s%s / %s defaults 0 1\n",hd,partNr,partFS);

						memset(iconfolder,0,sizeof(iconfolder));	//delete string
						sprintf(iconfolder,"/usr/m23/skel/Desktop/%s%s.desktop",hd,partNr);
						
						//string of folder where icon will be stored
						icon=fopen(iconfolder,"wt");
						
						//create and open icon file
						fprintf(icon,"[Desktop\
Entry]\nDev=/dev/%s%s\nFSType=Default\nIcon=hdd_mount\nMountPoint=/mnt/%s%s\
nReadOnly=false\nType=FSDevice\nUnmountIcon=hdd_unmount",hd,partNr,hd,partNr);
						//write in icon file
						fclose(icon);
					}
				else
					{
						fprintf(fstab,"/dev/%s%s /mnt/%s%s %s defaults 0 0\n"
								,hd,partNr,hd,partNr,partFS);
	
						memset(iconfolder,0,sizeof(iconfolder));	//delete string
						sprintf(mountpoint,"/mnt/%s%s",hd,partNr);
						sprintf(iconfolder,"/usr/m23/skel/Desktop/%s%s.desktop",hd,partNr);
	//string of folder where icon will be stored
						icon=fopen(iconfolder,"wt");
						//create and open icon file
					
fprintf(icon,"[DesktopEntry]\nDev=/dev/%s%s\nFSType=Default\nIcon=hdd_mount\n\
MountPoint=/mnt/%s%s\nReadOnly=false\nType=FSDevice\nUnmountIcon=hdd_unmount",hd
,partNr,hd,partNr); 
						//write in icon file
						fclose(icon);
						
						mkdir(mountpoint,777);
						}
			}
												
	
	fclose(fstab);
	
	
	
	//seems we found a linux partition
	if ((strcmp(partFS,"ext3")==0) ||
			(strcmp(partFS,"ext2")==0) ||
			(strcmp(partFS,"reiserfs")==0))
			addLinux(dev,partFS,strcmp(dev,rootDevice)==0);
			
	if ((strcmp(partFS,"vfat")==0) ||
			(strcmp(partFS,"ntfs")==0))
			addWindows(dev,hd);		
			
};





//get the cut symbol
void getCut(char *dev)
{
	char line[1000],
			 cut;
	FILE *pin;		 
	
	sprintf(line,"parted -s /dev/%s print| grep megabytes",dev);

	pin=popen(line,"r");

	memset(line,0,sizeof(line)); //clear buffer

	fgets(line, sizeof(line), pin); //try to read

	if (strlen(line)==0) return; //if it's still empty we should quit

	if (strchr(line, ',') != NULL)
			CUT=',';
			else
			CUT='.';
			
	pclose(pin);
};
			 

void genPartInfo(char *hd)
{
	char line[1000];

	FILE *pin;

	//if it's not a directory the device
	sprintf(line,"test -d /proc/ide/%s",hd);
	if ((system(line)!=0) && (hd[0]!='s')) return;
	
	//get the cut symbol and save it to CUT
	getCut(hd);

	//let's get the size of drive
	sprintf(line,"parted -s /dev/%s print| grep megabytes | cut -d'-' -f2 | cut\
-d'%c' -f1",hd,CUT);

	pin=popen(line,"r");
	 memset(line,0,sizeof(line)); //clear buffer
	 fgets(line, sizeof(line), pin); //try to read
	 if (strlen(line)==0) return; //if it's still empty we should quit
	pclose(pin);
	(*strchr(line,'\n'))=0;

	//now the more difficult part, detailed informations about the partitions
	sprintf(line,"parted -s /dev/%s print",hd);

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
		 genParamStr(line,hd);
		 fgets(line, sizeof(line), pin);
		 if (strstr(line,"fstab")) break;
		}

	pclose(pin);
};



int main(int argc, char *argv[])
{
	FILE *pin;
	char line[1000],
		 *elem;
	
	cdromNr=1;
 
	if (argc!=3)
		{
			printf("\ngenFstab <install lilo in (e.g. /dev/hda)> <rootDevice (e.g.\
/dev/hda1)\n");
			return(1);
		}

	strcpy(liloDevice,"/dev/hda");
	strcpy(rootDevice,"/dev/hda2");

	//strcpy(liloDevice,argv[1]);
	//strcpy(rootDevice,argv[2]);

	//create required directories
	mkdir("/usr/m23/",777);
	mkdir("/usr/m23/skel",777);
	mkdir("/usr/m23/skel/Desktop",777);
		
	//creates the fstab file
	createfstabheader();

	//creates the lilo.conf
	createliloheader();

	//get all IDE drives
	pin=popen("ls /proc/ide/ | grep hd","r");
	do
		{
		 fgets(line, sizeof(line), pin);
		 //search for the newline
		 elem=strchr(line,'\n');
		 //exchange the newline with
		 if (elem != NULL)
				(*elem)='\0';
				else
		 //if we can't find the newline, we have seen all hd? drives	 
				break;
				
		 switch (getMediaType(line))
			{
				case HD:
						{
							//adds a partition to fstab, desktop file and eventually to lilo.conf
							genPartInfo(line);
							break;
						};
				case CDROM:
						{
							//adds a cdrom to fstab and desktop file
							addCdrom(line);
							break;
						};
			}		 
		}
	while (!feof(pin));
	
	if (pin)
	pclose(pin);


	//get all SCSI drives
	pin=popen("dmesg | grep Attached | cut -d' ' -f4","r");
	do
		{
		 fgets(line, sizeof(line), pin);
		 //search for the newline
		 elem=strchr(line,'\n');
		 //exchange the newline with
		 if (elem != NULL)
				(*elem)='\0';
				else
		 //if we can't find the newline, we have seen all hd? drives
				break;

		 switch (getMediaType(line))
			{
				case HD:
						{
							//adds a partition to fstab, desktop file and eventually to lilo.conf
							
							genPartInfo(line);
							break;
						};
				case CDROM:
						{
							//adds a cdrom to fstab and desktop file
							addCdrom(line);
							break;
						};
			}
		}
	while (!feof(pin));

	if (pin)
	pclose(pin);	


	return EXIT_SUCCESS;
}
