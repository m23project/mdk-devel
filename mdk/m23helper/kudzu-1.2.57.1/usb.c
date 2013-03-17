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

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#include "usb.h"
#include "modules.h"

#include "kudzuint.h"

static void usbFreeDevice(struct usbDevice *dev)
{
	if (dev->usbmfr)
		free(dev->usbmfr);
	if (dev->usbprod)
		free(dev->usbprod);
	freeDevice((struct device *) dev);
}

static void usbWriteDevice(FILE * file, struct usbDevice *dev)
{
	writeDevice(file, (struct device *) dev);
	fprintf(file, "usbclass: %d\nusbsubclass: %d\nusbprotocol: %d\n",
		dev->usbclass, dev->usbsubclass, dev->usbprotocol);
	fprintf(file, "usbbus: %d\nusblevel: %d\nusbport: %d\nusbdev: %d\n",
		dev->usbbus, dev->usblevel, dev->usbport, dev->usbdev);
	fprintf(file, "vendorId: %04x\ndeviceId: %04x\n",
		dev->vendorId, dev->deviceId);
	if (dev->usbmfr)
		fprintf(file, "usbmfr: %s\n",dev->usbmfr);
	if (dev->usbprod)
		fprintf(file, "usbprod: %s\n",dev->usbprod);
}

static int usbCompareDevice(struct usbDevice *dev1, struct usbDevice *dev2)
{
	if (!dev1 || !dev2)
		return 1;
	return compareDevice((struct device *) dev1, (struct device *) dev2);
}


struct usbDevice *usbNewDevice(struct usbDevice *old)
{
	struct usbDevice *ret;

	ret = malloc(sizeof(struct usbDevice));
	memset(ret, '\0', sizeof(struct usbDevice));
	ret =
	    (struct usbDevice *) newDevice((struct device *) old,
					   (struct device *) ret);
	ret->bus = BUS_USB;
	ret->newDevice = usbNewDevice;
	ret->freeDevice = usbFreeDevice;
	ret->writeDevice = usbWriteDevice;
	ret->compareDevice = usbCompareDevice;
	if (old && old->bus == BUS_USB) {
		ret->usbclass = old->usbclass;
		ret->usbsubclass = old->usbsubclass;
		ret->usbprotocol = old->usbprotocol;
		ret->usbbus = old->usbbus;
		ret->usblevel = old->usblevel;
		ret->usbport = old->usbport;
		ret->usbdev = old->usbdev;
		ret->vendorId = old->vendorId;
		ret->deviceId = old->deviceId;
		if (old->usbmfr)
			ret->usbmfr = strdup(old->usbmfr);
		if (old->usbprod)
			ret->usbprod = strdup(old->usbprod);
	}
	return ret;
}

void usbFreeDrivers() {
}

int usbReadDrivers(char *filename)
{
	aliases = readAliases(aliases, filename, "usb");
	return 0;
}

static enum deviceClass usbToKudzu(int usbclass, int usbsubclass, int usbprotocol)
{
	switch (usbclass) {
	case 1:
		if (usbsubclass == 02)
			return CLASS_AUDIO;
		else
			return CLASS_OTHER;
	case 2:
		switch (usbsubclass) {
		case 2:
			return CLASS_MODEM;
		case 6:
		case 7:
			return CLASS_NETWORK;
		default:
			return CLASS_OTHER;
		}
	case 3:
		switch (usbprotocol) {
		case 1:
			return CLASS_KEYBOARD;
		case 2:
			return CLASS_MOUSE;
		default:
			return CLASS_OTHER;
		}

	case 7:
		return CLASS_PRINTER;
	case 8:
		switch (usbsubclass) {
		case 2:
			return CLASS_CDROM;
		case 3:
			return CLASS_TAPE;
		case 4:
			return CLASS_FLOPPY;
		case 5:
			/* Could be a M/O drive. Could be a floppy drive. */
			return CLASS_OTHER;
		case 6:
			return CLASS_HD;
		}
	default:
		return CLASS_OTHER;
	}
}

struct usbDevice *getUsbDevice(char *name, struct usbDevice *ret, enum deviceClass probeClass, int level) {
	struct usbDevice *tmp;
	struct dirent *entry;
	char *tname;
	DIR *d;
	int cwd;
	
	d = opendir(name);
	if (!d)
		return ret;
	cwd = open(".", O_RDONLY);
	chdir(name);
	tmp = usbNewDevice(NULL);
	tname = name;
	if (!strncmp(name,"usb",3)) {
		tname = alloca(strlen(name));
		sprintf(tname,"%d-0",atoi(name+3));
	} else {
		level++;
	}
	while ((entry = readdir(d))) {

		if (entry->d_name[0] == '.')
			continue;
		if (!strcmp(entry->d_name,"idProduct"))
			tmp->deviceId = __readHex(entry->d_name);
		if (!strcmp(entry->d_name,"idVendor"))
			tmp->vendorId = __readHex(entry->d_name);
		if (!strcmp(entry->d_name,"manufacturer"))
			tmp->usbmfr = __readString(entry->d_name);
		if (!strcmp(entry->d_name,"product"))
			tmp->usbprod = __readString(entry->d_name);
		if (!strcmp(entry->d_name,"devnum"))
			tmp->usbdev = __readInt(entry->d_name);
	}
	rewinddir(d);
	while ((entry = readdir(d))) {
		int ind = strchr(tname,'-')-tname;
		if (!strncmp(entry->d_name,tname,strlen(tname)) &&
		    entry->d_name[strlen(tname)] == ':') {
			struct usbDevice *t;
			DIR *interface;
			struct dirent *ent;
			int wd;
			int alt;
			int x;
			t = usbNewDevice(tmp);
			
			interface = opendir(entry->d_name);
			if (!interface)
				break;
			
			wd = open(".",O_RDONLY);
			chdir(entry->d_name);
			while ((ent = readdir(interface))) {
				
				if (!strcmp(ent->d_name,"bAlternateSetting"))
					alt = __readHex(ent->d_name);
				if (!strcmp(ent->d_name,"bInterfaceClass"))
					t->usbclass = __readHex(ent->d_name);
				if (!strcmp(ent->d_name,"bInterfaceSubClass"))
					t->usbsubclass = __readHex(ent->d_name);
				if (!strcmp(ent->d_name,"bInterfaceProtocol"))
					t->usbprotocol = __readHex(ent->d_name);
				if (!strcmp(ent->d_name,"modalias")) {
					char *modalias;
					
					modalias = __readString(ent->d_name);
					if (modalias) {
						char *alias = aliasSearch(aliases,"usb",modalias+4);
						
						if (alias) {
							if (t->driver)
								free(t->driver);
							t->driver = strdup(alias);
						}
						free(modalias);
					}
				}
				__getSysfsDevice((struct device *)t,".","net:",0);
				__getNetworkAddr((struct device *)t,t->device);
			}
			closedir(interface);
			fchdir(wd);
			close(wd);
			
			t->type = usbToKudzu(t->usbclass, t->usbsubclass, t->usbprotocol);
			t->usbbus = strtol(tname,NULL,10);
			t->usblevel = level;
			x = strlen(tname);
			while (!isdigit(tname[x])) x--;
			t->usbport = strtol(tname+x,NULL,10);
			if (level != 0)
				t->usbport--;
			
			if (!t->driver) {
				switch (t->type) {
				case CLASS_MOUSE:
					t->driver = strdup("genericwheelusb");
					t->device = strdup("input/mice");
					break;
				case CLASS_KEYBOARD:
					t->driver = strdup("keybdev");
					break;
				default:
					break;
				}
			}
			if (t->usbmfr && t->usbprod)
				asprintf(&t->desc,"%s %s",t->usbmfr, t->usbprod);
			else if (t->usbprod)
				t->desc = strdup(t->usbprod);
			else
				asprintf(&t->desc,"Unknown USB device 0x%x:0x%x",t->vendorId,t->deviceId);
			if (t->driver && (!strcmp(t->driver, "pegasus") ||
			    !strcmp(t->driver, "catc") ||
			    !strcmp(t->driver, "kaweth") ||
			    !strcmp(t->driver, "rtl8150") ||
			    !strcmp(t->driver, "ax8817x") ||
			    !strcmp(t->driver, "zd1201") ||
			    !strcmp(t->driver, "asix") ||
			    !strcmp(t->driver, "usbnet"))) {
				if (t->type == CLASS_OTHER)
					t->type = CLASS_NETWORK;
			}
			if (strcasestr(t->desc,"Wacom") &&
			    t->type == CLASS_MOUSE) {
				free(t->driver);
				t->driver = strdup("wacom");
			}
			if (t->type == CLASS_NETWORK && !t->device) {
				t->device = strdup("eth");
			}
		
			if ((probeClass & t->type) && !alt) {
				if (ret)
					t->next = (struct device *)ret;
				ret = t;
			} else {
				t->freeDevice(t);
			}
		} else if (!strncmp(entry->d_name,tname,ind) && !strchr(entry->d_name,':')) {
			ret = getUsbDevice(entry->d_name,ret,probeClass,level);
		}
	}
	closedir(d);
	fchdir(cwd);
	close(cwd);
	return ret;
}

struct device *usbProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
	int init_list = 0;
	int cwd;

	if (
	    (probeClass & CLASS_OTHER) ||
	    (probeClass & CLASS_CDROM) ||
	    (probeClass & CLASS_HD) ||
	    (probeClass & CLASS_TAPE) ||
	    (probeClass & CLASS_FLOPPY) ||
	    (probeClass & CLASS_KEYBOARD) ||
	    (probeClass & CLASS_MOUSE) ||
	    (probeClass & CLASS_AUDIO) ||
	    (probeClass & CLASS_MODEM) ||
	    (probeClass & CLASS_NETWORK)
	    ) {
		DIR *dir;
		struct dirent *entry;
		
		if (!getAliases(aliases,"usb")) {
			usbReadDrivers(NULL);
			init_list = 1;
		}
		
		dir = opendir("/sys/bus/usb/devices");
		if (!dir)
			goto out;
		cwd = open(".",O_RDONLY);
		chdir("/sys/bus/usb/devices");
		while ((entry = readdir(dir))) {
			if (!strncmp(entry->d_name,"usb",3) && isdigit(entry->d_name[3]))
				devlist = (struct device *)getUsbDevice(entry->d_name, (struct usbDevice *)devlist, probeClass,0);
		}
		closedir(dir);
		fchdir(cwd);
		close(cwd);
	}
out:
	if (init_list) {
		usbFreeDrivers();
	}
	return devlist;
}
