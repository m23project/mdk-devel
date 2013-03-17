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

#ifndef _KUDZU_XEN_H_
#define _KUDZU_XEN_H_

#include "device.h"

struct xenDevice {
    /* common fields */
    struct device *next;	/* next device in list */
    int index;
    enum deviceClass type;	/* type */
    enum deviceBus bus;		/* bus it's attached to */
    char * device;		/* device file associated with it */
    char * driver;		/* driver to load, if any */
    char * desc;		/* a description */
    int detached;
    void * classprivate;
    /* xen-specific fields */
    struct xenDevice *(*newDevice) (struct xenDevice *dev);
    void (*freeDevice) (struct xenDevice *dev);
    void (*writeDevice) (FILE *file, struct xenDevice *dev);
    int (*compareDevice) (struct xenDevice *dev1, struct xenDevice *dev2);
};

struct xenDevice *xenNewDevice(struct xenDevice *dev);
struct device *xenProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);

#endif
