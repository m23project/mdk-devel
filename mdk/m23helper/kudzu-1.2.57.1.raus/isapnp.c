/* Copyright 1999-2004 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "isapnp.h"

#include "kudzuint.h"

/* I am in hell. ISAPnP is the fifth ring of hell. */

static struct isapnpDevice *isapnpDeviceList = NULL;
static int numIsapnpDevices = 0;

static char *netlist[] = {
	  "3c509",
	  "3c515",
	  "ne",
	  "sb1000",
	  "smc-ultra",
	  NULL
};
	  
static char *scsilist[] = {
	  "aha1542",
	  "g_NCR5380",
	  NULL
};

static void isapnpFreeDevice(struct isapnpDevice *dev) {
	if (dev->deviceId) free(dev->deviceId);
	if (dev->pdeviceId) free(dev->pdeviceId);
	if (dev->compat) free(dev->compat);
	freeDevice((struct device *)dev);
}

static void isapnpWriteDevice(FILE *file, struct isapnpDevice *dev) {
	writeDevice(file, (struct device *)dev);
	if (dev->deviceId)
	  fprintf(file,"deviceId: %s\n",dev->deviceId);
	if (dev->pdeviceId)
	  fprintf(file,"pdeviceId: %s\n", dev->pdeviceId);
	if (dev->compat)
	  fprintf(file,"compat: %s\n",dev->compat);
}

static int devCmp(const void * a, const void * b) {
	const struct isapnpDevice * one = a;
	const struct isapnpDevice * two = b;
	int x =0 , y = 0;
	
	x = strcmp(one->deviceId,two->deviceId);
	if (one->pdeviceId && two->pdeviceId) 
	  y = strcmp(one->pdeviceId,two->pdeviceId);
	else
	  y = one->pdeviceId - two->pdeviceId;
	if (x)
	  return x;
	else if (y)
	  return y;
	return 0;
}

static int isapnpCompareDevice(struct isapnpDevice *dev1, struct isapnpDevice *dev2)
{
	int x=compareDevice((struct device *)dev1,(struct device *)dev2);
	if (x) return x;
	return devCmp( (void *)dev1, (void *)dev2 );
	/* needs finished */
}

static char *demangle(int vendor, int device) {
	static char ret[8];
	
        sprintf(ret, "%c%c%c%x%x%x%x",
		'A' + ((vendor >> 2) & 0x3f) - 1,
		'A' + (((vendor & 3) << 3) | ((vendor >> 13) & 7)) - 1,
		'A' + ((vendor >> 8) & 0x1f) - 1,
		(device >> 4) & 0x0f,
		device & 0x0f,
		(device >> 12) & 0x0f,
		(device >> 8) & 0x0f);
	return ret;
}

int isapnpReadDrivers(char *filename) {
	int fd;
	char path[256];
	int id1, id2, id3, id4;
	struct isapnpDevice key, *nextDevice;
	char *ident, *pident;
	char *buf, *start, *next, *ptr, *module;
	
	snprintf(path,255,"/lib/modules/%s/modules.isapnpmap",kernel_ver);
	fd = open(path,O_RDONLY);
	if (fd < 0) {
		fd = open("/etc/modules.isapnpmap", O_RDONLY);
		if (fd < 0) {
			fd = open("/modules/modules.isapnpmap", O_RDONLY);
			if (fd < 0) {
				fd = open("./modules.isapnpmap", O_RDONLY);
				if (fd <0) 
				  return -1;
			}
		}
	}
	
	start = buf = __bufFromFd(fd);
	
	nextDevice = isapnpDeviceList + numIsapnpDevices;
	
	while (*buf) {
		next = buf;
		while (*next && *next != '\n') next++;
		if (*next) {
			*next = '\0';
			next++;
		}
		ptr = buf;
		if (*ptr == '#') {
			buf = next;
			continue;
		}
		
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		module = strdup(buf);
		buf = ptr;

		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		id1 = strtoul(buf, NULL, 16);
		buf = ptr;
		
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		id2 = strtoul(buf, NULL, 16);
		buf = ptr;
		
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		buf = ptr;
		
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		id3 = strtoul(buf,NULL,16);
		buf = ptr;
		
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*ptr)) ptr++;
		id4 = strtoul(buf,NULL,16);
		
		pident = strdup(demangle(id1, id2));
		ident = strdup(demangle(id3, id4));
		key.deviceId = ident;
		key.pdeviceId = pident;
		
		nextDevice = bsearch(&key, isapnpDeviceList, numIsapnpDevices,
				     sizeof(struct isapnpDevice), devCmp);
		
		if (!nextDevice) {
			isapnpDeviceList = realloc(isapnpDeviceList,
						   (numIsapnpDevices + 1) *
						   sizeof(struct isapnpDevice));
			nextDevice = isapnpDeviceList + numIsapnpDevices;
			memset(nextDevice,'\0',sizeof(struct isapnpDevice));
			nextDevice->driver = module;
			nextDevice->deviceId = ident;
			nextDevice->pdeviceId = pident;
			nextDevice++;
			numIsapnpDevices++;
			qsort(isapnpDeviceList, numIsapnpDevices,
			      sizeof(*isapnpDeviceList), devCmp);
		} else {
			free(ident);
			free(pident);
			free(module);
		}
		buf = next;
	}
	free(start);
	return 0;
}

void isapnpFreeDrivers(void) {
        int x;

        if (isapnpDeviceList) {
		for (x=0;x<numIsapnpDevices;x++) {
			if (isapnpDeviceList[x].deviceId) free (isapnpDeviceList[x].deviceId);
			if (isapnpDeviceList[x].driver) free (isapnpDeviceList[x].driver);
	        }
		free(isapnpDeviceList);
		isapnpDeviceList=NULL;
		numIsapnpDevices=0;
	}
}

struct isapnpDevice * isapnpNewDevice(struct isapnpDevice *dev) {
	struct isapnpDevice *ret;
    
	ret = malloc(sizeof(struct isapnpDevice));
	memset(ret,'\0',sizeof(struct isapnpDevice));
	ret=(struct isapnpDevice *)newDevice((struct device *)dev,(struct device *)ret);
	ret->bus = BUS_ISAPNP;
	if (dev && dev->bus == BUS_ISAPNP) {
		if (dev->deviceId) ret->deviceId=strdup(dev->deviceId);
		if (dev->pdeviceId) ret->pdeviceId=strdup(dev->pdeviceId);
		if (dev->compat) ret->compat=strdup(dev->compat);
	}
	ret->newDevice = isapnpNewDevice;
	ret->freeDevice = isapnpFreeDevice;
	ret->writeDevice = isapnpWriteDevice;
	ret->compareDevice = isapnpCompareDevice;
	return ret;
}

static void setDriverAndClass(struct isapnpDevice *dev) {
	struct isapnpDevice key, *searchdev;
	int x;
	
	dev->type = CLASS_OTHER;
	key.deviceId = dev->deviceId;
	key.pdeviceId = dev->pdeviceId;
	searchdev = bsearch(&key, isapnpDeviceList, numIsapnpDevices,
			    sizeof(struct isapnpDevice), devCmp);
	if (!searchdev) {
		key.pdeviceId = demangle(0xffff,0xffff);
		searchdev = bsearch(&key, isapnpDeviceList, numIsapnpDevices,
			    sizeof(struct isapnpDevice), devCmp);
	}
	if (!searchdev && dev->compat) {
		char *tmp, *d;

		d = alloca(strlen(dev->compat)+1);
		strcpy(d,dev->compat);
		
		while (1) {
			tmp = strstr(d,",");
			if (!tmp) break;
			*tmp = '\0';
			tmp++;
			
			key.deviceId = d;
			searchdev = bsearch(&key, isapnpDeviceList, numIsapnpDevices,
					    sizeof(struct isapnpDevice), devCmp);
			if (searchdev) break;
			
			d = tmp;
		}
	}
	if (searchdev) {
		dev->driver = strdup(searchdev->driver);
	}
	if (dev->driver && !strncmp(dev->driver,"snd-",4))
		dev->type = CLASS_AUDIO;
	for (x=0; netlist[x]; x++) {
		if (dev->driver && !strcmp(netlist[x],dev->driver)) {
			dev->type = CLASS_NETWORK;
			dev->device = strdup("eth");
		}
	}
	for (x=0; scsilist[x]; x++) {
		if (dev->driver && !strcmp(scsilist[x],dev->driver))
		  dev->type = CLASS_SCSI;
	}
}

static struct device *isapnpAddDevice(int idfd, const char *pdevice, const char *pname,
	enum deviceClass probeClass, struct device *devlist)
{
	struct isapnpDevice *dev = NULL;
	char *tmp = NULL;
	char *devid;

	devid = __bufFromFd(idfd);
	devid[strlen(devid) - 1] = '\0';
	if ((tmp = strchr(devid, '\n'))) {
		*tmp = '\0';
		tmp++;
	}
	dev = isapnpNewDevice(NULL);
	if (pdevice)
		dev->pdeviceId = strdup(pdevice);
	dev->deviceId = strdup(devid);
	if (tmp) {
		char *t  = tmp;
		
		while ((t = strchr(t, '\n')))
			*t = ',';
		dev->compat = strdup(tmp);
	}
	if (pname) {
		dev->desc = malloc(strlen(pname) + strlen(devid) + 5);
		sprintf(dev->desc,"%s - %s",pname, devid);
	}
	else
		dev->desc = strdup(devid);
	setDriverAndClass(dev);
	if (probeClass & dev->type) {
		if (devlist)
			dev->next = devlist;
		devlist = (struct device *)dev;
	} else {
		isapnpFreeDevice(dev);
	}
	free(devid);
	return devlist;
}

struct device * isapnpProbe(enum deviceClass probeClass, int probeFlags, struct device *devlist) {
	int fd;
	int init_list = 0;
	
	if (
	    (probeClass & CLASS_OTHER) ||
	    (probeClass & CLASS_NETWORK) ||
	    (probeClass & CLASS_MODEM) ||
	    (probeClass & CLASS_AUDIO)
	    ) {
		DIR *dir;
		struct dirent *entry;
		
		if (!isapnpDeviceList) {
			isapnpReadDrivers(NULL);
			init_list = 1;
		}
		dir = opendir("/sys/devices/");
		if (!dir)
			return devlist;
		while ((entry = readdir(dir))) {
			DIR *bdir;
			struct dirent *bentry;
			char path[256];
			
			if (strncmp(entry->d_name,"pnp",3))
				continue;
			snprintf(path,255,"/sys/devices/%s",entry->d_name);
			bdir = opendir(path);
			if (!bdir)
				continue;
			
			while ((bentry = readdir(bdir))) {
				DIR *ddir;
				struct dirent *dentry;
				char dpath[256];
				char *pdevice  = NULL;
				char *pname = NULL;
				
				if (!isdigit(bentry->d_name[0]))
					continue;
				snprintf(dpath,255,"%s/%s",
					 path,bentry->d_name);
				ddir = opendir(dpath);
				
				snprintf(dpath,255,"%s/%s/id",
					 path,bentry->d_name);
				fd = open(dpath,O_RDONLY);
				if (fd >= 0)
					devlist = isapnpAddDevice(fd, pdevice, pname, probeClass,
						devlist);
				else {
					snprintf(dpath,255,"%s/%s/card_id",
						 path,bentry->d_name);
					fd = open(dpath,O_RDONLY);
					if (fd >= 0) {
						pdevice = __bufFromFd(fd);
						pdevice[strlen(pdevice) - 1] = '\0';
					}
					snprintf(dpath,255,"%s/%s/name",
						 path,bentry->d_name);
					fd = open(dpath,O_RDONLY);
					if (fd >= 0) {
						pname = __bufFromFd(fd);
						pname[strlen(pname) - 1] = '\0';
					}
				
					while ((dentry = readdir(ddir))) {
						char devpath[256];
					
						if (!isdigit(dentry->d_name[0]))
							continue;
						snprintf(devpath,255,"%s/%s/%s/id",
							 path,bentry->d_name,
							 dentry->d_name);
						fd = open(devpath, O_RDONLY);
						if (fd >= 0)
							devlist = isapnpAddDevice(fd, pdevice, pname,
								probeClass, devlist);
					}
					free(pdevice);
				}
				closedir(ddir);
			}
			closedir(bdir);
		}
		closedir(dir);
	}
	if (isapnpDeviceList && init_list)
	  isapnpFreeDrivers();
	return devlist;
}
