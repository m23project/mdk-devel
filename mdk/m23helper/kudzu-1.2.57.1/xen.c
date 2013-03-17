/* Copyright 2005 Red Hat, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fd.h>

#include "xen.h"
#include "kudzuint.h"
#include "modules.h"

static void xenFreeDevice(struct xenDevice *dev)
{
	freeDevice((struct device *) dev);
}

static void xenWriteDevice(FILE *file, struct xenDevice *dev)
{
	writeDevice(file, (struct device *)dev);
}

static int xenCompareDevice(struct xenDevice *dev1, struct xenDevice *dev2)
{
	return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct xenDevice *xenNewDevice(struct xenDevice *old)
{
	struct xenDevice *ret;

	ret = malloc(sizeof(struct xenDevice));
	memset(ret, '\0', sizeof(struct xenDevice));
	ret = (struct xenDevice *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_XEN;
	ret->newDevice = xenNewDevice;
	ret->freeDevice = xenFreeDevice;
	ret->writeDevice = xenWriteDevice;
	ret->compareDevice = xenCompareDevice;
	return ret;
}

struct device *xenProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
    struct xenDevice *xendev;
	
    if ((probeClass & CLASS_HD) ||
	(probeClass & CLASS_NETWORK) ||
	(probeClass & CLASS_VIDEO)) {
	    DIR * dir;
	    struct dirent * ent;
	    char path[64];
 
	    if (access("/sys/bus/xen/devices", R_OK)) 
		    return devlist;
	    
	    dir = opendir("/sys/bus/xen/devices");
	    while ((ent = readdir(dir))) {

		    if (!strncmp("vbd-", ent->d_name, 4) && (probeClass & CLASS_HD)) {
			    snprintf(path, 64, "/sys/bus/xen/devices/%s", ent->d_name);
			    xendev = xenNewDevice(NULL);
			    if (!xendev->device)
				    xendev->device = strdup("xvd");
			    xendev->desc = strdup("Xen Virtual Block Device");
			    xendev->type = CLASS_HD;
			    xendev->driver = strdup("xenblk");
			    __getSysfsDevice((struct device *)xendev, path, "block:",1);
			    if (devlist)
				    xendev->next = devlist;
			    devlist = (struct device *) xendev;
		    }
		    if (!strncmp("vif-", ent->d_name, 4) && (probeClass & CLASS_NETWORK)) {
			    xendev = xenNewDevice(NULL);
			    snprintf(path, 64, "/sys/bus/xen/devices/%s",ent->d_name);
			    __getSysfsDevice((struct device *)xendev, path, "net:",0);
			    if (xendev->device)
				    __getNetworkAddr((struct device *)xendev, xendev->device);
			    else
				    xendev->device = strdup("eth");
			    xendev->desc = strdup("Xen Virtual Ethernet");
			    xendev->type = CLASS_NETWORK;
			    xendev->driver = strdup("xennet");
			    if (devlist)
				    xendev->next = devlist;
			    devlist = (struct device *) xendev;
		    }
	    }
	    closedir(dir);
	    if (probeClass & CLASS_VIDEO) {
		    char path[64];
		    int i;
		    char *name;
		    
		    for (i = 0; ; i++) {
			    snprintf(path,64,"/sys/class/graphics/fb%d/name",i);
			    name = __readString(path);
			    if (!name)
				    break;
			    if (!strcmp(name, "xen")) {
				    xendev = xenNewDevice(NULL);
				    xendev->desc = strdup("Xen Virtual Framebuffer");
				    xendev->type = CLASS_VIDEO;
				    xendev->driver = strdup("xenfb");
				    xendev->classprivate = (void *)strdup("fbdev");
				    if (devlist)
					    xendev->next = devlist;
				    devlist = (struct device *) xendev;
			    }
		    }
		    
	    }
    }
    return devlist;
}
