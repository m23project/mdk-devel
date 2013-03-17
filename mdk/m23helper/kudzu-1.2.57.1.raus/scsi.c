/* Copyright 1999-2003 Red Hat, Inc.
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
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "scsi.h"
#include "modules.h"
#include "kudzuint.h"

static void scsiFreeDevice( struct scsiDevice *dev ) {
    freeDevice( (struct device *)dev);
}

static void scsiWriteDevice( FILE *file, struct scsiDevice *dev) {
	writeDevice(file, (struct device *)dev);
	fprintf(file,"host: %d\nid: %d\nchannel: %d\nlun: %d\n",
		dev->host, dev->id, dev->channel, dev->lun);
}

static int scsiCompareDevice (struct scsiDevice *dev1, struct scsiDevice *dev2)
{
        int c,h,i,l,x;
	x = compareDevice( (struct device *)dev1, (struct device *)dev2);
	if (x && x!=2) return x;
	c=dev1->channel-dev2->channel;
	h=dev1->host-dev2->host;
	i=dev1->id-dev2->id;
	l=dev1->lun-dev2->lun;
	if ( c || h || i || l) return 1;
	return x;
}

struct scsiDevice *scsiNewDevice( struct scsiDevice *old) {
    struct scsiDevice *ret;
    
    ret = malloc( sizeof(struct scsiDevice) );
    memset(ret, '\0', sizeof (struct scsiDevice));
    ret=(struct scsiDevice *)newDevice((struct device *)old,(struct device *)ret);
    ret->bus = BUS_SCSI;
    ret->newDevice = scsiNewDevice;
    ret->freeDevice = scsiFreeDevice;
	ret->writeDevice = scsiWriteDevice;
	ret->compareDevice = scsiCompareDevice;
    if (old && old->bus ==BUS_SCSI) {
	ret->host = old->host;
	ret->id = old->id;
	ret->channel = old->channel;
	ret->lun = old->lun;
    }
    return ret;
}

struct device *scsiProbe( enum deviceClass probeClass, int probeFlags,
			 struct device *devlist) {
	struct scsiDevice *sdev;
	DIR *dir;
	struct dirent *ent;
    
	if (
	    (probeClass & CLASS_OTHER) ||
	    (probeClass & CLASS_CDROM) ||
	    (probeClass & CLASS_TAPE) ||
	    (probeClass & CLASS_FLOPPY) ||
	    (probeClass & CLASS_SCANNER) ||
	    (probeClass & CLASS_HD)
	    ) {

	
		dir = opendir("/sys/bus/scsi/devices");
		if (!dir)
			return devlist;
	
		while ((ent = readdir(dir))) {
			char *tmp, *vendor, *model, *hostdriver;
			int type;
			DIR * scsidir;
			struct dirent *scsient;
			int cwd;
		
			if (ent->d_name[0] == '.')
				continue;
			sdev = scsiNewDevice(NULL);
		
			tmp = ent->d_name;
			sdev->host = atoi(tmp);
			while (*tmp && *tmp != ':') tmp++;
			if (!*tmp) {
				scsiFreeDevice(sdev);
				continue;
			}
			tmp++;
			sdev->channel = atoi(tmp);
			while (*tmp && *tmp != ':') tmp++;
			if (!*tmp) {
				scsiFreeDevice(sdev);
				continue;
			}
			tmp++;
			sdev->id = atoi(tmp);
			while (*tmp && *tmp != ':') tmp++;
			if (!*tmp) {
				scsiFreeDevice(sdev);
				continue;
			}
			tmp++;
			sdev->lun = atoi(tmp);

			asprintf(&tmp,"/sys/bus/scsi/devices/%s",ent->d_name);
			cwd = open(".",O_RDONLY);
			chdir(tmp);
			model = __readString("model");
			vendor = __readString("vendor");
			type = __readInt("type");
			scsidir = opendir(tmp);
			while ((scsient = readdir(scsidir))) {
				__getSysfsDevice((struct device *)sdev, ".", "block:",0);
				if (!sdev->device)
					__getSysfsDevice((struct device *)sdev, ".", "tape:",0);
				if (!sdev->device)
					__getSysfsDevice((struct device *)sdev, ".", "scsi_generic:",0);
			}
			/* Yay for kernel device names. */
			if (sdev->device && !strncmp(sdev->device,"sr",2)) {
				int num = atoi(sdev->device+2);
				
				free(sdev->device);
			        asprintf(&sdev->device,"scd%d",num);
			}
			asprintf(&sdev->desc,"%s %s", vendor, model);
			free(vendor);
			free(model);
			fchdir(cwd);
			close(cwd);
			switch (type) {
			case 0x00:
			case 0x07:
			case 0x0e:
				sdev->type = CLASS_HD;
				break;
			case 0x01:
				sdev->type = CLASS_TAPE;
				break;
			case 0x04:
			case 0x05:
				sdev->type = CLASS_CDROM;
				break;
			case 0x06:
				sdev->type = CLASS_FLOPPY;
				break;
			default:
				sdev->type = CLASS_OTHER;
			}
			/* Haaaack */
			asprintf(&tmp,"/sys/class/scsi_host/host%d/proc_name",sdev->host);
			hostdriver = __readString(tmp);
			free(tmp);
			if (!strcmp(hostdriver,"usb-storage")) {
				int fd;
				char *buf;
				
				asprintf(&tmp,"/proc/scsi/usb-storage/%d",sdev->host);
				fd = open(tmp,O_RDONLY);
				free(tmp);
				buf = __bufFromFd(fd);
				if (strstr(buf, "Protocol: Uniform Floppy Interface (UFI)")) {
					sdev->type = CLASS_FLOPPY;
				}
				free(buf);
			}
			free(hostdriver);
			if (sdev->type & probeClass) {
				if (devlist)
					sdev->next = devlist;
				devlist = (struct device *)sdev; 
			} else {
				scsiFreeDevice(sdev);
			}
		}
	}
	return devlist;
}
