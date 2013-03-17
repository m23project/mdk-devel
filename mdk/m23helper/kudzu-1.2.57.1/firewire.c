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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "firewire.h"
#include "modules.h"

#include "kudzuint.h"

static void firewireFreeDevice(struct firewireDevice *dev)
{
	freeDevice((struct device *) dev);
}

static void firewireWriteDevice(FILE *file, struct firewireDevice *dev)
{
	writeDevice(file, (struct device *)dev);
}

static int firewireCompareDevice(struct firewireDevice *dev1, struct firewireDevice *dev2)
{
	return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct firewireDevice *firewireNewDevice(struct firewireDevice *old)
{
	struct firewireDevice *ret;

	ret = malloc(sizeof(struct firewireDevice));
	memset(ret, '\0', sizeof(struct firewireDevice));
	ret = (struct firewireDevice *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_FIREWIRE;
	ret->newDevice = firewireNewDevice;
	ret->freeDevice = firewireFreeDevice;
	ret->writeDevice = firewireWriteDevice;
	ret->compareDevice = firewireCompareDevice;
	return ret;
}

struct device *firewireProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
	struct firewireDevice *fwdev;
	
	if (
	    (probeClass & CLASS_SCSI) 
	    ) {
		DIR *dir;
		struct dirent *entry;
		int fd;
		
		dir = opendir("/sys/bus/ieee1394/devices");
		if (!dir)
			goto out;
		
		while ((entry = readdir(dir))) {
			char path[256];
			char *specifier_id, *version;
			if (entry->d_name[0] == '.')
				continue;
			snprintf(path,255,"/sys/bus/ieee1394/devices/%s/specifier_id",entry->d_name);
			fd = open(path, O_RDONLY);
			if (fd < 0)
				continue;
			specifier_id = __bufFromFd(fd);
			if (!specifier_id) continue;
			specifier_id[strlen(specifier_id) - 1] = '\0';
			snprintf(path,255,"/sys/bus/ieee1394/devices/%s/version",entry->d_name);
			fd = open(path, O_RDONLY);
			if (fd < 0) {
				free(specifier_id);
				continue;
			}
			version = __bufFromFd(fd);
			if (!version) {
				free(specifier_id);
				continue;
			}
			version[strlen(version) - 1] = '\0';
				
			if (!strcmp(version,"0x010483") &&
			    !strcmp(specifier_id,"0x00609e")) {
				fwdev = firewireNewDevice(NULL);
				fwdev->driver = strdup("sbp2");
				fwdev->type = CLASS_SCSI;
				if (devlist)
					fwdev->next = devlist;
				snprintf(path,255,"/sys/bus/ieee1394/devices/%s/model_name_kv",entry->d_name);
				fd = open(path, O_RDONLY);
				if (fd >= 0) {
					fwdev->desc = __bufFromFd(fd);
					fwdev->desc[strlen(fwdev->desc) - 1] = '\0';
				} else
					fwdev->desc = strdup("Generic IEEE-1394 Storage Device");
				devlist = (struct device *) fwdev;
			}
			free(version);
			free(specifier_id);
		}
	}
out:
	return devlist;
}
