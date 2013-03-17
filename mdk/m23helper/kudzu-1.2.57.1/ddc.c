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

#include <ctype.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ddc.h"

#include "kudzuint.h"

static void ddcFreeDevice(struct ddcDevice *dev)
{
	if (dev->id) free(dev->id);
	freeDevice((struct device *) dev);
}

static void ddcWriteDevice(FILE *file, struct ddcDevice *dev)
{
	int x;
	
	writeDevice(file, (struct device *)dev);
	if (dev->id)
	  fprintf(file,"id: %s\n",
		  dev->id);
	if (dev->horizSyncMin)
	  fprintf(file,"horizSyncMin: %d\n",
		dev->horizSyncMin);
	if (dev->horizSyncMax)
	  fprintf(file,"horizSyncMax: %d\n",
		dev->horizSyncMax);
	if (dev->vertRefreshMin)
	  fprintf(file,"vertRefreshMin: %d\n",
		dev->vertRefreshMin);
	if (dev->vertRefreshMax)
	  fprintf(file,"vertRefreshMax: %d\n",
		dev->vertRefreshMax);	  
	if (dev->modes) {
		for (x=0;dev->modes[x]!=0;x+=2) {
			fprintf(file,"mode: %dx%d\n",
				dev->modes[x],dev->modes[x+1]);
		}
	}
	if (dev->mem)
	  fprintf(file,"mem: %ld\n",dev->mem);
}

static int ddcCompareDevice(struct ddcDevice *dev1, struct ddcDevice *dev2)
{
	return compareDevice((struct device *)dev1, (struct device *)dev2);
}

struct ddcDevice *ddcNewDevice(struct ddcDevice *old)
{
	struct ddcDevice *ret;

	ret = malloc(sizeof(struct ddcDevice));
	memset(ret, '\0', sizeof(struct ddcDevice));
	ret = (struct ddcDevice *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_DDC;
	ret->newDevice = ddcNewDevice;
	ret->freeDevice = ddcFreeDevice;
	ret->writeDevice = ddcWriteDevice;
	ret->compareDevice = ddcCompareDevice;
	if (old && old->bus == BUS_DDC) {
		if (old->id) 
		  ret->id = strdup(old->id);
		ret->horizSyncMin = old->horizSyncMin;
		ret->horizSyncMax = old->horizSyncMax;
		ret->vertRefreshMin = old->vertRefreshMin;
		ret->vertRefreshMax = old->vertRefreshMax;
		ret->mem = old->mem;
		if (old->modes) {
			int x;
			
			for (x=0;old->modes[x]!=0;x+=2);
			ret->modes = malloc((x+1)*sizeof(int));
			memcpy(ret->modes,old->modes,x+1);
		}
	}
	return ret;
}

int ddcReadDrivers(char *filename)
{
	return 0;
}

void ddcFreeDrivers() 
{
};


struct device *ddcProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
	if (probeFlags & PROBE_SAFE) return devlist;	
	openlog(NULL,LOG_PID,LOG_USER);
	syslog(LOG_NOTICE,"obsolete kudzu ddcProbe called");
	closelog();
	return devlist;
}
