/* Copyright 1999-2005 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "kudzu.h"
#include "alias.h"
#include "adb.h"
#include "ddc.h"
#include "firewire.h"
#include "ide.h"
#include "isapnp.h"
#include "keyboard.h"
#include "macio.h"
#include "misc.h"
#include "modules.h"
#include "parallel.h"
#include "pci.h"
#include "pcmcia.h"
#include "psaux.h"
#include "usb.h"
#include "s390.h"
#include "serial.h"
#include "sbus.h"
#include "scsi.h"
#include "vio.h"
#include "xen.h"

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#include "device.h"

#ifndef __LOADER__

static struct {
    char *prefix;
    char *match;
} fbcon_drivers[] = {
/* The first string is a prefix of fix->id reported by fbcon
   (check linux/drivers/video for that), the latter is
   the X driver used by matching cards. */
{ "ATY Mach64", "ati" },
{ "ATI Radeon", "radeon" },
{ "BWtwo", "sunbw2" },
{ "CGfourteen", "suncg14" },
{ "CGsix ", "suncg6" },
{ "CGthree", "suncg3" },
{ "CLgen", "cirrus" },
{ "Creator", "sunffb" },
{ "DEC 21030 TGA", "tga" },
{ "Elite 3D", "sunffb" },
{ "Leo", "sunleo" },
{ "MATROX", "mga" },
{ "Permedia2", "glint" },
{ "TCX", "suntcx" },
{ "VESA VGA", "fbdev" },
{ "VGA16 VGA", "fbdev" },
{ "xen", "fbdev" },
{ NULL, NULL },
};

#endif

struct kudzuclass classes[] = {
	{ CLASS_UNSPEC, "UNSPEC" }, 
	{ CLASS_OTHER, "OTHER"},
	{ CLASS_NETWORK, "NETWORK"},
	{ CLASS_SCSI, "SCSI"},
	{ CLASS_MOUSE, "MOUSE"},
	{ CLASS_AUDIO, "AUDIO"},
	{ CLASS_CDROM, "CDROM"},
	{ CLASS_MODEM, "MODEM"},
	{ CLASS_VIDEO,  "VIDEO"},
	{ CLASS_TAPE,  "TAPE"},
	{ CLASS_FLOPPY, "FLOPPY"},
	{ CLASS_SCANNER, "SCANNER"},
	{ CLASS_HD, "HD"},
	{ CLASS_RAID, "RAID"},
	{ CLASS_PRINTER, "PRINTER"},
	{ CLASS_CAPTURE, "CAPTURE"},
	{ CLASS_KEYBOARD, "KEYBOARD"},
	{ CLASS_MONITOR, "MONITOR"},
	{ CLASS_USB, "USB"},
	{ CLASS_SOCKET, "SOCKET"},
	{ CLASS_FIREWIRE, "FIREWIRE"},
	{ CLASS_IDE, "IDE"},
	{ CLASS_ATA, "ATA"},
	{ CLASS_SATA, "SATA"},
	{ 0, NULL }
};

struct bus buses[] = {
	{ BUS_UNSPEC, "UNSPEC", NULL, NULL, NULL, NULL },
	{ BUS_OTHER, "OTHER", (newFunc *)newDevice, NULL, NULL, NULL },
	{ BUS_PCI, "PCI", (newFunc *)pciNewDevice, pciReadDrivers, pciFreeDrivers, pciProbe },
#if !defined(__LOADER__) || defined(__sparc__)
	{ BUS_SBUS, "SBUS", (newFunc *)sbusNewDevice, NULL, NULL, sbusProbe },
#endif
	{ BUS_USB, "USB", (newFunc *)usbNewDevice, usbReadDrivers, usbFreeDrivers, usbProbe },
#ifndef __LOADER__
	{ BUS_PSAUX, "PSAUX", (newFunc *)psauxNewDevice, NULL, NULL, psauxProbe },
	{ BUS_SERIAL, "SERIAL", (newFunc *)serialNewDevice, NULL, NULL, serialProbe },
	{ BUS_PARALLEL, "PARALLEL", (newFunc *)parallelNewDevice, NULL, NULL, parallelProbe },
#endif /* LOADER */
	{ BUS_SCSI, "SCSI", (newFunc *)scsiNewDevice, NULL, NULL, scsiProbe },
	{ BUS_IDE, "IDE", (newFunc *)ideNewDevice, NULL, NULL, ideProbe },
#ifndef __LOADER__
	{ BUS_KEYBOARD, "KEYBOARD", (newFunc *)keyboardNewDevice, NULL, NULL, keyboardProbe },
	{ BUS_DDC, "DDC", (newFunc *)ddcNewDevice, ddcReadDrivers, ddcFreeDrivers, ddcProbe },
        { BUS_ISAPNP, "ISAPNP", (newFunc *)isapnpNewDevice, isapnpReadDrivers, isapnpFreeDrivers, isapnpProbe },
#endif /* LOADER */
	{ BUS_MISC, "MISC", (newFunc *)miscNewDevice, NULL, NULL, miscProbe },
	{ BUS_FIREWIRE, "FIREWIRE", (newFunc *)firewireNewDevice, NULL, NULL, firewireProbe },
	{ BUS_PCMCIA, "PCMCIA", (newFunc *)pcmciaNewDevice, pcmciaReadDrivers, pcmciaFreeDrivers, pcmciaProbe },
#if !defined(__LOADER__) || defined(__powerpc__)
	{ BUS_ADB, "ADB", (newFunc *)adbNewDevice, NULL, NULL, adbProbe },
	{ BUS_MACIO, "MACIO", (newFunc *)macioNewDevice, NULL, NULL, macioProbe },
	{ BUS_VIO, "VIO", (newFunc *)vioNewDevice, NULL, NULL, vioProbe },
#endif
#if !defined(__LOADER__) || defined(__s390__) || defined(__s390x__)
	{ BUS_S390, "S390", (newFunc *)s390NewDevice, NULL, NULL, s390Probe },
#endif
	{ BUS_XEN, "XEN", (newFunc *)xenNewDevice, NULL, NULL, xenProbe },
	{ 0, NULL, NULL, NULL, NULL, NULL }
};

char *module_file = NULL;
float kernel_release;
char *kernel_ver = NULL;
struct aliaslist *aliases = NULL;

static void setupKernelVersion() {
	unsigned int major, sub, minor;
	
	if (!kernel_ver) {
		struct utsname utsbuf;
		
		uname(&utsbuf);
		kernel_ver = strdup(utsbuf.release);
	}
	sscanf(kernel_ver, "%u.%u.%u", &major, &sub, &minor);
	kernel_release = major + (float)(sub/10.0);
	if (kernel_release >= 2.5)
		module_file = "/etc/modprobe.conf";
	else
		module_file = "/etc/modules.conf";
}

struct device *__newDevice(struct device *old) {
    return newDevice(old, NULL);
}

struct device *newDevice(struct device *old, struct device *new) {
    if (!old) {
	if (!new) {
	    new = malloc(sizeof(struct device));
	    memset(new,'\0',sizeof(struct device));
	}
	    new->type = CLASS_UNSPEC;
    } else {
	    new->type = old->type;
	    if (old->device) new->device = strdup(old->device);
	    if (old->driver) new->driver = strdup(old->driver);
	    if (old->desc) new->desc = strdup(old->desc);
	    new->detached = old->detached;
    }
    new->newDevice = __newDevice;
    new->freeDevice = freeDevice;
    new->compareDevice = compareDevice;
    return new;
}

void freeDevice(struct device *dev) {
    if (!dev) {
	    printf("freeDevice(null)\n");
	    abort(); /* return; */
    }
    if (dev->device) free (dev->device);
    if (dev->driver) free (dev->driver);
    if (dev->desc) free (dev->desc);
    free (dev);
}

void writeDevice(FILE *file, struct device *dev) {
	int bus, class = CLASS_UNSPEC, i;

	if (!file) {
		printf("writeDevice(null,dev)\n");
		abort();
	}
	if (!dev) {
		printf("writeDevice(file,null)\n");
		abort();
	}
	bus = 0;
	for (i = 0; buses[i].busType; i++)
	  if (dev->bus == buses[i].busType) {
		  bus = i;
		  break;
	  }
        for (i = 0; classes[i].classType; i++)
	  if (dev->type == classes[i].classType) {
		  class = i;
		  break;
	  }
	fprintf(file,"-\nclass: %s\nbus: %s\ndetached: %d\n",
		classes[class].string,buses[bus].string,dev->detached);
	if (dev->device) 
	  fprintf(file,"device: %s\n",dev->device);
	if (dev->driver)
	  fprintf(file,"driver: %s\n",dev->driver);
	fprintf(file,"desc: \"%s\"\n",dev->desc);
	if (dev->type == CLASS_NETWORK && dev->classprivate)
		fprintf(file,"network.hwaddr: %s\n", (char *)dev->classprivate);
	if (dev->type == CLASS_VIDEO && dev->classprivate)
		fprintf(file,"video.xdriver: %s\n", (char *)dev->classprivate);
}

int compareDevice(struct device *dev1, struct device *dev2) {
	if (!dev1 || !dev2) return 1;
	if (dev1->type != dev2->type) return 1;
	if (dev1->bus != dev2->bus) return 1;
	
	if (dev1->device && dev2->device && strcmp(dev1->device,dev2->device)) {
	  /* If a network device has the same hwaddr, don't worry about the
	   * dev changing */
	  if (dev1->type == CLASS_NETWORK && dev2->type == CLASS_NETWORK &&
	      dev1->classprivate && dev2->classprivate &&
	      !strcmp((char *)dev1->classprivate,(char *)dev2->classprivate))
	      return 0;
	  /* We now get actual ethernet device names. Don't flag that as a change. */
	  if (strcmp(dev1->device,"eth") && strcmp(dev1->device,"tr") && strcmp(dev1->device,"fddi") &&
	      strcmp(dev2->device,"eth") && strcmp(dev2->device,"tr") && strcmp(dev2->device,"fddi"))
	      return 1;
	}
	/* Look - a special case!
	 * If it's just the driver that changed, we might
	 * want to act differently on upgrades.
	 */
	if ((dev1->driver && dev2->driver) && strcmp(dev1->driver,dev2->driver)) return 2;
	/* If a network device changes hwaddr, flag it! */
	if (dev1->type == CLASS_NETWORK && dev2->type == CLASS_NETWORK &&
	    dev1->classprivate && dev2->classprivate &&
	    strcmp((char *)dev1->classprivate,(char *)dev2->classprivate))
		return 1;
	return 0;
}

struct device *listRemove(struct device *devlist, struct device *dev) {
	struct device *head,*ptr,*prev;

	head = ptr = devlist;
	prev = NULL;
	while (ptr != NULL) {
		if (!ptr->compareDevice(ptr,dev)) {
			if (ptr == head) {
				head = head->next;
			} else {
				prev->next = ptr->next;
			}
			return head;
		}
		prev = ptr;
		ptr = ptr->next;
	}
	return head;
}

void sortNetDevices(struct device *devs) {
	struct device *cur, *next, *tmp;
	char *modulename;
	
	cur = devs;
	while (cur && cur->type != CLASS_NETWORK) {
		cur = cur->next;
	}
	while (cur && cur->type == CLASS_NETWORK) {
		modulename = cur->driver;
		next = cur->next;
		if (!next || next->type != CLASS_NETWORK) return;
		tmp = next->next;
		while (tmp && tmp->type == CLASS_NETWORK) {
			if (tmp->driver && modulename && !strcmp(tmp->driver,modulename)) {
				next->next = tmp->next;
				tmp->next = cur->next;
				cur->next = tmp;
				cur = cur->next;
			}
			next = tmp;
			tmp = next->next;
		}
		if (cur) 
		  cur = cur->next;
	}
	return;
}

struct device *filterNetDevices(struct device *devs) {
	struct device *ret, *prev = NULL;
	
        ret = devs;
	
	while (devs) {
		if (devs->type == CLASS_NETWORK) {
			if (!isLoaded(devs->driver))  {
				struct device *tmp;
				if (prev == NULL) {
					ret = devs->next;
				} else {
					prev->next = devs->next;
				}
				tmp = devs;
				devs = devs->next;
				tmp->freeDevice(tmp);
				continue;
			}
		}
		prev = devs;
		devs = devs->next;
	}
	return ret;
}

#ifndef __LOADER__

struct device *readDevice(FILE *file) {
	char *linebuf=malloc(512);
	struct device *retdev=NULL, *tmpdev;
	int i,x=0;

	if (!file) {
		printf("readDevice(null)\n");
		abort();
	}
	memset(linebuf,'\0',512);
	while (strcmp(linebuf,"-")) {
		memset(linebuf,'\0',512);
		linebuf=fgets(linebuf,512,file);
		if (!linebuf) break;
		/* kill trailing \n */
		(*rindex(linebuf,'\n'))='\0';
		if (!strcmp(linebuf,"-")) {
			break;
		} else {
			if (!retdev) retdev = newDevice(NULL,NULL);
		}
		if (!strncmp(linebuf,"class:",6)) {
			for (i=0; 
			     classes[i].string && strcmp(classes[i].string,linebuf+7);
			     i++);
			if (classes[i].string)
			  retdev->type = classes[i].classType;
			else
			  retdev->type = CLASS_OTHER;
		} else if (!strncmp(linebuf,"bus:",4)) {
			for (i=0; 
			     buses[i].string && strcmp(buses[i].string,linebuf+5);
			     i++);
			if (buses[i].string) {
				tmpdev = (struct device *)buses[i].newFunc(retdev);
				retdev->freeDevice(retdev);
				retdev = tmpdev;
			} else
			  retdev->bus = BUS_OTHER;
		} else if (!strncmp(linebuf,"driver:",7)) {
			retdev->driver = strdup(linebuf+8);
		} else if (!strncmp(linebuf,"detached:",9)) {
			retdev->detached = atoi(linebuf+10);
		} else if (!strncmp(linebuf,"device:",7)) {
			retdev->device = strdup(linebuf+8);
		} else if (!strncmp(linebuf,"desc:",5)) {
			if (rindex(linebuf,'"')!=index(linebuf,'"')) {
				(*rindex(linebuf,'"')) = '\0';
				retdev->desc = strdup(index(linebuf,'"')+1);
			} else {
				retdev->desc = strdup(linebuf+6);
			}
		} else if (retdev->type == CLASS_NETWORK &&
			   !strncmp(linebuf,"network.hwaddr:",15)) {
			retdev->classprivate = strdup(linebuf+16);
		} else if (retdev->type == CLASS_VIDEO &&
			   !strncmp(linebuf,"video.xdriver:",14)) {
			retdev->classprivate = strdup(linebuf+15);
		}
		switch (retdev->bus) {
		 case BUS_PCI:
			if (!strncmp(linebuf,"vendorId:",9))
			  ((struct pciDevice *)retdev)->vendorId = strtoul(linebuf+10, (char **) NULL, 16);
			else if (!strncmp(linebuf,"deviceId:",9))
			  ((struct pciDevice *)retdev)->deviceId = strtoul(linebuf+10, (char **) NULL, 16);
			else if (!strncmp(linebuf,"pciType:",8))
			  ((struct pciDevice *)retdev)->pciType = strtol(linebuf+9, (char **) NULL, 10);
			else if (!strncmp(linebuf,"subVendorId:",12))
			  ((struct pciDevice *)retdev)->subVendorId = strtoul(linebuf+13, (char **) NULL, 16);
			else if (!strncmp(linebuf,"subDeviceId:",12))
			  ((struct pciDevice *)retdev)->subDeviceId = strtoul(linebuf+13, (char **) NULL, 16);
			else if (!strncmp(linebuf,"pcidom:",7))
			  ((struct pciDevice *)retdev)->pcidom = strtoul(linebuf+8, (char **) NULL, 16);
			else if (!strncmp(linebuf,"pcibus:",7))
			  ((struct pciDevice *)retdev)->pcibus = strtoul(linebuf+8, (char **) NULL, 16);
			else if (!strncmp(linebuf,"pcidev:",7))
			  ((struct pciDevice *)retdev)->pcidev = strtoul(linebuf+8, (char **) NULL, 16);
			else if (!strncmp(linebuf,"pcifn:",6))
			  ((struct pciDevice *)retdev)->pcifn = strtoul(linebuf+7, (char **) NULL, 16);
			break;
		 case BUS_PCMCIA:
			if (!strncmp(linebuf,"vendorId:",9))
			  ((struct pcmciaDevice *)retdev)->vendorId = strtoul(linebuf+10, (char **) NULL, 16);
			else if (!strncmp(linebuf,"deviceId:",9))
			  ((struct pcmciaDevice *)retdev)->deviceId = strtoul(linebuf+10, (char **) NULL, 16);
			else if (!strncmp(linebuf,"function:",9))
			  ((struct pcmciaDevice *)retdev)->function = strtol(linebuf+10, (char **) NULL, 10);
			else if (!strncmp(linebuf,"slot:",5))
			  ((struct pcmciaDevice *)retdev)->slot = strtoul(linebuf+6, (char **) NULL, 16);
			break;
		 case BUS_PARALLEL:
			if (!strncmp(linebuf,"pnpmodel:",9))
			  ((struct parallelDevice *)retdev)->pnpmodel = strdup(linebuf+10);
			if (!strncmp(linebuf,"pnpmfr:",7))
			  ((struct parallelDevice *)retdev)->pnpmfr = strdup(linebuf+8);
			if (!strncmp(linebuf,"pnpmodes:",9))
			  ((struct parallelDevice *)retdev)->pnpmodes = strdup(linebuf+10);
			if (!strncmp(linebuf,"pnpdesc:",8))
			  ((struct parallelDevice *)retdev)->pnpdesc = strdup(linebuf+9);
			break;
		 case BUS_SERIAL:
			if (!strncmp(linebuf,"pnpmodel:",9))
			  ((struct serialDevice *)retdev)->pnpmodel = strdup(linebuf+10);
			if (!strncmp(linebuf,"pnpmfr:",7))
			  ((struct serialDevice *)retdev)->pnpmfr = strdup(linebuf+8);
			if (!strncmp(linebuf,"pnpcompat:",10))
			  ((struct serialDevice *)retdev)->pnpcompat = strdup(linebuf+11);
			if (!strncmp(linebuf,"pnpdesc:",8))
			  ((struct serialDevice *)retdev)->pnpdesc = strdup(linebuf+9);
			break;
		 case BUS_SBUS:
			if (!strncmp(linebuf,"width:",6))
			  ((struct sbusDevice *)retdev)->width = atoi(linebuf+7);
			if (!strncmp(linebuf,"height:",7))
			  ((struct sbusDevice *)retdev)->height = atoi(linebuf+8);
			if (!strncmp(linebuf,"freq:",5))
			  ((struct sbusDevice *)retdev)->freq = atoi(linebuf+6);
			if (!strncmp(linebuf,"monitor:",8))
			  ((struct sbusDevice *)retdev)->monitor = atoi(linebuf+9);
			break;
		 case BUS_SCSI:
			if (!strncmp(linebuf,"host:",5))
			  ((struct scsiDevice *)retdev)->host = atoi(linebuf+6);
			if (!strncmp(linebuf,"channel:",8))
			  ((struct scsiDevice *)retdev)->channel = atoi(linebuf+9);
			if (!strncmp(linebuf,"id:",3))
			  ((struct scsiDevice *)retdev)->id = atoi(linebuf+4);
			if (!strncmp(linebuf,"lun:",3))
			  ((struct scsiDevice *)retdev)->lun = atoi(linebuf+4);
			break;
		 case BUS_IDE:
			if (!strncmp(linebuf,"physical:",9))
			  ((struct ideDevice *)retdev)->physical = strdup(linebuf+10);
			if (!strncmp(linebuf,"logical:",8))
			  ((struct ideDevice *)retdev)->logical = strdup(linebuf+9);
		        break;
		 case BUS_DDC:
			if (!strncmp(linebuf,"id:", 3))
			  ((struct ddcDevice *)retdev)->id = strdup(linebuf+4);
			if (!strncmp(linebuf,"horizSyncMin:",13))
			  ((struct ddcDevice *)retdev)->horizSyncMin = atoi(linebuf+14);
			if (!strncmp(linebuf,"horizSyncMax:",13))
			  ((struct ddcDevice *)retdev)->horizSyncMax = atoi(linebuf+14);
			if (!strncmp(linebuf,"vertRefreshMin:",15))
			  ((struct ddcDevice *)retdev)->vertRefreshMin = atoi(linebuf+16);
			if (!strncmp(linebuf,"vertRefreshMax:",15))
			  ((struct ddcDevice *)retdev)->vertRefreshMax = atoi(linebuf+16);
			if (!strncmp(linebuf,"mode:",5)) {
				struct ddcDevice *ddev = (struct ddcDevice*)retdev;
				char *tmp;

				ddev->modes = realloc(ddev->modes,(x+3)*sizeof(int));
				ddev->modes[x] = atoi(linebuf+6);
				tmp = strstr(linebuf,"x");
				ddev->modes[x+1] = atoi(tmp+1);
				ddev->modes[x+2] = 0;
				x+=2;
			}
			if (!strncmp(linebuf,"mem:",4))
			  ((struct ddcDevice *)retdev)->mem = atol(linebuf+5);
		        break;
		 case BUS_USB:
			if (!strncmp(linebuf,"usbclass:",9))
			  ((struct usbDevice *)retdev)->usbclass = atoi(linebuf+10);
			if (!strncmp(linebuf,"usbsubclass:",12))
			  ((struct usbDevice *)retdev)->usbsubclass = atoi(linebuf+13);
			if (!strncmp(linebuf,"usbprotocol:",12))
			  ((struct usbDevice *)retdev)->usbprotocol = atoi(linebuf+13);
			if (!strncmp(linebuf,"usbbus:",7))
			  ((struct usbDevice *)retdev)->usbbus = atoi(linebuf+8);
			if (!strncmp(linebuf,"usblevel:",9))
			  ((struct usbDevice *)retdev)->usblevel = atoi(linebuf+10);
			if (!strncmp(linebuf,"usbport:",8))
			  ((struct usbDevice *)retdev)->usbport = atoi(linebuf+9);
			if (!strncmp(linebuf,"usbdev:",7))
			  ((struct usbDevice *)retdev)->usbdev = atoi(linebuf+8);
			if (!strncmp(linebuf,"vendorId:",9))
			  ((struct usbDevice *)retdev)->vendorId = strtol(linebuf+10,NULL,16);
			if (!strncmp(linebuf,"deviceId:",9))
			  ((struct usbDevice *)retdev)->deviceId = strtol(linebuf+10,NULL,16);
			if (!strncmp(linebuf,"usbmfr:",7))
			  ((struct usbDevice *)retdev)->usbmfr = strdup(linebuf+8);
			if (!strncmp(linebuf,"usbprod:",8))
			  ((struct usbDevice *)retdev)->usbprod = strdup(linebuf+9);
			break;
		 case BUS_ISAPNP:
			if (!strncmp(linebuf,"deviceId:",9))
			  ((struct isapnpDevice *)retdev)->deviceId = strdup(linebuf+10);
			if (!strncmp(linebuf,"pdeviceId:",10))
			  ((struct isapnpDevice *)retdev)->pdeviceId = strdup(linebuf+11);
			if (!strncmp(linebuf,"compat:",7))
			  ((struct isapnpDevice *)retdev)->compat = strdup(linebuf+8);
			break;
		 default:
			break;
		}
	}
	return retdev;
}

#endif

int initializeBusDeviceList(enum deviceBus busSet) {
	int bus;
	
	if (!kernel_ver)
		setupKernelVersion();
	for (bus=0;buses[bus].string;bus++) {
	  if ((busSet & buses[bus].busType) && buses[bus].initFunc) {
	      buses[bus].initFunc(NULL);
	  }
	}
	return 0;
}

int initializeDeviceList(void) {
	if (!kernel_ver)
		setupKernelVersion();
	return initializeBusDeviceList(BUS_UNSPEC);
}

void freeDeviceList() {
	int bus;
	
	for (bus=0;buses[bus].string;bus++)
	  if (buses[bus].freeFunc)
	    buses[bus].freeFunc();
}

/* used to sort device lists by a) type, b) device, c) description */
static int devCmp( const void *a, const void *b )
{
        const struct device *one,*two;
	int x,y,z,zz;
	
	one=((const struct device **)a)[0];
	two=((const struct device **)b)[0];
	x=one->type - two->type;
	if (one->device && two->device)
	  y=strcmp(one->device,two->device);
	else {
		y = one->device - two->device;
	}
	z=two->index - one->index;
	zz=strcmp(one->desc,two->desc);
	if (x)
	  return x;
	else if (y)
	  return y;
	else if (z)
	  return z;
	else
	  return zz;
}

char *__bufFromFd(int fd) {
	struct stat sbuf;
	char *buf = NULL;
	unsigned long bytes = 0;
	char tmpbuf[16384];
	
	fstat(fd,&sbuf);
	if (sbuf.st_size) {
		buf = malloc(sbuf.st_size + 1);
		memset(buf,'\0',sbuf.st_size + 1);
		if (read(fd, buf, sbuf.st_size) == -1) {
			close(fd);
			free(buf);
			return NULL;
		}
		buf[sbuf.st_size] = '\0';
	} else {
		memset(tmpbuf,'\0', sizeof(tmpbuf));
		while (read(fd, tmpbuf, sizeof(tmpbuf)) > 0) {
			buf = realloc(buf, bytes + sizeof(tmpbuf));
			memcpy(buf + bytes, tmpbuf, sizeof(tmpbuf));
			bytes += sizeof(tmpbuf);
			memset(tmpbuf, '\0', sizeof(tmpbuf));
		}
	}
	close(fd);
	return buf;
}


static inline void trim(char *buffer) {
	int x = strlen(buffer) - 1;
	do {
		x--;
	} while (x > 0 && isspace(buffer[x]));
	buffer[x+1] = '\0';
}

char *__readString(char *name) {
	int fd;
	char *buf;
	
	fd = open(name,O_RDONLY);
	if (fd == -1)
		return NULL;
	buf = __bufFromFd(fd);
	if (buf)
		trim(buf);
	return buf;
}

char *__readRaw(char *name) {
	int fd;
	char *buf;
	
	fd = open(name,O_RDONLY);
	if (fd == -1)
		return NULL;
	buf = __bufFromFd(fd);
	return buf;
}

int __readHex(char *name) {
	int fd, ret;
	char *buf;
	
	fd = open(name, O_RDONLY);
	if (fd == -1)
		return 0;
	buf = __bufFromFd(fd);
	if (!buf)
		return 0;
	ret = strtoul(buf,NULL,16);
	free(buf);
	return ret;
}

int __readInt(char *name) {
	int fd, ret;
	char *buf;
	
	fd = open(name, O_RDONLY);
	if (fd == -1)
		return 0;
	buf = __bufFromFd(fd);
	if (!buf)
		return 0;
	ret = strtoul(buf,NULL,10);
	free(buf);
	return ret;
}

static void sanitize(char *name) {
	char *c;
	
	for (c = name; *c; c++)
		if (*c == '!') *c = '/';
}

int __getSysfsDevice(struct device *dev, char *path, char *type, int return_multiple) {
	DIR *dir;
	struct dirent *entry;
	int ret = 0;
	
	dir = opendir(path);
	if (!dir) return 0;
	while ((entry = readdir(dir))) {
		if (!strncmp(entry->d_name, type, strlen(type))) {
			if (!return_multiple || !ret) {
				if (dev->device) free(dev->device);
				dev->device = strdup(entry->d_name + strlen(type));
				if (!strcmp(type,"block"))
					sanitize(dev->device);
			} else {
				struct device *tmpdev = dev->newDevice(dev);
				
				if (tmpdev->device) free (tmpdev->device);
				tmpdev->device = strdup(entry->d_name + strlen(type));
				if (!strcmp(type,"block"))
					sanitize(dev->device);
				dev->next = tmpdev;
			}
			ret += 1;
		}
	}
	closedir(dir);
	return ret;
}

int __getNetworkAddr(struct device *dev, char *devname) {
	char path[PATH_MAX];
	
	if (!devname)
		return 0;
	snprintf(path, PATH_MAX, "/sys/class/net/%s/address",devname);
	dev->classprivate = (void *)__readString(path);
	if (dev->classprivate)
		return 1;
	return 0;
}

#ifndef __LOADER__
struct device ** readDevices ( char *fn ) {
	FILE *f;
	f = fopen(fn,"r");
	return readDevs(f);
}

struct device ** readDevs (FILE *f) {
	char *linebuf;
	struct device *dev, **retdevs=NULL;
	int num=0;
	int index=0,x;
	enum deviceClass cl=CLASS_UNSPEC;
	
	if (!f) return NULL;
	linebuf=calloc(512,sizeof(char));
	
	while (strcmp(linebuf,"-\n")) {
		linebuf=fgets(linebuf,512,f);
		if (!linebuf) return NULL;
	}
	while (1) {
		dev = readDevice(f);
		if (!dev) break;
		retdevs = realloc (retdevs,(num+2) * sizeof (struct device *));
		retdevs[num] = dev;
		retdevs[num+1] = NULL;
		num++;
	}
	fclose(f);
	qsort(retdevs, num, sizeof(struct device *), devCmp);
	for (x=0;retdevs[x];x++) {
		if (retdevs[x]->type!=cl) {
			index = 0;
		}
		retdevs[x]->index = index;
		cl = retdevs[x]->type;
		index++;
	}
	return retdevs;	
}

int writeDevices ( char *fn, struct device **devlist ) {
	int x;
	FILE *confFile;
	
	if (!devlist || !devlist[0]) return 1;
	confFile = fopen(fn,"w");
	if (!confFile) return 1;
	for (x=0;devlist[x];x++) {
		devlist[x]->writeDevice(confFile,devlist[x]);
	}
	fclose(confFile);
	return 0;
}

static void fbProbe( struct device *devices ) {
    FILE *procfb;
    int i, j;
    char name[4], buffer[50], *id, *end;
    struct device *d;

    procfb = fopen("/proc/fb","r");
    if (!procfb) return;
    while (fgets(buffer, 50, procfb) != NULL) {
    	i = atoi (buffer);
    	id = strchr (buffer, ' ') + 1;
	end = id + strlen(id) - 1;
	while (*end && (*end == '\n' || *end == ' '))
	  *(end--) = '\0';
	for (j = 0; fbcon_drivers[j].prefix; j++) {
	    if (!strncmp (id, fbcon_drivers[j].prefix,
			  strlen (fbcon_drivers[j].prefix)))
		break;
	}
	if (!fbcon_drivers[j].prefix)
	    continue;
	for (d = devices; d; d = d->next) {
	    if (!d->device && d->type == CLASS_VIDEO &&
	        (d->classprivate && !fnmatch (fbcon_drivers[j].match, (char *)d->classprivate, FNM_NOESCAPE))) {
		sprintf(name, "fb%d", i);
		d->device = strdup (name);
	    }
	}
    }
    fclose(procfb);
}

#endif

struct device ** probeDevices ( enum deviceClass probeClass,
			      enum deviceBus probeBus,
			      int probeFlags
			      ) {
	struct device *devices=NULL,**devlist=NULL;
	int numDevs=0, bus, x, index=0;
	enum deviceClass cl=CLASS_UNSPEC;
	int logLevel = -1;

#ifndef __LOADER__
	logLevel = getLogLevel();
	setLogLevel(1);
#endif	
	setupKernelVersion();
	
	for (bus=1;buses[bus].string;bus++) {
	    if ( (probeBus & buses[bus].busType) &&
		 !(probeBus == BUS_UNSPEC &&
		  buses[bus].busType & BUS_DDC))
		if (buses[bus].probeFunc) {
		    DEBUG("Probing %s\n",buses[bus].string);
		    devices = buses[bus].probeFunc(probeClass,
						   probeFlags, devices);
		}
	    if ((probeFlags & PROBE_ONE) && (devices))
		break;
	}
	if (devices == NULL) {
#ifndef __LOADER__
		setLogLevel(logLevel);
#endif
		return NULL;
	}
#ifndef __LOADER__		
	if (probeClass & CLASS_VIDEO)
	    fbProbe(devices);
#endif
#ifndef __LOADER__
	setLogLevel(logLevel);
#endif
	if (probeClass & CLASS_NETWORK) {
		if (probeFlags & PROBE_LOADED) {
			devices = filterNetDevices(devices);
			if (!devices)
				return NULL;
		}
	}
			
	while (devices) {
		devlist=realloc(devlist, (numDevs+2) * sizeof(struct device *));
		devlist[numDevs]=devices;
		devlist[numDevs+1]=NULL;
		numDevs++;
		devices=devices->next;
	}
	qsort(devlist, numDevs, sizeof(struct device *), devCmp);
	/* We need to sort the network devices by module name. Fun. */
	for (x=0; devlist[x] != NULL; x++) {
		devlist[x]->next = devlist[x+1];
	}
	if (probeClass & CLASS_NETWORK) {
		sortNetDevices(devlist[0]);
		if (!(probeFlags & PROBE_NOLOAD))
			matchNetDevices(devlist[0]);
	}
	devices = devlist[0];
	for (x = 0; x < numDevs ; x++) {
		devlist[x] = devices;
		devices = devices->next;
	}

	for (x=0;devlist[x];x++) {
		if (devlist[x]->type!=cl) {
			index = 0;
		}
		devlist[x]->index = index;
		cl = devlist[x]->type;
		index++;
	}
	return devlist;
}

struct netdev {
	char hwaddr[32];
	char *dev;
	char driver[32];
	struct netdev *next;
};
	       
int isCfg(const struct dirent *dent) {
	int len = strlen(dent->d_name);
	
	if (strncmp(dent->d_name,"ifcfg-",6))
		return 0;
	if (strstr(dent->d_name,"rpmnew") ||
	    strstr(dent->d_name,"rpmsave") ||
	    strstr(dent->d_name,"rpmorig"))
		return 0;
	if (dent->d_name[len-1] == '~')
		return 0;
	if (!strncmp(dent->d_name+len-5,".bak",4))
		return 0;
	return 1;
}

static inline int addToList(char ***list, char *entry, int num) {
	int x;
	char **l = *list;
	
	x = num + 1;
	l = realloc(l, (x+1) * sizeof(char *));
	l[x-1] = strdup(entry);
	l[x] = NULL;
	*list = l;
	return x;
}

static inline int inList(char **list, char *entry) {
	int x;
	
	if (!list) return 0;
	for (x = 0; list[x]; x++) {
		if (!strcmp(list[x],entry))
			return 1;
	}
	return 0;
}

static inline void removeMatchingDevices(struct device *list, struct device *current) {
	struct device *dev;
	
	for (dev = list; dev; dev = dev->next) {
		if (dev->type != CLASS_NETWORK) continue;
		if (dev == current) continue;
		if (strcmp(dev->device, current->device)) continue;
		if (!strncmp(dev->device, "eth",3)) {
			free(dev->device);
			dev->device = strdup("eth");
		} else if (!strncmp(dev->device, "tr",2)) {
			free(dev->device);
			dev->device = strdup("tr");
		} else if (!strncmp(dev->device,"fddi",4)) {
			free(dev->device);
			dev->device = strdup("fddi");
		} else {
			free(dev->device);
			dev->device = strdup("eth");
		}
	}
}

static inline int isTemporaryName(char *name) {	
	if (!strncmp(name,"__tmp",5)) {
		int x;
		char *end;
		
		x = strtoul(name+5,&end,10);
		if (!*end) {
			return 1;
		}
	}
	return 0;
}
	
void matchNetDevices(struct device *devlist) {
	struct device *dev;
	char *b = NULL;
	int ndevs = 0, x, ncfgs = 0;
	struct netdev *confdevs, *tmpdev;
	struct dirent **cfgs;
	int lasteth = 0, lasttr = 0, lastfddi = 0;
	struct confModules *cf;
	char **devicelist = NULL;
	
	confdevs = NULL;
 #ifndef __LOADER__
	if ((ncfgs = scandir("/etc/sysconfig/network-scripts",&cfgs,
			     isCfg, alphasort)) < 0) {
		return;
	}
	cf = readConfModules(module_file);
	for (x = 0; x < ncfgs ; x++ ) {
		char path[256];
		char *devname, *hwaddr, *mod = NULL;
		char *buf, *tmp;
		int fd;
		
		devname = hwaddr = NULL;
		snprintf(path,255,"/etc/sysconfig/network-scripts/%s",
			 cfgs[x]->d_name);
		if ((fd = open(path,O_RDONLY)) < 0)
			continue;
		b = buf = __bufFromFd(fd);
	        while (buf) {
			tmp = strchr(buf,'\n');
			if (tmp) {
				*tmp = '\0';
				tmp++;
			}
			if (!strncmp(buf,"DEVICE=",7)) {
				devname=buf+7;
				if (cf) {
					mod = getAlias(cf,buf+7);
				}
			}
			if (!strncmp(buf,"HWADDR=",7))
				hwaddr=buf+7;
			buf = tmp;
		}
		if (!devname || (!hwaddr && !mod)) {
			free(b);
			continue;
		}
		tmpdev = calloc(1,sizeof(struct netdev));
		tmpdev->dev = strdup(devname);
		if (hwaddr)
			strncpy(tmpdev->hwaddr,hwaddr,32);
		if (mod)
			strncpy(tmpdev->driver,mod,32);
		if (confdevs) {
			tmpdev->next = confdevs; 
		}
		confdevs = tmpdev;
		free(cfgs[x]);
		free(b);
	}
	free(cfgs);
	if (cf) freeConfModules(cf);
#endif
	/* Pass 1; fix any device names that are configured */
	for (dev = devlist; dev; dev = dev->next) {
		if (dev->type != CLASS_NETWORK) continue;
		if (!dev->classprivate)
			continue;
		for (tmpdev = confdevs; tmpdev; tmpdev = tmpdev->next) {
			if (tmpdev->hwaddr && !strcasecmp((char *)dev->classprivate,
					tmpdev->hwaddr) && strcmp(dev->device, tmpdev->dev)) {
				free(dev->device);
				dev->device = strdup(tmpdev->dev);
				ndevs = addToList(&devicelist, dev->device, ndevs);
				removeMatchingDevices(devlist, dev);
			}
		}
	}
	/* Pass 2; guess by driver */
	for (dev = devlist; dev; dev = dev->next) {
		if (dev->type != CLASS_NETWORK) continue;
		for (tmpdev = confdevs; tmpdev; tmpdev = tmpdev->next) {
			if ((tmpdev->driver && dev->driver) && !strcmp(tmpdev->driver, dev->driver)) {
				if (!inList(devicelist, tmpdev->dev)) {
					free(dev->device);
					dev->device = strdup(tmpdev->dev);
					ndevs = addToList(&devicelist, dev->device, ndevs);
					removeMatchingDevices(devlist, dev);
					break;
				}
			}
		}
	}
	/* Pass 4: everything else */
	for (dev = devlist; dev; dev = dev->next) {

		if (dev->type != CLASS_NETWORK) continue;
		if (!dev->device) continue;
		if (isTemporaryName(dev->device)) {
			free(dev->device);
			dev->device = strdup("eth");
		}
		if (!strcmp(dev->device,"eth")) {
			free(dev->device);
			dev->device = malloc(10);
			snprintf(dev->device,9,"eth%d",lasteth);
			while (1) {
				if (!inList(devicelist,dev->device))
					break;
				lasteth++;
				snprintf(dev->device,9,"eth%d",lasteth);
			}
			ndevs = addToList(&devicelist, dev->device, ndevs);
			removeMatchingDevices(devlist, dev);
			lasteth++;
			continue;
		}
		if (!strcmp(dev->device,"tr")) {
			free(dev->device);
			dev->device=malloc(10);
			snprintf(dev->device,9,"tr%d",lasttr);
			while (1) {
				if (!inList(devicelist,dev->device))
					break;
				lasttr++;
				snprintf(dev->device,9,"tr%d",lasttr);
			}
			ndevs = addToList(&devicelist, dev->device, ndevs);
			removeMatchingDevices(devlist, dev);
			lasttr++;
			continue;
		}
		if (!strcmp(dev->device,"fddi")) {
			free(dev->device);
			dev->device=malloc(10);
			snprintf(dev->device,9,"fddi%d",lastfddi);
			while (1) {
				if (!inList(devicelist,dev->device))
					break;
				lastfddi++;
				snprintf(dev->device,9,"fddi%d",lastfddi);
			}
			ndevs = addToList(&devicelist, dev->device, ndevs);
			removeMatchingDevices(devlist, dev);
			lastfddi++;
			continue;
		}
	}
	if (devicelist) {
		for (x = 0 ; devicelist[x] ; x++) {
			free(devicelist[x]);
		}
		free(devicelist);
	}
	while (confdevs) {
		tmpdev = confdevs->next;
		if (confdevs->dev)
			free(confdevs->dev);
		free(confdevs);
		confdevs = tmpdev;
	}
	return;
}

/* Stub these here; assume failure. */
#ifdef __LOADER__

struct confModules *readConfModules(char *filename)
{
    return NULL;
}

char *getAlias(struct confModules *cf, char *alias)
{
    return NULL;
}

void freeConfModules(struct confModules *cf)
{
}

static char * modNameMunge(char * mod) {
	unsigned int i;

	for (i = 0; mod[i]; i++) {
		if (mod[i] == '-')
			mod[i] = '_';
	}
	return mod;
}

int isLoaded(char *module) {
	FILE *pm;
        char * mod = NULL;
	char line[256];
	char path[256];
	
	pm = fopen("/proc/modules", "r");
	if (!pm)
		return 0;

	mod = strdup(module);
	mod = modNameMunge(mod);
	snprintf(path,255,"%s ", mod);
	while (fgets(line, sizeof(line),pm)) {
		if (!strncmp(line,path,strlen(path))) {
			free(mod);
			fclose(pm);
			return 1;
		}
	}
	free(mod);
	fclose(pm);
	return 0;
}

#endif

#ifndef __LOADER__

int listCompare( struct device **list1, struct device **list2, 
		struct device ***retlist1,
		struct device ***retlist2) {
	struct device *curr1, *prev1, *curr2, *prev2, *head1, *head2;
	struct device **ret1=NULL, **ret2=NULL;
	int x, notfound=1;
	
	/* Turn arrays into lists. */
	for (x=0;list1[x];x++) {
		list1[x]->next = list1[x+1];
	}
	for (x=0;list2[x];x++) {
		list2[x]->next = list2[x+1];
	}
	curr1 = head1 = list1[0];
	head2 = list2[0];
	prev1 = NULL;
	while (curr1) {
		curr2 = head2;
		prev2 = NULL;
		while (curr2) {
			if (!(notfound=curr1->compareDevice(curr1,curr2))) {
				if (!prev1) 
				  head1 = curr1->next;
				else
				  prev1->next = curr1->next;
				if (!prev2) 
				  head2 = curr2->next;
				else 
				  prev2->next = curr2->next;
				break;
			} else {
				prev2 = curr2;
				curr2 = curr2->next;
			}
		}
		if (notfound)
		  prev1 = curr1;
		curr1 = curr1 ->next;
	}
	/* Generate return lists */
	if (retlist1) {
		curr1 = head1;
		ret1=malloc(sizeof(struct device *));
		ret1[0]=NULL;
		for(x=0;curr1;x++) {
			ret1=realloc(ret1,(x+2)*sizeof(struct device *));
			ret1[x]=curr1;
			curr1=curr1->next;
		}
		ret1[x]=NULL;
		(*retlist1)=ret1;
	}
	if (retlist2) {
		curr2 = head2;
		ret2=malloc(sizeof(struct device *));
		ret2[0]=NULL;
		for(x=0;curr2;x++) {
			ret2=realloc(ret2,(x+2)*sizeof(struct device *));
			ret2[x]=curr2;
			curr2=curr2->next;
		}
		ret2[x]=NULL;
		(*retlist2)=ret2;
	}
	if (head1 || head2 )
	  return 1;
	else
	  return 0;
}

#endif
