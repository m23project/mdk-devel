/* adb.c: Probe for adb devices on Apple machines.
 * 
 * Copyright (C) 2003 Dan Burcaw <dburcaw@terrasoftsolutions.com>
 *           (C) 2003 Terra Soft Solutions, Inc.
 *           (C) 2003 Red Hat, Inc.
 *
 * Based on sbus.c
 * Copyright (C) 1998, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 *           (C) 1999, 2000 Red Hat, Inc.
 * 
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "adb.h"

static void adbFreeDevice( struct adbDevice *dev ) {
    freeDevice( (struct device *)dev);
}

static void adbWriteDevice( FILE *file, struct adbDevice *dev) {
    writeDevice(file, (struct device *) dev);
}

static int adbCompareDevice(struct adbDevice *dev1, struct adbDevice *dev2)
{
	        return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct adbDevice *adbNewDevice( struct adbDevice *old ) {
    struct adbDevice *ret;

    ret = malloc( sizeof(struct adbDevice) );
    memset(ret, '\0', sizeof (struct adbDevice));
    ret=(struct adbDevice *)newDevice((struct device *)old,(struct device *)ret);
    ret->bus = BUS_ADB;
    ret->newDevice = adbNewDevice;
    ret->freeDevice = adbFreeDevice;
    ret->writeDevice = adbWriteDevice;
    ret->compareDevice = adbCompareDevice;
    return ret;
}

#ifdef __powerpc__

#include <linux/adb.h>
#define ADB_QUERY       7
#define ADB_QUERY_GETDEVINFO    1

/* given a class to probe, returns an array adb devices found which match */
/* all entries are malloc'd, so caller must free when done. Use           */
/* dev->freeDevice(dev) to free individual adbDevice's, since they 	  */
/* have internal elements which are malloc'd                              */

struct device *adbProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist) {


    if (probeClass & CLASS_MOUSE) {
	struct adbDevice *mousedev;
	int i, fd, rc;
	unsigned char cmd[16];
	unsigned char rep[16];

        if ((fd = open("/dev/adb", O_RDWR)) != -1) {
		for (i=0; i<16; i++) {
		    cmd[0] = ADB_QUERY;
		    cmd[1] = ADB_QUERY_GETDEVINFO,
		    cmd[2] = i;

		    rc = write(fd, cmd, 3);
		    /* write error */
		    if (rc == -1) {
			continue;
		    }

		    rc = read(fd, rep, 2);
		    /* read error */
		    if (rc != 2) {
			continue;
		    }

		    switch (rep[0]) {
			case 3:
			    mousedev = adbNewDevice(NULL);
			    mousedev->type = CLASS_MOUSE;
			    mousedev->device = strdup("input/mice");

			    /* We need to figure out if this is a trackpad  */
			    /* or just a generic adb mouse so we can report */
			    /* the correct information to mouseconfig, etc. */
			    cmd[0] = ADB_PACKET;
			    cmd[1] = ADB_READREG(i, 1);
			    
			    rc = write(fd, cmd, 2);
			    /* write error */
			    if (rc == -1) {
				continue;
			    }

			    rc = read(fd, rep, 5);
		     	    /* read error */
			    if (rc != 5) {
				continue;
			    }
			    if ((rep[1] == 0x74) && (rep[2] == 0x70) &&
				(rep[3] == 0x61) && (rep[4] == 0x64)) {
				    mousedev->desc = strdup("Apple Trackpad");
			    	    mousedev->driver = strdup("appletpad");
			    }
			    else {
				    mousedev->desc = strdup("Generic ADB Mouse");
			    	    mousedev->driver = strdup("genericadb");
			    }

			    mousedev->next = devlist;
			    devlist = (struct device *)mousedev;
			    break;

			default:
			    continue;
		    }
		}
		close (fd);
	}
    }
    return devlist;
}

#else

struct device *adbProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist ) {
    return devlist;
}

#endif
