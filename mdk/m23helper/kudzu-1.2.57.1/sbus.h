/* sbus.c: Probe for Sun SBUS and UPA framebuffers using OpenPROM,
 *         SBUS SCSI and Ethernet cards and SBUS or EBUS audio chips.
 *
 * Copyright (C) 1998, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 *           (C) 1999-2003 Red Hat, Inc.
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

#ifndef _KUDZU_SBUS_H_
#define _KUDZU_SBUS_H_

#include "device.h"

struct sbusDevice {
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
    /* sbus specific fields */
    struct sbusDevice *(*newDevice) (struct sbusDevice *dev );
    void (*freeDevice) ( struct sbusDevice *dev );
    void (*writeDevice) (FILE *file, struct sbusDevice *dev );
	int (*compareDevice) (struct sbusDevice *dev1, struct sbusDevice *dev2);
    
    int                width;
    int               height;
    int                 freq;
    int              monitor;
};

struct sbusDevice *sbusNewDevice(struct sbusDevice *dev);
struct device *sbusProbe(enum deviceClass probeClass,
			       int probeFlags,
			       struct device *devlist);

#endif
