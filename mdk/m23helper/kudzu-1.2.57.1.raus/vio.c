/* Copyright 2004 Red Hat, Inc.
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

#include "vio.h"
#include "kudzuint.h"
#include "modules.h"

static void vioFreeDevice(struct vioDevice *dev)
{
	freeDevice((struct device *) dev);
}

static void vioWriteDevice(FILE *file, struct vioDevice *dev)
{
	writeDevice(file, (struct device *)dev);
}

static int vioCompareDevice(struct vioDevice *dev1, struct vioDevice *dev2)
{
	return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct vioDevice *vioNewDevice(struct vioDevice *old)
{
	struct vioDevice *ret;

	ret = malloc(sizeof(struct vioDevice));
	memset(ret, '\0', sizeof(struct vioDevice));
	ret = (struct vioDevice *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_VIO;
	ret->newDevice = vioNewDevice;
	ret->freeDevice = vioFreeDevice;
	ret->writeDevice = vioWriteDevice;
	ret->compareDevice = vioCompareDevice;
	return ret;
}

struct device *vioProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
#ifdef __powerpc__
    struct vioDevice *viodev;
    int fd;

    if (probeClass & CLASS_HD) {
      if (!access("/proc/device-tree/mambo", R_OK)) {
	DIR * dir;
	struct dirent * ent;
	int ctlNum;

	dir = opendir("/proc/device-tree/mambo");
	while ((ent = readdir(dir))) {
	    if (strncmp("bogus-disc@", ent->d_name, 11))
		continue;
	    ctlNum = atoi(ent->d_name + 11);
	    viodev = vioNewDevice(NULL);
	    viodev->device = malloc(20);
	    snprintf(viodev->device, 19, "mambobd%d", ctlNum);
	    viodev->desc = strdup("IBM Mambo virtual disk");
	    viodev->type = CLASS_HD;
	    viodev->driver = strdup("mambo_bd");
	    if (devlist)
	      viodev->next = devlist;
	    devlist = (struct device *) viodev;
	}
      }
      if (!access("/sys/bus/vio/drivers/viodasd", R_OK)) {
	DIR * dir;
	struct dirent * ent;
	int ctlNum;

	dir = opendir("/sys/bus/vio/drivers/viodasd");
	while ((ent = readdir(dir))) {
	    if (strncmp("viodasd", ent->d_name, 7))
		continue;
	    ctlNum = atoi(ent->d_name + 7);
	    viodev = vioNewDevice(NULL);
	    viodev->device = malloc(20);
	    if (ctlNum < 26) {
	      snprintf(viodev->device, 19, "iseries/vd%c", 'a' + ctlNum);
	    } else {
	      snprintf(viodev->device, 19, "iseries/vda%c", 'a' + ctlNum - 26);
	    }
	    viodev->desc = strdup("IBM Virtual DASD");
	    viodev->type = CLASS_HD;
	    viodev->driver = strdup("viodasd");
	    if (devlist)
	      viodev->next = devlist;
	    devlist = (struct device *) viodev;
	}
      } else {
	char * buf = NULL, * start, * end, * chptr, * next, * model, * ptr;
	int ctlNum = 0;

	if (access("/proc/iSeries/viodasd", R_OK))
	    goto dasddone;

	/* Read from /proc/iSeries/viodasd */
	fd = open("/proc/iSeries/viodasd", O_RDONLY);
	if (fd < 0) {
	    /* fprintf(stderr, "failed to open /proc/iSeries/viodasd!\n");*/
	    goto dasddone;
	}

	start = buf = __bufFromFd(fd);
	close(fd);

	end = start ? start + strlen(start) : NULL;
	while (start && *start && start < end) {
	    /* parse till end of line and store the start of next line. */
	    chptr = start;
	    while (*chptr != '\n') chptr++;
	    *chptr = '\0';
	    next = chptr + 1;

	    /* get rid of anything which is not alpha */
	    while (!(isalpha(*start))) start++;

	    model = NULL;
	    if (!strncmp("DISK ", start, 5))
		model = "IBM Virtual DASD";

	    if (model) {
		chptr = start += 5;
		ptr = strchr(chptr, ' ');
                if (ptr)
                    *ptr = '\0';
		ctlNum = atoi(chptr);

		viodev = vioNewDevice(NULL);
		viodev->device = malloc(20);
		if (ctlNum < 26) {
		    snprintf(viodev->device, 19, "iseries/vd%c", 'a' + ctlNum);
		} else {
		    snprintf(viodev->device, 19, "iseries/vda%c", 'a' + ctlNum - 26);
		}
		viodev->desc = malloc(64);
		viodev->desc = strdup(model);
		viodev->type = CLASS_HD;
		viodev->driver = strdup("viodasd");
		if (devlist) 
		    viodev->next = devlist;
		devlist = (struct device *) viodev;
	    }

	    start = next;
	    end = start + strlen(start);
	} /* end of while */

dasddone:
	if (buf)
	    free (buf);
      }
    } 

    if (probeClass & CLASS_NETWORK) {
      if (!access("/proc/device-tree/vdevice", R_OK)) {
        DIR * dir;
        struct dirent * ent;

        dir = opendir("/proc/device-tree/vdevice");
        while ((ent = readdir(dir))) {
            char *f, *tmp;
            DIR *devdir;
		
	    if (strncmp("l-lan@", ent->d_name, 6))
		continue;
	    viodev = vioNewDevice(NULL);
	    viodev->device = strdup("eth");
	    viodev->desc = strdup("IBM i/pSeries Virtual Ethernet");
	    viodev->type = CLASS_NETWORK;
	    viodev->driver = strdup("ibmveth");
	    asprintf(&f,"/sys/bus/vio/devices/%s",ent->d_name+6);
	    __getSysfsDevice(viodev, f, "net:",0);
	    __getNetworkAddr(viodev, viodev->device);
	    if (devlist)
	      viodev->next = devlist;
	    devlist = (struct device *) viodev;
	}
	closedir(dir);
      }
      if (!access("/sys/bus/vio/devices", R_OK)) {
	DIR * dir;
	struct dirent * ent;

	dir = opendir("/sys/bus/vio/devices");
	while ((ent = readdir(dir))) {
	    if (strncmp("vlan", ent->d_name, 4))
		continue;
	    viodev = vioNewDevice(NULL);
	    viodev->device = strdup("eth");
	    viodev->desc = strdup("IBM Virtual Ethernet");
	    viodev->type = CLASS_NETWORK;
	    viodev->driver = strdup("iseries_veth");
	    if (devlist)
	      viodev->next = devlist;
	    devlist = (struct device *) viodev;
	}
        closedir(dir);
      }
      if (!access("/proc/device-tree/mambo/bogus-net@0", R_OK)) {

	viodev = vioNewDevice(NULL);
	viodev->device = strdup("eth");
	viodev->desc = strdup("Mambo Virtual Ethernet");
	viodev->type = CLASS_NETWORK;
	viodev->driver = strdup("mambonet");
	if (devlist)
	  viodev->next = devlist;
	devlist = (struct device *) viodev;
      }
    } 

    if (probeClass & CLASS_CDROM) {
	char *buf, *ptr, *b;
	int cdnum;

	if (access("/proc/iSeries/viocd", R_OK))
	    goto viocddone;
	fd = open("/proc/iSeries/viocd", O_RDONLY);
	if (fd < 0)
	    goto viocddone;
	b = buf = __bufFromFd(fd);
	while (buf && *buf) {
	    ptr = buf;
	    while (*ptr && *ptr != '\n')
		ptr++;
	    if (*ptr) {
		*ptr = '\0';
		ptr++;
	    }
	    if (strncmp("viocd", buf, 5)) {
		buf = ptr;
		continue;
	    }
	    buf +=13;
	    cdnum = atoi(buf);
	    buf = strstr(buf,"type");
	    viodev = vioNewDevice(NULL);
	    viodev->device = malloc(20);
	    snprintf(viodev->device,19,"iseries/vcd%c", cdnum + 'a');
	    viodev->desc = malloc(64);
	    if (buf)
		snprintf(viodev->desc,63,"IBM Virtual CD-ROM %s",buf);
	    else
		snprintf(viodev->desc,63,"IBM Virtual CD-ROM");
	    viodev->type = CLASS_CDROM;
	    if (devlist)
		viodev->next = devlist;
	    devlist = (struct device *) viodev;
	    buf = ptr;
	    continue;
	}
	if (b) free(b);
viocddone:
	    ;
    }

    if (probeClass & CLASS_SCSI) {
        if (!access("/proc/device-tree/vdevice", R_OK)) {
            DIR * dir;
            struct dirent * ent;

            dir = opendir("/proc/device-tree/vdevice");
            while ((ent = readdir(dir))) {
                if (strncmp("v-scsi@", ent->d_name, 7))
                    continue;
                viodev = vioNewDevice(NULL);
                viodev->desc = strdup("IBM Virtual SCSI");
                viodev->type = CLASS_SCSI;
                viodev->driver = strdup("ibmvscsic");
                if (devlist)
                    viodev->next = devlist;
                devlist = (struct device *) viodev;
            }
        }
    }
#endif
    return devlist;
}
