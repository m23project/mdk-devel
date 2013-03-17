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

#ifndef _KUDZU_PARALLEL_H_
#define _KUDZU_PARALLEL_H_

#include "device.h"

struct parallelDevice {
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
	/* parallel-specific fields */
	struct parallelDevice *(*newDevice) (struct parallelDevice *dev);
	void (*freeDevice) (struct parallelDevice *dev);
	void (*writeDevice) (FILE *file, struct parallelDevice *dev);
	int (*compareDevice) (struct parallelDevice *dev1, struct parallelDevice *dev2);
	/* These are more printer-specific than parallel specific. */
	/* However, we can't autoprobe other printers well, so here it stays. */
	char *pnpmodel;
	char *pnpmfr;
	char *pnpmodes;
	char *pnpdesc;
};

struct parallelDevice *parallelNewDevice(struct parallelDevice *dev);
struct device *parallelProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist);
#endif
