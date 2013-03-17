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

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "sbus.h"

static void sbusFreeDevice( struct sbusDevice *dev ) {
    freeDevice( (struct device *)dev);
}

static void sbusWriteDevice( FILE *file, struct sbusDevice *dev) {
	writeDevice(file, (struct device *) dev);
	fprintf(file,"width: %d\nheight: %d\nfreq: %d\nmonitor: %d\n",
		dev->width, dev->height, dev->freq, dev->monitor);
}

static int sbusCompareDevice( struct sbusDevice *dev1, struct sbusDevice *dev2)
{
	int w,h,f,m,x;
	
	x = compareDevice( (struct device *)dev1, (struct device *)dev2);
	if (x && x!=2) return x;
	w=dev1->width-dev2->width;
	h=dev1->height-dev2->height;
	f=dev1->freq-dev2->freq;
	m=dev1->monitor-dev2->monitor;
	if ( w || h || f || m) return 1;
	return x;
}

struct sbusDevice *sbusNewDevice( struct sbusDevice *old ) {
    struct sbusDevice *ret;

    ret = malloc( sizeof(struct sbusDevice) );
    memset(ret, '\0', sizeof (struct sbusDevice));
    ret=(struct sbusDevice *)newDevice((struct device *)old,(struct device *)ret);
    ret->bus = BUS_SBUS;
    if (old && old->bus == BUS_SBUS) {
	ret->width = old->width;
	ret->height = old->height;
	ret->freq = old->freq;
	ret->monitor = old->monitor;
    }
    ret->newDevice = sbusNewDevice;
    ret->freeDevice = sbusFreeDevice;
	ret->writeDevice = sbusWriteDevice;
	ret->compareDevice = sbusCompareDevice;
    return ret;
}

#ifdef __sparc__

#include <asm/openpromio.h>

static char *promdev = "/dev/openprom";
static int promfd;
static int prom_root_node, prom_current_node;
#define MAX_PROP	128
#define MAX_VAL		(4096-128-4)
static char buf[4096];
#define DECL_OP(size) struct openpromio *op = (struct openpromio *)buf; op->oprom_size = (size)

static int prom_getsibling(int node)
{
	DECL_OP(sizeof(int));
	
	if (node == -1) return 0;
	*(int *)op->oprom_array = node;
	if (ioctl (promfd, OPROMNEXT, op) < 0)
		return 0;
	prom_current_node = *(int *)op->oprom_array;
	return *(int *)op->oprom_array;
}

static int prom_getchild(int node)
{
	DECL_OP(sizeof(int));
	
	if (!node || node == -1) return 0;
	*(int *)op->oprom_array = node;
	if (ioctl (promfd, OPROMCHILD, op) < 0)
		return 0;
	prom_current_node = *(int *)op->oprom_array;
	return *(int *)op->oprom_array;
}

static char *prom_getproperty(char *prop, int *lenp)
{
	DECL_OP(MAX_VAL);
	
	strcpy (op->oprom_array, prop);
	if (ioctl (promfd, OPROMGETPROP, op) < 0)
		return 0;
	if (lenp) *lenp = op->oprom_size;
	return op->oprom_array;
}

static int prom_getbool(char *prop)
{
	DECL_OP(0);

	*(int *)op->oprom_array = 0;
	for (;;) {
		op->oprom_size = MAX_PROP;
	        if (ioctl(promfd, OPROMNXTPROP, op) < 0)
	        	return 0;
	        if (!op->oprom_size)
	        	return 0;
	        if (!strcmp (op->oprom_array, prop))
	        	return 1;
	}
}

struct device *prom_walk(int node, int sbus, int ebus,
			 enum deviceClass probeClass, struct device *devlist)
{
	int nextnode;
	int len, nsbus = sbus, nebus = ebus;
	char *prop = prom_getproperty("device_type", &len);
	char *type = NULL, *port=NULL, *module = NULL;
    	enum deviceClass devClass;
	int depth=-1, width = 1152, height = 900;
	int freq = 0, sense = -1;
	
	if (len <= 0)
		prop = NULL;
    	while (probeClass & CLASS_NETWORK) {
		if (!prop || strcmp(prop, "network"))
	      		break;
		prop = prom_getproperty("name", &len);
		if (!prop || len <= 0) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		while ((*prop >= 'A' && *prop <= 'Z') || *prop == ',') prop++;
		if (!sbus) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		if (!strcmp(prop, "hme")) {
			type = "Sun Happy Meal Ethernet";
			module = "sunhme";
		    	devClass = CLASS_NETWORK;
		} else if (!strcmp(prop, "le")) {
			type = "Sun Lance Ethernet";
		    	devClass = CLASS_NETWORK;
		} else if (!strcmp(prop, "qe")) {
			prop = prom_getproperty("channel#", &len);
			if (prop && len == 4 && *(int *)prop)
				goto next;
			type = "Sun Quad Ethernet";
			module = "sunqe";
		    	devClass = CLASS_NETWORK;
		} else if (!strcmp(prop, "mlanai") || !strcmp(prop, "myri")) {
			type = "MyriCOM MyriNET Gigabit Ethernet";
			module = "myri_sbus";
		    	devClass = CLASS_NETWORK;
		}
	    	prop = prom_getproperty("device_type", &len);
		break;
	}
        while (probeClass & CLASS_SCSI) {
	    	int x;
	    
		x = prop ? strcmp(prop, "scsi") : 1;
		prop = prom_getproperty("name", &len);
		if (!prop || len <= 0) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		while ((*prop >= 'A' && *prop <= 'Z') || *prop == ',') prop++;
		if (!sbus) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		if (x && strcmp(prop, "soc") && strcmp(prop, "socal")) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		if (!strcmp(prop, "soc")) {
			type = "Sun SPARCStorage Array";
			module = "fc4:soc:pluto";
		    	devClass = CLASS_SCSI;
		} else if (!strcmp(prop, "socal")) {
			type = "Sun Enterprise Network Array";
			module = "fc4:socal:fcal";
		    	devClass = CLASS_SCSI;
		} else if (!strcmp(prop, "esp")) {
			type = "Sun Enhanced SCSI Processor (ESP)";
		    	devClass = CLASS_SCSI;
		} else if (!strcmp(prop, "fas")) {
			type = "Sun Swift (ESP)";
		    	devClass = CLASS_SCSI;
		} else if (!strcmp(prop, "ptisp")) {
			type = "Performance Technologies ISP";
			module = "qlogicpti";
		    	devClass = CLASS_SCSI;
		} else if (!strcmp(prop, "isp")) {
			type = "QLogic ISP";
			module = "qlogicpti";
		    	devClass = CLASS_SCSI;
		}
	    	prop = prom_getproperty("device_type", &len);
	    	break;
	}
    	while (probeClass & CLASS_AUDIO) {
		prop = prom_getproperty("name", &len);
		if (!prop || len <= 0) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		if (strchr (prop, ','))
			while ((*prop >= 'A' && *prop <= 'Z') || *prop == ',')
				if (*prop++ == ',') break;
		if (!strcmp(prop, "audio")) {
			type = "AMD7930";
			module = "amd7930";
		    	devClass = CLASS_AUDIO;
		} else if (!strcmp(prop, "CS4231")) {
			if (ebus)
				type = "CS4231 EB2 DMA (PCI)";
			else
				type = "CS4231 APC DMA (SBUS)";
			module = "cs4231";
		    	devClass = CLASS_AUDIO;
		} else if (!strcmp(prop, "DBRIe")) {
			type = "SS10/SS20 DBRI";
			module = "dbri";
		    	devClass = CLASS_AUDIO;
		}
	    	prop = prom_getproperty("device_type", &len);
	    	break;
	}
        while (probeClass & CLASS_VIDEO) {
		if (!prop || strcmp(prop, "display")) {
		    break;
		}
		prop = prom_getproperty("name", &len);
		if (!prop || len <= 0) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		while ((*prop >= 'A' && *prop <= 'Z') || *prop == ',') prop++;
		if (!sbus && strcmp(prop, "ffb") && strcmp(prop, "afb") &&
		    strcmp(prop, "cgfourteen")) {
		    prop = prom_getproperty("device_type", &len);
		    break;
		}
		if (!strcmp(prop, "bwtwo")) {
			type = "Sun|Monochrome (bwtwo)";
			depth = 1;
		} else if (!strcmp(prop, "cgthree")) {
			type = "Sun|Color3 (cgthree)";
			depth = 8;
		} else if (!strcmp(prop, "cgeight")) {
			type = "Sun|CG8/RasterOps";
			depth = 8;
		} else if (!strcmp(prop, "cgtwelve")) {
			type = "Sun|GS (cgtwelve)";
			depth = 24;
		} else if (!strcmp(prop, "gt")) {
			type = "Sun|Graphics Tower";
			depth = 24;
		} else if (!strcmp(prop, "mgx")) {
			prop = prom_getproperty("fb_size", &len);
			if (prop && len == 4 && *(int *)prop == 0x400000)
				type = "Quantum 3D MGXplus with 4M VRAM";
			else
				type = "Quantum 3D MGXplus";
			depth = 24;
		} else if (!strcmp(prop, "cgsix")) {
			int chiprev = 0;
			int vmsize = 0;
			prop = prom_getproperty("chiprev", &len);
			if (prop && len == 4)
				chiprev = *(int *)prop;
			prop = prom_getproperty("montype", &len);
			if (prop && len == 4)
				sense = *(int *)prop;
			prop = prom_getproperty("vfreq", &len);
			if (prop && len == 4)
				freq = *(int *)prop;
			prop = prom_getproperty("vmsize", &len);
			if (prop && len == 4)
				vmsize = *(int *)prop;
			switch (chiprev) {
			case 0: type = "Sun|Unknown GX"; break;
			case 1 ... 4: type = "Sun|Double width GX"; break;
			case 5 ... 9: type = "Sun|Single width GX"; break;
			case 11: 
				switch (vmsize) {
				case 2: type = "Sun|Turbo GX with 1M VSIMM"; break;
				case 4: type = "Sun|Turbo GX Plus"; break;
				default: type = "Sun|Turbo GX"; break;
				}
			}
			depth = 8;
		} else if (!strcmp(prop, "cgfourteen")) {
			int size = 0;
			prop = prom_getproperty("vfreq", &len);
			if (prop && len == 4)
				freq = *(int *)prop;
			prop = prom_getproperty("reg", &len);
			if (prop && !(len % 12) && len > 0)
				size = *(int *)(prop + len - 4);
			switch (size) {
			case 0x400000: type = "Sun|SX with 4M VSIMM"; break;
			case 0x800000: type = "Sun|SX with 8M VSIMM"; break;
			default: type = "Sun|SX";
			}
			depth = 24;
		} else if (!strcmp(prop, "leo")) {
			prop = prom_getproperty("model", &len);
			if (prop && len > 0 && !strstr(prop, "501-2503"))
				type = "Sun|Turbo ZX";
			else
				type = "Sun|ZX or Turbo ZX";
			depth = 24;
		} else if (!strcmp(prop, "tcx")) {
			if (prom_getbool("tcx-8-bit")) {
				type = "Sun|TCX (8bit)";
				depth = 8;
			} else {
				type = "Sun|TCX (S24)";
				depth = 24;
			}
		} else if (!strcmp(prop, "afb")) {
			int btype = 0;
			prop = prom_getproperty("vfreq", &len);
			if (prop && len == 4)
				freq = *(int *)prop;
			prop = prom_getproperty("board_type", &len);
			if (prop && len == 4)
				btype = *(int *)prop;
			if (btype == 3)
				type = "Sun|Elite3D-M6 Horizontal";
			else
				type = "Sun|Elite3D";
			depth = 24;
		} else if (!strcmp(prop, "ffb")) {
			int btype = 0;
			prop = prom_getproperty("vfreq", &len);
			if (prop && len == 4)
				freq = *(int *)prop;
			prop = prom_getproperty("board_type", &len);
			if (prop && len == 4)
				btype = *(int *)prop;
			switch (btype) {
			case 0x08: type="Sun|FFB 67MHz Creator"; break;
			case 0x0b: type="Sun|FFB 67MHz Creator 3D"; break;
			case 0x1b: type="Sun|FFB 75MHz Creator 3D"; break;
			case 0x20:
			case 0x28: type="Sun|FFB2 Vertical Creator"; break;
			case 0x23:
			case 0x2b: type="Sun|FFB2 Vertical Creator 3D"; break;
			case 0x30: type="Sun|FFB2+ Vertical Creator"; break;
			case 0x33: type="Sun|FFB2+ Vertical Creator 3D"; break;
			case 0x40:
			case 0x48: type="Sun|FFB2 Horizontal Creator"; break;
			case 0x43:
			case 0x4b: type="Sun|FFB2 Horizontal Creator 3D"; break;
			default: type = "Sun|FFB"; break;
			}
			depth = 24;
		}
		if (type) {
			prop = prom_getproperty("width", &len);
			if (prop && len == 4)
				width = *(int *)prop;
			prop = prom_getproperty("height", &len);
			if (prop && len == 4)
				height = *(int *)prop;
		}
	    	prop = prom_getproperty("device_type", &len);
	    	break;
	}
	if (type) {
	    struct sbusDevice *newDev;
	    
	    newDev=sbusNewDevice(NULL);
	    if (depth != -1) {  /* it's a video device */
		newDev->width = width;
		newDev->height = height;
		newDev->freq = freq;
		newDev->monitor = sense;
	    }
	    newDev->desc = strdup(type);
	    switch (depth) {
	     case 1: 
		newDev->driver = strdup("Server:SunMono"); 
		newDev->type = CLASS_VIDEO;
		break;
	     case 8: 
		newDev->driver = strdup("Server:Sun");
		newDev->type = CLASS_VIDEO;
		break;
	     case 24: 
		newDev->driver = strdup("Server:Sun24");
		newDev->type = CLASS_VIDEO;
		break;
	     default:
		if (module)
		    newDev->driver = strdup(module);
		newDev->type = devClass;
	    }
	    if (newDev->type == CLASS_NETWORK)
		  newDev->device = strdup("eth");
	    if (port) newDev->device = strdup(port);
	    if (devlist) newDev->next = devlist;
	    devlist = (struct device *)newDev;
	}
next:
	prop = prom_getproperty("name", &len);
	if (prop && len > 0) {
		if (!strcmp(prop, "sbus") || !strcmp(prop, "sbi"))
			nsbus = 1;
		if (!strcmp(prop, "ebus"))
			nebus = 1;
	}
	nextnode = prom_getchild(node);
	if (nextnode)
		devlist=prom_walk(nextnode, nsbus, nebus, probeClass, devlist);
	nextnode = prom_getsibling(node);
	if (nextnode)
		devlist=prom_walk(nextnode, sbus, ebus, probeClass, devlist);
    	return devlist;
}
    
/* given a class to probe, returns an array of devices found which match */
/* all entries are malloc'd, so caller must free when done. Use          */
/* dev->freeDevice(dev) to free individual sbusDevice's, since they 	 */
/* have internal elements which are malloc'd                             */

struct device *sbusProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist) {
    if (probeClass & CLASS_MOUSE) {
	int fd;
	struct sbusDevice *mousedev;

	if ((fd = open("/dev/sunmouse", O_RDONLY)) != -1) {
	    /* FIXME: Should probably talk to the mouse to see
	       if the connector is not empty. */
	    close (fd);
	    mousedev = sbusNewDevice(NULL);
	    mousedev->type = CLASS_MOUSE;
	    mousedev->device = strdup("sunmouse");
	    mousedev->desc = strdup("Sun Mouse");
	    mousedev->next = devlist;
	    devlist = (struct device *)mousedev;
	}
    }
    if (
	(probeClass & CLASS_OTHER) ||
	(probeClass & CLASS_NETWORK) ||
	(probeClass & CLASS_SCSI) ||
	(probeClass & CLASS_VIDEO) ||
	(probeClass & CLASS_AUDIO) ||
	(probeClass & CLASS_MODEM) ||
	(probeClass & CLASS_RAID)
	) {
	promfd = open(promdev, O_RDONLY);
	if (promfd == -1)
	  return devlist;
	prom_root_node = prom_getsibling(0);
	if (!prom_root_node)
	  return devlist;

	devlist = prom_walk(prom_root_node, 0, 0, probeClass, devlist);
	close(promfd);
    }
    return devlist;
}

#else

struct device *sbusProbe( enum deviceClass probeClass, int probeFlags,
		    struct device *devlist ) {
    return devlist;
}

#endif
