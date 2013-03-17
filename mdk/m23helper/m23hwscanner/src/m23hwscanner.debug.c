/***************************************************************************
 *   Copyright (C) 2003-2007 by Hauke Goos-Habermann                       *
 *   HHabermann@pc-kiel.de                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parted/parted.h>
#include <parted/device.h>
#include <parted/disk.h>

char	keys[10000],
		values[100000],
		rootDevice[30],
		liloDevice[30];

FILE	*fstabFile,
		*liloFile;
		
int		cdromNr;

#define RINGBUFF_LEN 10
// #define FSTABFILE "/tmp/fstab"
// #define LILOFILE "/tmp/lilo.conf"

#define FSTABFILE "/etc/fstab"
#define LILOFILE "/etc/lilo.conf"

static char *ringbuff[RINGBUFF_LEN];
int ringbuff_counter = 0;


static char badchars[] =
        "\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020"
        "\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037\040"
        "\177\"'%$:|&";





/**
**name addAssoc(char *key, char *value)
**description C implementation of an associative array. glue is ###.
**parameter key: the key to store the information under
**parameter value: the value to store
**/
void addAssoc(char *key, char *value)
{
	strcat(keys,key);
	strcat(keys,"###");
	strcat(values,value);
	strcat(values,"###");
};





/**
**name getCDRoms(char *out, int scsi)
**description writes the device names of all CDRoms to out sperated by a blank.
**parameter out: variable to write the devices to
**parameter scsi: if set to "1" SCSI drives are added too
**/
void getCDRoms(char *out, int scsi)
{
	FILE	*pin;
	char	command[1000],
			line[1000],
			*elem;

	memset(command,0,sizeof(command));

	//set the command for finding all IDE CDRoms
	strcpy(command,"for i in `find /proc/ide/ -name media`\n\
do\n\
 media=`grep cdrom $i`\n\
if test -z $media\n\
 then\n\
 false\n\
 else\n\
 echo $i | cut -d'/' -f5\n\
 fi\n\
done");

	//add the SCSI finding commands, if needed
	if (scsi == 1)
		strcat(command,"\ndmesg | grep Attached | grep -v \"scsi disk\" |\
cut -d' ' -f4");

	pin=popen(command,"r");

	//add the device names one by one
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
			strcat(out," ");
		}
	while (!feof(pin));

	//strip last blank
	if (out[strlen(out)-1]!='\"')
		out[strlen(out)-1]=0;
	
	pclose(pin);
}





/**
**name void addCdrom(char *dev)
**description adds a cdrom drive to fstab file and creates a KDE icon file
**parameter dev: device name of the CDRom drive (e.g. sr3)
**/
void addCdrom(char *dev)
{
	FILE	*icon;
	char	dir[500],
			line[1000];

	if (fstabFile == NULL)
		return;

	//write to the fstab file
	fprintf(fstabFile,"/dev/%s /mnt/cdrom%i auto ro,noauto,user,exec 0 0\n",
			dev,cdromNr);

	//create the mount dir for the cdrom
	memset(dir,0,sizeof(dir));
	sprintf(dir,"/mnt/cdrom%i",cdromNr);
	mkdir(dir,555);

	sprintf(line,"chmod 555 %s",dir);
	system(line);

	memset(dir,0,sizeof(dir));
	
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
Dev=/dev/%s\n\
Encoding=UTF-8\n\
FSType=Default\n\
Icon=cdrom_mount\n\
MountPoint=/mnt/cdrom%i\n\
ReadOnly=true\n\
Type=FSDevice\n\
UnmountIcon=cdrom_unmount\n",dev,cdromNr);

	cdromNr++;

	fclose(icon);
};





/**
**name getIDE_SCSIEmulation(char *out)
**description gets all cdrom drives and generates an append line for the lilo.conf all cdrom are set to ide-scsi emulation
**parameter out: variable to write the append line to
**/
void getIDE_SCSIEmulation(char *out)
{
	char	cdRoms[10000],
			*cdRom;

	//get all CDRoms
	getCDRoms(cdRoms, 0);

	strcpy(out,"append=\"");

	//get the first CDRom
	cdRom = strtok(cdRoms," ");

	//if there is no CDRom drive: empty append line and exit
	if (cdRom == NULL)
		{
			strcpy(out,"");
			return;
		};

	//add the CDRoms
	do
		{
			strcat(out,cdRom);
			strcat(out,"=scsi-ide ");
			cdRom = strtok(NULL," ");
		}
	while (cdRom != NULL);

	//strip the last blank
	if (out[strlen(out)-1]!='\"')
		out[strlen(out)-1]=0;

	strcat(out,"\"");
};





/**
**name getIDE_SCSIEmulation(char *out)
**description creates the /etc/fstab and /etc/lilo.conf files or overwrites them
if the exists and creates a header in the files. If the creation failes return=0
else 1
**/
int InitLiloFstab()
{
	char	append[5000],
			cdRoms[10000],
			*cdRom;


	//create required directories
	mkdir("/usr/m23/",777);
	mkdir("/usr/m23/skel",777);
	mkdir("/usr/m23/skel/Desktop",777);

	//open file for writing fstab
	fstabFile=fopen(FSTABFILE,"wt");

	if (fstabFile==NULL)
		{
			fprintf(stderr,"could not write to: %s\n",FSTABFILE);
			exit(10);
		};

	//write fstab header + necessary proc entry
	fprintf(fstabFile,"%s\n%s\n",
			"#/etc/fstab was created by m23hwscanner",
			"#<file system> <mount point> <type> <options> <dump> <pass>");
	fprintf(fstabFile,"proc /proc proc defaults 0 0\n");
	mkdir("/proc",777);

	//get all CDRoms (IDE+SCSI)
	getCDRoms(cdRoms, 1);

	//get the first CDRom
	cdRom = strtok(cdRoms," ");

	//if there is no CDRom drive: empty append line and exit
	if (cdRom != NULL)
		//add the CDRoms
		do
			{
				addCdrom(cdRom);
				cdRom = strtok(NULL," ");
			}
		while (cdRom != NULL);




	//++++++++++++LILO

	//get lilo scsi-ide parameter append line
	memset(append,0,sizeof(append));
	getIDE_SCSIEmulation(append);

	liloFile=fopen(LILOFILE,"wt");
	if (liloFile==NULL)
		{
			fprintf(stderr,"could not write to: %s\n",LILOFILE);
			exit(20);
		};

	//write standard boot system
	fprintf(liloFile,"%s\n%s\nboot=%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
	"#/etc/lilo.conf was created by m23hwscanner",
	"lba32",
	liloDevice,
	"install=/boot/boot-menu.b",
	"map=/boot/map",
	"prompt",
	"delay=20",
	"timeout=150",
	append,
	"default=m23angelOne");

	return 1;
};





/**
**name void addHD(const char *path, const char *fs)
**description adds a harddisk to /etc/fstab and creates a KDE icon
**parameter path: path to the partition (e.g. /dev/hda2)
**parameter fs: filesystem of the partition (e.g. ext3)
**/
void addHD(const char *path, const char *fs, char wantedMp[5])
{
	char	mountPoint[1000],
			mp[1010],
			iconfolder[50];
			
	FILE	*icon;
	
	if (fstabFile == NULL)
		return;

	if (strlen(wantedMp)==0)
		sprintf(mountPoint,"/mnt/%s",strrchr(path,'/')+1);
	else
		strcpy(mountPoint,wantedMp);

	fprintf(fstabFile,"%s %s %s defaults 0 0\n",path,mountPoint,fs);

	//create folder for the HD icon
	memset(iconfolder,0,sizeof(iconfolder));	
	sprintf(iconfolder,"/usr/m23/skel/Desktop/%s.desktop",strrchr(path,'/')+1);
	
	//string of folder where icon will be stored
	icon=fopen(iconfolder,"wt");
	//create and open icon file
					
	fprintf(icon,"[Desktop Entry]\nDev=%s\nFSType=Default\nIcon=hdd_mount\n\
MountPoint=%s\nReadOnly=false\nType=FSDevice\nUnmountIcon=hdd_unmount",
path,mountPoint); 

	//write in icon file
	fclose(icon);

	mkdir(mountPoint,777);
};





/**
**name int addLinux(const char *dev, const char *fs, int rootDev)
**description adds a linux system to lilo.conf and fstab. It also tries to detect, if there is a Linux operating system installed on the partition and add it in this case as a alternative bootable system.
**paramater path: path to the Linux device to add
**parameter fs: filesystem of the partition (e.g. ext3)
**parameter rootDev: set to 1 if it is the partition, the m23 operating system is installed on
**/
int addLinux(const char *path, const char *fs, int rootDev)
{
	char		cmd[100],
				label[50],
				mountPoint[5],
				*deviceName;
	static int	lastRAID = 0;

	if (liloFile == NULL)
		return(0);
 
	//make mount dir
	mkdir("/tmp/mounttest",777);
	
	//mount dev
	sprintf(cmd,"mount -t %s %s /tmp/mounttest", fs, path);
	system(cmd);
	
	//get the name of the divice from a path (e.g. sdb1 from /dev/sdb1)
	deviceName=strrchr(path,'/');
	
	if (deviceName == NULL)
		return(1);
	else
		deviceName += sizeof(char);

	//if its our rootdevice select the special name
	if (rootDev)
		strcpy(label,"m23angelOne");
	else
		sprintf(label,"Linux(%s)",deviceName);
	
	memset(mountPoint,0,sizeof(mountPoint));

	//check if we have a bootable linux system 
	if (rootDev || (system("test -f /tmp/mounttest/vmlinuz || test -L /tmp/mounttest/vmlinuz")==0))
		{
			fprintf(liloFile,"\nimage=/vmlinuz\nlabel=%s\nread-only\nroot=%s\n",
					label,path) ;
			//check if there is an initial ramdisk
			if (system("test -f /tmp/mounttest/initrd.img || test -L /tmp/mounttest/initrd.img")==0)
				fprintf(liloFile,"%s\n",
				"initrd=/initrd.img");
		}

	//if it's the root device set the mountpoint to root
	if (rootDev)
		strcpy(mountPoint,"/");

	rmdir("/tmp/mounttest");

	system("umount /tmp/mounttest");

	addHD(path, fs, mountPoint);

	return(0);
};


void addRAID(const char *path, int vDev)
{
	char	*elem,
			*pos,
			devName[10],
			line[100],
			key[100],
			value[100];
	FILE	*pin;
	int		rPart = 0;

	//split the full device name to device name (e.g. /dev/md0 => md0)
	elem = strrchr(path, '/') + 1;
	strcpy(devName, elem);

	/*
		let's fetch the line with RAID information from /proc/mdstat
		this line may look like this:
			md0 : active raid0 hda1[0] sda2[1]
	*/
	sprintf(line,"grep %s /proc/mdstat",devName);
	pin = popen(line, "r");

	memset(line,0,sizeof(line));
	if (!feof(pin))
		fgets(line,sizeof(line),pin);
	else
		return;

	pclose(pin);
	
	//the parts of the line have to get split by a blank
	elem = strtok(line," ");
	
	while (elem != NULL)
	{
		//here stands the RAID level (e.g. raid0)
		if (strncmp(elem,"raid",4)==0)
			{
				//the RAID level number is located after the fourth character
				elem += 4;
				sprintf(key,"dev%ipart0_type",vDev);
				sprintf(value,"RAID-%s",elem);
				addAssoc(key, value);
			}
		else
			{
				//check if we have a "[" in the element
				pos = strchr(elem,'[');
				if (pos != NULL)
					{
						//yes, we have. this means that here stands a partition or drive that is used in the RAID (e.g. hda1[0])
						memset(devName,0,sizeof(devName));
						strncpy(devName,elem,pos - elem);
						sprintf(key,"dev%i_raidDrive%i",vDev,rPart);
						sprintf(value,"/dev/%s",devName);
						addAssoc(key, value);
						rPart++;
					};
			}

		elem = strtok(NULL," ");
	}
	
	sprintf(key,"dev%i_raidDriveAmount",vDev);
	sprintf(value,"%i",rPart);
	addAssoc(key, value);
}


/**
**name addWindows(const char *path, const char *fs, int bootable)
**description adds a Win32 system to lilo.conf and fstab.
**paramater path: path to the Win32 partition to add.
**parameter fs: filesystem of the partition (e.g. vfat).
**parameter bootable: set to 1 if it is a bootable partition.
**/
void addWindows(const char *path, const char *fs, int bootable)
{
	char *deviceName;

	if (liloFile == NULL)
		return;

	//get the name of the divice from a path (e.g. sdb1 from /dev/sdb1)
	deviceName=strrchr(path,'/');

	if (deviceName == NULL)
		return(1);
	else
		deviceName += sizeof(char);


	if (bootable)
		fprintf(liloFile,"\nother=%s\nlabel=Windows(%s)\ntable=%s\n",
				path,deviceName,path);
				
	addHD(path, fs, "");
};





/**
**name addPartition(const char *path, const char *fs, const char *type, int bootable)
**description adds a partition to lilo.conf and fstab and calls the real-adding functions.
**paramater path: path to the Win32 partition to add.
**parameter fs: filesystem of the partition (e.g. vfat).
**parameter type: type of the partition (e.g. extended, primary)
**parameter bootable: set to 1 if it is a bootable partition.
**/
void addPartition(const char *path, const char *fs, const char *type, int bootable)
{
	if (fstabFile == NULL)
		return;

	
	//extended partitions can't be added
	if (strcmp(type,"extended")==0)
		return;
	
	if ((strcmp(fs,"ext3")==0) ||
		(strcmp(fs,"ext2")==0) ||
		(strcmp(fs,"reiserfs")==0))
		addLinux(path,fs,strcmp(path,rootDevice)==0);

	if ((strcmp(fs,"fat32")==0) ||
		(strcmp(fs,"fat16")==0) ||
		(strcmp(fs,"vfat")==0) ||
		(strcmp(fs,"ntfs")==0))
		addWindows(path,fs,bootable);
		
	if (strcmp(fs,"linux-swap")==0)
	{
		fprintf(fstabFile,"%s none swap sw 0 0\n",path);
		printf("DEBUG: adding swap with path: %s\n",path);
	}
};





/**
**name url_encode(const char * in)
**description encodes a string with url encoding and returnes the encoded string
**parameter in: the string to encode
**/
const char *url_encode(const char * in)
{
        char *out;
        int i, j, k, len;

        for (i=len=0; in[i]; i++, len++)
                for (j=0; badchars[j]; j++)
                        if ( in[i] == badchars[j] ) { len+=2; break; }

        out = malloc(len + 1);

        for (i=k=0; in[i]; i++) {
                for (j=0; badchars[j]; j++)
                        if ( in[i] == badchars[j] ) break;
                if ( badchars[j] ) {
                        snprintf(out+k, 4, "%%%02X", in[i]);
                        k += 3;
                } else
                        out[k++] = in[i];
        }
        //assert(k==len);
        out[k] = 0;

        if ( ringbuff[ringbuff_counter] )
                free(ringbuff[ringbuff_counter]);
        ringbuff[ringbuff_counter++] = out;
        if ( ringbuff_counter == RINGBUFF_LEN ) ringbuff_counter=0;

        return out;
}





/**
**name PedDevice *nextDrive(PedDevice *last, char mds[20][5], int mdAmount, int *mdPos)
**description Gets the next SCSI/IDE/MD device.
**parameter last: PedDevice that was used last or NULL if it is the first call.
**parameter mds: 
**returns Pointer to PedDevice that stores the next 
**/
PedDevice *nextDrive(PedDevice *last)
{
	char 		devName[10],
				*elem;
	PedDevice	*next;
	static char	MDevicesArr[20][5];
	static int	firstCall = 1,
				mdAmount = 0, //amount of found multi devices
				mdPos = 0; //position in the mulri device array
	FILE 		*pin;

	if (firstCall)
	{
		//get all multi devices and store them in an array
		pin = popen("cat /proc/mdstat | grep md[0-9] | cut -d' ' -f1", "r");

		memset(MDevicesArr,0,sizeof(MDevicesArr));
		while (!feof(pin))
			{
				fgets(MDevicesArr[mdAmount],5,pin);

				elem = strchr(MDevicesArr[mdAmount],'\n');
				if (elem != NULL)
					(*elem)='\0';
				else
					break;

				if (strlen(MDevicesArr[mdAmount]) > 0)
					mdAmount++;
			};
	
		pclose(pin);
		firstCall = 0;
	};

	next = ped_device_get_next(last);

	if (next)
		return(next);
	else
		if (mdAmount != mdPos)
			{
				sprintf(devName,"/dev/%s",MDevicesArr[mdPos]);
				mdPos++;
				return(ped_device_get(devName));
			}
		else
			return(NULL);
}





/**
**name scanDisksPartitions()
**description scans all disks and partitions and saves the gathered information in an associative array
**/
void scanDisksPartitions()
{
	PedDevice	*currentDev = NULL;

	char		*deviceDev,
				key[100],
				value[100],
				line[1000];

	int 		devNr = 0,
				partNr,
				first_flag,
				bootable;

	PedDisk*	disk;
	PedPartition*	part;
	PedPartitionFlag flag;

	double		dTemp;

	//get all device information
	ped_device_probe_all();

	//set pointer to first drive
	currentDev = nextDrive(NULL);
	
	while (currentDev)
		{
			//save path of the device (e.g. /dev/hda)
			sprintf(key,"dev%i_path",devNr);
			addAssoc(key, currentDev->path);

			//label the partition
			sprintf(line,"checkdisklabel %s\n",currentDev->path);
			system(line);
  
			//save size in MB
			sprintf(key,"dev%i_size",devNr);
			
			dTemp = (double)currentDev->length / 1048576;
			dTemp *= currentDev->sector_size;
			
			sprintf(value,"%i", (long)dTemp);

			addAssoc(key, value);

			//save model name of the device (e.g. WD 22323)
			sprintf(key,"dev%i_model",devNr);
			addAssoc(key, currentDev->model);
			
			disk = ped_disk_new (currentDev);
			if (!disk) 
				{
					currentDev = nextDrive(currentDev);
					devNr++;
					continue;
				}

			partNr = 0;
			
			for (part = ped_disk_next_partition (disk, NULL); part;
					part = ped_disk_next_partition (disk, part)) 
				{
					if (!ped_partition_is_active (part))
						continue;

					//partition number (e.g. 1 for /dev/hda1)
					sprintf(key,"dev%ipart%i_nr",devNr,partNr);
					sprintf(value,"%i",part->num);
					addAssoc(key, value);

					//partition start
					sprintf(key,"dev%ipart%i_start",devNr,partNr);
					dTemp = (double)part->geom.start / 1048576;
					dTemp *= (double)currentDev->sector_size;
					sprintf(value,"%i",(long)dTemp);
					addAssoc(key, value);

					//partition end
					sprintf(key,"dev%ipart%i_end",devNr,partNr);
					dTemp = (double)part->geom.end / 1048576;
					dTemp *= (double)currentDev->sector_size;
					sprintf(value,"%i",(long)dTemp);
					addAssoc(key, value);

					//partition file system
					sprintf(key,"dev%ipart%i_fs",devNr,partNr);
					sprintf(value,"%s",part->fs_type ? part->fs_type->name : "");
					addAssoc(key, value);

					//partition type
					if (strstr(currentDev->path,"md") == NULL)
						{
							//only add a type here if it's not a MD (MDs get RAID-? as type)
							sprintf(key,"dev%ipart%i_type",devNr,partNr);
							addAssoc(key, ped_partition_type_get_name (part->type));
						};
					
					//figure out, if the partition is bootable
					bootable = 0;
					first_flag = 1;
					for (flag = ped_partition_flag_next (0); flag;
						flag = ped_partition_flag_next (flag)) 
						{
							if (ped_partition_get_flag (part, flag))
								{
									if (first_flag)
										first_flag = 0;

									if (strstr(ped_partition_flag_get_name(flag),"boot") != NULL)
										{
											bootable = 1;
											break;
										};
								}
						}

					
					
					if (strstr(currentDev->path,"md") == NULL)
						sprintf(value,"%s%i",currentDev->path,part->num);
					else
						sprintf(value,"%s",currentDev->path);

					printf("DEBUG: Path:%s Type:%s Bootable:%i\n",part->fs_type ? part->fs_type->name : "", ped_partition_type_get_name (part->type), bootable);
					fflush(stdout);


					addPartition(value,part->fs_type ? part->fs_type->name : "",
									ped_partition_type_get_name (part->type), bootable);
									

					partNr++;
				}

			//save amount of partitions
			sprintf(key,"dev%i_partamount",devNr);
			sprintf(value,"%i",partNr);
			addAssoc(key, value);

			//add RAID information
			if (strstr(currentDev->path,"md") != NULL)
				addRAID(currentDev->path,devNr);

			ped_disk_destroy (disk);

			devNr++;

			currentDev = nextDrive(currentDev);

			if (currentDev==NULL)
				break;
		};
}





/**
**name readInfo(char *query, char *out)
**description reads info from comand-line-arguments
**parameter query: the command to execute
**parameter out: the variable, the output of the command should be stored in
**/
void readInfo(char *query, char *out)
{
	memset(out,0,999);

	FILE *pin;

	pin=popen(query,"r");
	
	if (pin)
		{
			fgets(out, 999, pin);
			fclose(pin);
		};
}





/**
**name getAllLines(const char * command, const char *key)
**description gets all returned lines from the command and stores the result in
the associative array under a key.
**/
void getAllLines(const char * command, const char *key)
{
	char	line[1000],
			dmi[65636];
	FILE *pin;

	unsigned int i;

	pin=popen(command,"r");

	memset(dmi,0,sizeof(dmi));	

		while (!feof(pin))
		{
			memset(line,0,sizeof(line));
			fgets(line, sizeof(line), pin);
			strcat(dmi,line);
		}
 
	addAssoc(key, dmi);

	pclose(pin);
};





/**
**name getHardwareData()
**description gathers different hardware informations and store them to the
associative array
**/
void getHardwareData()
{
	char	zw[1000],
			zw2[1000];

	getAllLines("grep name /proc/cpuinfo | cut -d':' -f2", "cpu");
	getAllLines("grep MHz /proc/cpuinfo | cut -d':' -f2 | cut -d' ' -f2", "mhz");

	getAllLines("lspci | grep Ethernet","net");
	
	getAllLines("lspci | grep -i audio","sound");

	getAllLines("lspci | grep -i vga; lspci | grep -i display","vga");

	readInfo("grep Card /proc/isapnp 2> /dev/null | cut -d\"'\" -f2",zw);
	addAssoc("isa", zw);
	
	readInfo("grep MemTotal /proc/meminfo",zw2);
	//correct mem view
	memset(zw,0,sizeof(zw));
	(*strrchr(zw2,' '))=0;
	strcpy(zw,strrchr(zw2,' ')+1);
	addAssoc("mem", zw);
  
	getAllLines("dmidecode","dmi");
	getAllLines("lsmod","modules");
};


void fixLiloFstab(const char *rootDevice)
{
	char	cmd[350];

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"\
#if the m23hwscanner doesn't detect the root device => add the entries to lilo.conf anf fstab via the script\n\
if test `grep -c m23angelOne /etc/lilo.conf` -lt 2\n\
then\n\
cat >> /etc/lilo.conf << \"LILOEOF\"\n\
image=/vmlinuz\n\
label=m23angelOne\n\
read-only\n\
root=%s\n\
initrd=/initrd.img\n\
LILOEOF\n\
\n\
echo \"%s / auto defaults 0 0\" >> /etc/fstab\n\
fi",rootDevice,rootDevice);
	system(cmd);
}



int main(int argc, char *argv[])
{
	FILE *fHWData;

	if (argc==3)
		{
			cdromNr = 1;

			strcpy(liloDevice,argv[1]);
			strcpy(rootDevice,argv[2]);

			InitLiloFstab();
		}
	else if (argc!=1)
		{
			printf("%s can be used in 2 different ways\n\
1. gather hardware and partition information in the m23 hardware and \
partition format ans store the result in /tmp/partHwData.post:\n\
\tuse %s with NO arguments\n\
2. generate /etc/fstab and /etc/lilo.conf by:\n\
\t%s [bootDevice] [rootDevice]\n\
\te.g.: bootDevice=/dev/hda and rootDevice=/dev/hda1\n",argv[0],argv[0],argv[0]);
			exit(-1);
		}
	else
		{
			liloFile = NULL;
			fstabFile = NULL;
		};

	//clear fields
	memset(keys,0,10000);
	memset(values,0,100000);
	
	addAssoc("ver", "2");

	scanDisksPartitions();
	getHardwareData();

	//remove the last 3 #'es
	(*strrchr(values,'#'))=0;
	(*strrchr(values,'#'))=0;
	(*strrchr(values,'#'))=0;
	
	if (argc==3)
		{
			fclose(fstabFile);
			fclose(liloFile);
			fixLiloFstab(rootDevice);
		}
	else
		{
			fHWData=fopen("/tmp/partHwData.post","wt");
			fprintf(fHWData,"type=partHwData&data=%s%s",keys,url_encode(values));
			fclose(fHWData);
		}

  return EXIT_SUCCESS;
}
