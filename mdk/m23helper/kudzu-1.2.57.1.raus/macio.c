/* macio.c: Probe for i2o audio and airport on Apple machines.
 * 
 * Copyright (C) 2002 Dan Burcaw <dburcaw@terrasoftsolutions.com>
 *           (C) 2002 Terra Soft Solutions, Inc.
 *           (C) 2003 Red Hat, Inc.
 *
 * Based on sbus.c
 * Copyright (C) 1998, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 *           (C) 1999, 2000 Red Hat, Inc.
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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "macio.h"

static void macioFreeDevice( struct macioDevice *dev ) {
    freeDevice( (struct device *)dev);
}

static void macioWriteDevice( FILE *file, struct macioDevice *dev) {
    writeDevice(file, (struct device *) dev);
}

static int macioCompareDevice(struct macioDevice *dev1, struct macioDevice *dev2)
{
	        return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct macioDevice *macioNewDevice( struct macioDevice *old ) {
    struct macioDevice *ret;

    ret = malloc( sizeof(struct macioDevice) );
    memset(ret, '\0', sizeof (struct macioDevice));
    ret=(struct macioDevice *)newDevice((struct device *)old,(struct device *)ret);
    ret->bus = BUS_MACIO;
    ret->newDevice = macioNewDevice;
    ret->freeDevice = macioFreeDevice;
    ret->writeDevice = macioWriteDevice;
    ret->compareDevice = macioCompareDevice;
    return ret;
}

#ifdef __powerpc__
#include "minifind.h"

/* given a class to probe, returns an array macio devices found which match */
/* all entries are malloc'd, so caller must free when done. Use          */
/* dev->freeDevice(dev) to free individual macioDevice's, since they 	 */
/* have internal elements which are malloc'd                             */

struct device *macioProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist) {
    // check for airport
    if (probeClass & CLASS_NETWORK) {
	struct macioDevice *airport;
	struct macioDevice *bmac;
	struct pathNode *n;
	int ret = 0;

	struct findNode *list = (struct findNode *) malloc(sizeof(struct findNode));
        list->result = (struct pathNode *) malloc(sizeof(struct pathNode));
	list->result->path = NULL;
	list->result->next = list->result;

	minifind("/proc/device-tree", "radio", list);

	// Supported
	for (n = list->result->next; n != list->result; n = n->next)
	{
		if (n->path)
			ret = 1;
	}

	if (ret)
	{
		airport = macioNewDevice(NULL);
		airport->type = CLASS_NETWORK;
		airport->device = strdup("eth");
		airport->desc = strdup("Apple Computer Inc.|Airport");
		airport->driver = strdup("airport");
		airport->next = devlist;
		devlist = (struct device *)airport;
	}

	ret = 0;
	minifind("/proc/device-tree", "ethernet", list);

	// Supported
	for (n = list->result->next; n != list->result; n = n->next)
	{
		int fd;
		char buf[256];

		if (!n->path)
			continue;

		buf[255] = 0;
		snprintf(buf, 255, "%s/compatible", n->path);

		fd = open(buf, O_RDONLY);
		if (fd < 0)
			continue;

		if (read (fd, buf, 256) < 0) {
			close(fd);
			continue;
		}
		/* Check for bmac or bmac+ */
		if (!strncmp(buf, "bmac", 4))
			ret = 1;

		close(fd);
	}

	if (ret)
	{
		bmac = macioNewDevice(NULL);
		bmac->type = CLASS_NETWORK;
		bmac->device = strdup("eth");
		bmac->desc = strdup("Apple Computer Inc.|BMAC/BMAC+");
		bmac->driver = strdup("bmac");
		bmac->next = devlist;
		devlist = (struct device *)bmac;
	}

    }
    if (probeClass & CLASS_AUDIO) {
	struct macioDevice *dmasound;

	struct pathNode *n;
	int ret = 0;

	struct findNode *list = (struct findNode *) malloc(sizeof(struct findNode));
        list->result = (struct pathNode *) malloc(sizeof(struct pathNode));
	list->result->path = NULL;
	list->result->next = list->result;

	minifind("/proc/device-tree", "sound", list);

	// Supported
	for (n = list->result->next; n != list->result; n = n->next)
	{
		if (n->path && strstr(n->path, "mac-io"))
			ret = 1;
	}

	if (ret)
	{
		dmasound = macioNewDevice(NULL);
		dmasound->type = CLASS_AUDIO;
		dmasound->desc = strdup("Apple Computer Inc.|PowerMac Sound");
		dmasound->driver = strdup("snd-powermac");
		dmasound->next = devlist;
		devlist = (struct device *)dmasound;
	}

    }
    if (probeClass & CLASS_OTHER) {
	    struct macioDevice *i2c;
	    struct pathNode *n;
	    int ret = 0;
	    int fd;
	    char buf[256];
	    
	    struct findNode *list = (struct findNode *) malloc(sizeof(struct findNode));
	    list->result = (struct pathNode *) malloc(sizeof(struct pathNode));
	    list->result->path = NULL;
	    list->result->next = list->result;
	    
	    minifind("/proc/device-tree", "i2c", list);
	    
	    // Supported
	    for (n = list->result->next; n != list->result; n = n->next)
	    {
		    if (!n->path)
			    continue;

		    buf[255] = 0;
		    snprintf(buf, 255, "%s/compatible", n->path);

		    fd = open(buf, O_RDONLY);
		    if (fd < 0)
			    continue;

		    if (read (fd, buf, 256) < 0) {
			    close(fd);
			    continue;
		    }
		    /* Check for KeyWest or Hyrda */
		    if (!strcmp(buf, "keywest-i2c"))
			    ret = 1;
		    if (!strcmp(buf, "hydra-i2c"))
			    ret = 2;
		    close(fd);

	    }
	    if (ret)
	    {
		    i2c = macioNewDevice(NULL);
		    i2c->type = CLASS_OTHER;
		    
		    if (ret == 1) {
			    i2c->desc = strdup("Apple Computer Inc.|KeyWest I2C");
			    i2c->driver = strdup("i2c-keywest");
		    } else {
			    i2c->desc = strdup("Apple Computer Inc.|Hydra I2C");
			    i2c->driver = strdup("i2c-hydra");
		    }
		    i2c->next = devlist;
		    devlist = (struct device *)i2c;
	    }
	    if (list->result) free(list->result);
	    list->result = (struct pathNode *) malloc(sizeof(struct pathNode));
	    list->result->path = NULL;
	    list->result->next = list->result;
	    
	    ret = 0;
	    minifind("/proc/device-tree", "fan", list);
	    // Supported
	    for (n = list->result->next; n != list->result; n = n->next)
	    {
		    if (!n->path)
			    continue;

		    buf[255] = 0;
		    snprintf(buf, 255, "%s/compatible", n->path);

		    fd = open(buf, O_RDONLY);
		    if (fd < 0)
			    continue;

		    if (read (fd, buf, 256) < 0) {
			    close(fd);
			    continue;
		    }
		    if (!strncmp(buf, "adt746",6))
			    ret = 1;
		    close(fd);
	    }
	    if (ret)
	    {
		    i2c = macioNewDevice(NULL);
		    i2c->type = CLASS_OTHER;
		    i2c->desc = strdup("Apple Computer Inc.|ADT 746x Thermostat");
		    i2c->driver = strdup("therm_adt746x");
		    i2c->next = devlist;
		    devlist = (struct device *)i2c;
	    }
	    buf[255] = 0;
	    snprintf(buf,255, "/proc/device-tree/compatible");
	    fd = open(buf, O_RDONLY);
	    if (fd >= 0) {
		    if (read (fd, buf, 256) >= 0) {
			    if (!strncmp(buf, "PowerMac7,2", 11) ||
				!strncmp(buf, "PowerMac7,3", 11) ||
				!strncmp(buf, "RackMac3,1", 10)) {
				    i2c = macioNewDevice(NULL);
				    i2c->type = CLASS_OTHER;
				    i2c->desc = strdup("Apple Computer Inc.|G5 Thermostat");
				    i2c->driver = strdup("therm_pm72");
				    i2c->next = devlist;
				    devlist = (struct device *)i2c;
			    }
		    }
		    close(fd);
	    }
    }
    return devlist;
}

#else

struct device *macioProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist ) {
    return devlist;
}

#endif
