/* Copyright 2003 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _KUDZU_PCMCIA_H_
#define _KUDZU_PCMCIA_H_

#include "device.h"
#include "kudzu.h"

struct pcmciaDevice {
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
	/* pcmcia-specific fields */
	struct pcmciaDevice  *(*newDevice) (struct pcmciaDevice *dev);
	void (*freeDevice) (struct pcmciaDevice *dev);
	void (*writeDevice) (FILE *file, struct pcmciaDevice *dev);
	int (*compareDevice) (struct pcmciaDevice *dev1, struct pcmciaDevice *dev2);
	unsigned int vendorId;	/* vendor id */
	unsigned int deviceId;	/* device id */
	unsigned int function; /* logical function # */
	unsigned int slot; /* slot the card is in */
	unsigned int port; /* i/o port */
};

struct pcmciaDevice *pcmciaNewDevice(struct pcmciaDevice *dev);
struct device *pcmciaProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);
int pcmciaReadDrivers(char *filename);
void pcmciaFreeDrivers(void);
#endif
