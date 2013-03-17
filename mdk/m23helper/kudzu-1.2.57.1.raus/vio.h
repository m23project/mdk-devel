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

#ifndef _KUDZU_VIO_H_
#define _KUDZU_VIO_H_

#include "device.h"

struct vioDevice {
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
    /* vio-specific fields */
    struct vioDevice *(*newDevice) (struct vioDevice *dev);
    void (*freeDevice) (struct vioDevice *dev);
    void (*writeDevice) (FILE *file, struct vioDevice *dev);
    int (*compareDevice) (struct vioDevice *dev1, struct vioDevice *dev2);
};

struct vioDevice *vioNewDevice(struct vioDevice *dev);
struct device *vioProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);

#endif
