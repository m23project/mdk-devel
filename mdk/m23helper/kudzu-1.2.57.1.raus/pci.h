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

#ifndef _KUDZU_PCI_H_
#define _KUDZU_PCI_H_

#include "kudzu.h"
#include "device.h"

/* Cardbus stuff can show up as PCI. */
/* Start at 1 so we don't do weird stuff on previous installations. */
#define PCI_UNKNOWN		0
#define PCI_NORMAL		1
#define PCI_CARDBUS		2

struct pciDevice {
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
	/* pci-specific fields */
	struct pciDevice  *(*newDevice) (struct pciDevice *dev);
	void (*freeDevice) (struct pciDevice *dev);
	void (*writeDevice) (FILE *file, struct pciDevice *dev);
	int (*compareDevice) (struct pciDevice *dev1, struct pciDevice *dev2);
	unsigned int vendorId;	/* vendor id */
	unsigned int deviceId;	/* device id */
	int pciType;			/* type of PCI bus */
	unsigned int subVendorId;
	unsigned int subDeviceId;
	unsigned int pcidom;
	unsigned int pcibus;
	unsigned int pcidev;
	unsigned int pcifn;
};

struct pciDevice *pciNewDevice(struct pciDevice *dev);
struct device *pciProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);
int pciReadDrivers(char *filename);
void pciFreeDrivers(void);
#endif
