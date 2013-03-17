/* Copyright 2003-2005 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "pcmcia.h"
#include "alias.h"

#include "kudzuint.h"

static int devCmp(const void * a, const void * b) {
	const struct pcmciaDevice * one = a;
	const struct pcmciaDevice * two = b;
	int x=0,y=0, z=0;
    
	x = (one->vendorId - two->vendorId);
	y = (one->deviceId - two->deviceId);
	z = (one->function - two->function);
	if (x)
		return x;
	else if (y)
		return y;
	else return z;
}

static void pcmciaFreeDevice(struct pcmciaDevice *dev) {
    freeDevice((struct device *)dev);
}

static void pcmciaWriteDevice(FILE *file, struct pcmciaDevice *dev) {
	writeDevice(file, (struct device *)dev);
	fprintf(file,"vendorId: %04x\ndeviceId: %04x\nfunction: %d\nslot: %d\n",dev->vendorId,dev->deviceId,dev->function,dev->slot);
}

static int pcmciaCompareDevice(struct pcmciaDevice *dev1, struct pcmciaDevice *dev2)
{
	int x,y;
	
	x = compareDevice((struct device *)dev1,(struct device *)dev2);
	if (x && x!=2) 
	  return x;
	if ((y=devCmp( (void *)dev1, (void *)dev2 )))
	  return y;
	return x;
}
			    
struct pcmciaDevice * pcmciaNewDevice(struct pcmciaDevice *dev) {
    struct pcmciaDevice *ret;
    
    ret = malloc(sizeof(struct pcmciaDevice));
    memset(ret,'\0',sizeof(struct pcmciaDevice));
    ret=(struct pcmciaDevice *)newDevice((struct device *)dev,(struct device *)ret);
    ret->bus = BUS_PCMCIA;
    if (dev && dev->bus == BUS_PCMCIA) {
	ret->vendorId = dev->vendorId;
	ret->deviceId = dev->deviceId;
	ret->slot = dev->slot;
	ret->function = dev->function;
    }
    ret->newDevice = pcmciaNewDevice;
    ret->freeDevice = pcmciaFreeDevice;
    ret->writeDevice = pcmciaWriteDevice;
    ret->compareDevice = pcmciaCompareDevice;
    return ret;
}

int pcmciaReadDrivers(char *filename) {
	
	aliases = readAliases(aliases, filename, "pcmcia");
	return 0;
}

void pcmciaFreeDrivers(void) {
}

static enum deviceClass pcmciaToKudzu(unsigned int class) {
	if (!class) return CLASS_UNSPEC;
	switch (class) {
	case 0x02:
		return CLASS_MODEM;
	case 0x04:
		return CLASS_HD;
	case 0x05:
		return CLASS_VIDEO;
	case 0x06:
		return CLASS_NETWORK;
	case 0x08:
		return CLASS_SCSI;
	default:
		return CLASS_OTHER;
	}
}

static char *readId() {
	char *ret, *tmp, *t;
	
	tmp = __readString("prod_id1");
	ret = tmp;
	tmp = __readString("prod_id2");
	if (!tmp)
		return ret;
        asprintf(&t,"%s %s",ret, tmp);
	free(ret);
	ret = t;
	tmp = __readString("prod_id3");
	if (!tmp)
		return ret;
        asprintf(&t,"%s %s",ret, tmp);
	free(ret);
	ret = t;
	tmp = __readString("prod_id4");
	if (!tmp)
		return ret;
        asprintf(&t,"%s %s",ret, tmp);
	free(ret);
	ret = t;
	return ret;
}

struct device * pcmciaProbe(enum deviceClass probeClass, int probeFlags, struct device *devlist) {
	
	int init_list =0;
	DIR *dir;
	struct dirent *entry;
	int cwd;

	if (
	    (probeClass & CLASS_OTHER) ||
	    (probeClass & CLASS_NETWORK) ||
	    (probeClass & CLASS_SCSI) ||
	    (probeClass & CLASS_MODEM)) {
		
		if (!getAliases(aliases, "pcmcia")) {
			pcmciaReadDrivers(NULL);
			init_list = 1;
		}
		
		dir = opendir("/sys/bus/pcmcia/devices");
		if (!dir)
			goto out;
		cwd = open(".",O_RDONLY);
		while ((entry = readdir(dir))) {
			struct pcmciaDevice *dev;
			char *path, *tmp, *drv;
			int func;
			
			if (entry->d_name[0] == '.')
				continue;
			
			asprintf(&path,"/sys/bus/pcmcia/devices/%s",entry->d_name);
			chdir(path);
			
			dev = pcmciaNewDevice(NULL);
			dev->slot = strtoul(entry->d_name,&tmp,10);
			if (tmp) {
				dev->function = strtoul(tmp+1,NULL,10);
			}
			dev->vendorId = __readHex("manf_id");
			dev->deviceId = __readHex("card_id");
			func = __readHex("func_id");
			dev->type = pcmciaToKudzu(func);
			dev->desc = readId();
			tmp = __readString("modalias");
			__getSysfsDevice((struct device *)dev, path, "net:",0);
			if (dev->device) {
				dev->type = CLASS_NETWORK;
				__getNetworkAddr((struct device *)dev, dev->device);
			}
			if (dev->type == CLASS_NETWORK && !dev->device)
				dev->device = strdup("eth");
			drv = aliasSearch(aliases,"pcmcia",tmp+7);
			if (drv)
				dev->driver = strdup(drv);
			if (probeClass & dev->type &&
			    ((probeFlags & PROBE_ALL) || dev->driver)) {
				if (devlist)
					dev->next = devlist;
				devlist = (struct device *)dev;
			}
		}
		closedir(dir);
		fchdir(cwd);
		close(cwd);
	}
out:
	if (init_list)
		pcmciaFreeDrivers();
	return devlist;
}

