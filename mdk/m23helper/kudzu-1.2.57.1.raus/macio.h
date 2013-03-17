/* macio.h: Probe for i2o audio and airport on Apple machines.
 *
 * Copyright (C) 2002 Dan Burcaw <dburcaw@terrasoftsolutions.com>
 *           (C) 2002 Terra Soft Solutions, Inc.
 *	     (C) 2003 Red Hat, Inc.
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

#ifndef _KUDZU_MACIO_H_
#define _KUDZU_MACIO_H_

#include "device.h"

struct macioDevice {
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
    /* macio specific fields */
    struct macioDevice *(*newDevice) (struct macioDevice *dev );
    void (*freeDevice) ( struct macioDevice *dev );
    void (*writeDevice) (FILE *file, struct macioDevice *dev );
    int (*compareDevice) (struct macioDevice *dev1, struct macioDevice *dev2);
};

struct macioDevice *macioNewDevice(struct macioDevice *dev);
struct device *macioProbe(enum deviceClass probeClass,
			       int probeFlags,
			       struct device *devlist);

#endif
