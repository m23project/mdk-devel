/* Copyright 2002-2003 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _KUDZU_FIREWIRE_H_
#define _KUDZU_FIREWIRE_H_

#include "device.h"

struct firewireDevice {
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
    /* firewire-specific fields */
    struct firewireDevice *(*newDevice) (struct firewireDevice *dev);
    void (*freeDevice) (struct firewireDevice *dev);
    void (*writeDevice) (FILE *file, struct firewireDevice *dev);
    int (*compareDevice) (struct firewireDevice *dev1, struct firewireDevice *dev2);
};

struct firewireDevice *firewireNewDevice(struct firewireDevice *dev);
struct device *firewireProbe(enum deviceClass probeClass, int probeFlags,
			     struct device *devlist);

#endif
