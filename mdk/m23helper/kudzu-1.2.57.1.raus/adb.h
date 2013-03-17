/* adb.h: Probe for adb devices on Apple machines.
 *
 * Copyright (C) 2003 Dan Burcaw <dburcaw@terrasoftsolutions.com>
 * 	      (C) 2003 Terra Soft Solutions, Inc.
 *           (C) 2003 Red Hat, Inc.
 *
 * Based on sbus.h 
 * Copyright (C) 1998, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 *           (C) 1999 Red Hat, Inc.
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

#ifndef _KUDZU_ADB_H_
#define _KUDZU_ADB_H_

#include "device.h"

struct adbDevice {
    /* common fields */
    struct device *next;        /* next device in list */
	int index;
    enum deviceClass type;     /* type */
    enum deviceBus bus;         /* bus it's attached to */
    char * device;              /* device file associated with it */
    char * driver;              /* driver to load, if any */
    char * desc;                /* a description */
	int detached;
    void * classprivate;
    /* adb specific fields */
    struct adbDevice *(*newDevice) (struct adbDevice *dev );
    void (*freeDevice) ( struct adbDevice *dev );
    void (*writeDevice) (FILE *file, struct adbDevice *dev );
    int (*compareDevice) (struct adbDevice *dev1, struct adbDevice *dev2);
};

struct adbDevice *adbNewDevice(struct adbDevice *dev);
struct device *adbProbe(enum deviceClass probeClass,
			       int probeFlags,
			       struct device *devlist);

#endif
