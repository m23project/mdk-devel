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

#ifndef _KUDZU_S390_H_
#define _KUDZU_S390_H_

#include "device.h"

struct s390Device {
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
    /* s390-specific fields */
    struct s390Device *(*newDevice) (struct s390Device *dev);
    void (*freeDevice) (struct s390Device *dev);
    void (*writeDevice) (FILE *file, struct s390Device *dev);
    int (*compareDevice) (struct s390Device *dev1, struct s390Device *dev2);
};

struct s390Device *s390NewDevice(struct s390Device *dev);
struct device *s390Probe(enum deviceClass probeClass, int probeFlags,
			 struct device *devlist);

#endif
