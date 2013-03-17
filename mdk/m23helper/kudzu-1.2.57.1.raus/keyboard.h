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

#ifndef _KUDZU_KEYBOARD_H_
#define _KUDZU_KEYBOARD_H_

#include "device.h"

struct keyboardDevice {
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
    /* keyboard-specific fields */
    struct keyboardDevice *(*newDevice) (struct keyboardDevice *dev);
    void (*freeDevice) (struct keyboardDevice *dev);
    void (*writeDevice) (FILE *file, struct keyboardDevice *dev);
	int (*compareDevice) (struct keyboardDevice *dev1, struct keyboardDevice *dev2);
};

struct keyboardDevice *keyboardNewDevice(struct keyboardDevice *dev);
struct device *keyboardProbe(enum deviceClass probeClass, int probeFlags,
			     struct device *devlist);

#endif
