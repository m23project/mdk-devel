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

#ifndef _KUDZU_IDE_H_
#define _KUDZU_IDE_H_

#include "device.h"

struct ideDevice {
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
    /* IDE-specific fields */
    struct ideDevice *(*newDevice) (struct ideDevice *dev);
    void (*freeDevice) (struct ideDevice *dev);
    void (*writeDevice) (FILE *file, struct ideDevice *dev);
    int (*compareDevice) (struct ideDevice *dev1, struct ideDevice *dev2);
    char * physical;
    char * logical;
};

struct ideDevice *ideNewDevice(struct ideDevice *dev);
struct device *ideProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);

#endif
