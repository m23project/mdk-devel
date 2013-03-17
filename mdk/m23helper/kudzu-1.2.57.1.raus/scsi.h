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

#ifndef _KUDZU_SCSI_H_
#define _KUDZU_SCSI_H_

#include "device.h"
#include "kudzu.h"

struct scsiDevice {
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
    /* SCSI-specific fields */
    struct scsiDevice *(*newDevice) (struct scsiDevice *dev);
    void (*freeDevice) (struct scsiDevice *dev);
    void (*writeDevice) (FILE *file, struct scsiDevice *dev);
	int (*compareDevice) (struct scsiDevice *dev1, struct scsiDevice *dev2);
    unsigned int host;
    unsigned int channel;
    unsigned int id;
    unsigned int lun;
};

struct scsiDevice *scsiNewDevice(struct scsiDevice *dev);
struct device *scsiProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);

#endif
