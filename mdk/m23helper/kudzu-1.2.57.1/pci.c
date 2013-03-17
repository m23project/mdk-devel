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
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pci/pci.h>

#include "pci.h"

#include "kudzuint.h"

static struct pci_access *pacc=NULL;
static jmp_buf pcibuf;
static char *pcifiledir = NULL;
static void pciWriteDevice(FILE *file, struct pciDevice *dev);

static int devCmp2(const void * a, const void * b) {
    const struct pciDevice * one = a;
    const struct pciDevice * two = b;
    int x=0,y=0,z=0,xx=0,yy=0;
    
    x = (one->vendorId - two->vendorId);
    xx = (one->subVendorId - two->subVendorId);
    y = (one->deviceId - two->deviceId);
    yy = (one->subDeviceId - two->subDeviceId);
    if (one->pciType && two->pciType)
	  z = (one->pciType - two->pciType);
    if (x)
      return x;
    else if (y)
      return y;
    else if (one->subVendorId != 0xffff && two->subVendorId != 0xffff) {
	    if (xx)
	      return xx;
	    else if (yy)
	      return yy;
    } 
	return z;
}

static void pciFreeDevice(struct pciDevice *dev) {
    freeDevice((struct device *)dev);
}

static void pciWriteDevice(FILE *file, struct pciDevice *dev) {
	writeDevice(file, (struct device *)dev);
	fprintf(file,"vendorId: %04x\ndeviceId: %04x\nsubVendorId: %04x\nsubDeviceId: %04x\npciType: %d\npcidom: %4x\npcibus: %2x\npcidev: %2x\npcifn: %2x\n",
		dev->vendorId,dev->deviceId,dev->subVendorId,dev->subDeviceId,dev->pciType,dev->pcidom,dev->pcibus,dev->pcidev,dev->pcifn);
}

static int pciCompareDevice(struct pciDevice *dev1, struct pciDevice *dev2)
{
	int x,y;
	
	x = compareDevice((struct device *)dev1,(struct device *)dev2);
	if (x && x!=2) 
	  return x;
	if ((y=devCmp2( (void *)dev1, (void *)dev2 )))
	  return y;
	return x;
}
			    
struct pciDevice * pciNewDevice(struct pciDevice *dev) {
    struct pciDevice *ret;
    
    ret = malloc(sizeof(struct pciDevice));
    memset(ret,'\0',sizeof(struct pciDevice));
    ret=(struct pciDevice *)newDevice((struct device *)dev,(struct device *)ret);
    ret->bus = BUS_PCI;
    /* Older entries will come in with subvendor & subdevice IDs of 0x0.
     * 0x0 is a *valid* entry, though, so set it to something invalid. */
    ret->subVendorId = 0xffff;
    if (dev && dev->bus == BUS_PCI) {
	ret->vendorId = dev->vendorId;
	ret->deviceId = dev->deviceId;
	ret->pciType = dev->pciType;
	ret->subVendorId = dev->subVendorId;
        ret->subDeviceId = dev->subDeviceId;
	ret->pcidom = dev->pcidom;
	ret->pcibus = dev->pcibus;
	ret->pcidev = dev->pcidev;
	ret->pcifn = dev->pcifn;
    } else {
	ret->pciType = PCI_UNKNOWN;
    }
    ret->newDevice = pciNewDevice;
    ret->freeDevice = pciFreeDevice;
    ret->writeDevice = pciWriteDevice;
    ret->compareDevice = pciCompareDevice;
    return ret;
}

static unsigned int kudzuToPci(enum deviceClass class) {
    switch (class) {
     case CLASS_UNSPEC:
	return 0;
     case CLASS_OTHER:
	return 0;
     case CLASS_NETWORK:
	return PCI_BASE_CLASS_NETWORK;
     case CLASS_VIDEO:
	return PCI_BASE_CLASS_DISPLAY;
     case CLASS_AUDIO:
	return PCI_CLASS_MULTIMEDIA_AUDIO;
     case CLASS_SCSI:
	return PCI_CLASS_STORAGE_SCSI;
     case CLASS_FLOPPY:
	return PCI_CLASS_STORAGE_FLOPPY;
     case CLASS_RAID:
	return PCI_CLASS_STORAGE_RAID;
     case CLASS_CAPTURE:
	return PCI_CLASS_MULTIMEDIA_VIDEO;
     case CLASS_MODEM:
	return PCI_CLASS_COMMUNICATION_SERIAL;
     case CLASS_MOUSE: /* !?!? */
	return PCI_CLASS_INPUT_MOUSE;
     case CLASS_USB:
	return PCI_CLASS_SERIAL_USB;
     case CLASS_FIREWIRE:
	return PCI_CLASS_SERIAL_FIREWIRE;
     case CLASS_SOCKET:
	return PCI_CLASS_BRIDGE_CARDBUS;
     case CLASS_IDE:
	return PCI_CLASS_STORAGE_IDE;
     case CLASS_ATA:
	return PCI_CLASS_STORAGE_ATA;
     case CLASS_SATA:
	return PCI_CLASS_STORAGE_SATA;
     default:
	return 0;
    }
}

static enum deviceClass pciToKudzu(unsigned int class) {
    
    if (!class) return CLASS_UNSPEC;
    switch (class >> 8) {
     case PCI_BASE_CLASS_DISPLAY:
	return CLASS_VIDEO;
    case PCI_BASE_CLASS_NETWORK:
	return CLASS_NETWORK;
     default:
	break;
    }
    switch (class) {
     case PCI_CLASS_STORAGE_SCSI:
     case PCI_CLASS_SERIAL_FIBER:
	return CLASS_SCSI;
     case PCI_CLASS_STORAGE_FLOPPY:
	return CLASS_FLOPPY;
     case PCI_CLASS_STORAGE_RAID:
	return CLASS_RAID;
     case PCI_CLASS_STORAGE_ATA:
	return CLASS_ATA;
     case PCI_CLASS_STORAGE_SATA:
	return CLASS_SATA;
     case PCI_CLASS_MULTIMEDIA_AUDIO:
     /* HD Audio */
     case 0x0403:
	return CLASS_AUDIO;
     case PCI_CLASS_INPUT_MOUSE:
	return CLASS_MOUSE;
     case PCI_CLASS_MULTIMEDIA_OTHER:
     case PCI_CLASS_MULTIMEDIA_VIDEO:
	return CLASS_CAPTURE;
     case PCI_CLASS_COMMUNICATION_SERIAL:
     case PCI_CLASS_COMMUNICATION_OTHER:
	return CLASS_MODEM;
     case PCI_CLASS_NOT_DEFINED_VGA:
	return CLASS_VIDEO;
     /* Fix for one of the megaraid variants.
      * It claims to be an I2O controller. */
     case 0x0e00:
	return CLASS_SCSI;
     case PCI_CLASS_SERIAL_USB:
	return CLASS_USB;
     case PCI_CLASS_SERIAL_FIREWIRE:
	return CLASS_FIREWIRE;
     case PCI_CLASS_BRIDGE_CARDBUS:
	return CLASS_SOCKET;
     case PCI_CLASS_NETWORK_ETHERNET:
	return CLASS_NETWORK;
     case PCI_CLASS_NETWORK_TOKEN_RING:
	return CLASS_NETWORK;
     case PCI_CLASS_NETWORK_FDDI:
	return CLASS_NETWORK;
     case PCI_CLASS_NETWORK_ATM:
	return CLASS_NETWORK;
     case PCI_CLASS_STORAGE_IDE:
	return CLASS_IDE;
     default:
	return CLASS_OTHER;
    }
}

static void readVideoAliasesDir(char *dirname) {
	DIR *d;
	struct dirent *entry;
	
	d = opendir(dirname);
	if (!d) return;
	
	while ((entry = readdir(d))) {
		char *path;
		char *s;
		
		if (entry->d_name[0] == '.') continue;
		s = strstr(entry->d_name, ".xinf");
		if (s != (entry->d_name + strlen(entry->d_name) - 5))
			continue;
		
		asprintf(&path,"%s/%s",dirname,entry->d_name);
		aliases = readAliases(aliases, path, "pcivideo");
		free(path);
	}
	closedir(d);
}

int pciReadDrivers(char *filename) {
	char *p;
	struct stat sbuf;
	
	aliases = readAliases(aliases, filename, "pci");

	if (filename) {
		pcifiledir = dirname(strdup(filename));
		asprintf(&p,"%s/videoaliases",pcifiledir);
	        if (!stat(p,&sbuf)) {
			return 0;
		}
		if (S_ISDIR(sbuf.st_mode)) {
			readVideoAliasesDir(p);
		} else
			aliases = readAliases(aliases, p, "pcivideo");
		free(p);
		return 0;
	} else {
		int x;
		struct stat sbuf;
		char *paths[] = { "/usr/share/hwdata/videoaliases", "/etc/videoaliases",
				"/modules/videoaliases", "./videoaliases" , NULL };
		
		for (x = 0; paths[x] ; x++) {
			if (!stat(paths[x],&sbuf)) {
				p = paths[x];
				break;
			}
		}
		if (!paths[x])
			return 0;
		if (S_ISDIR(sbuf.st_mode)) {
			readVideoAliasesDir(p);
		} else 
			aliases = readAliases(aliases, p, "pcivideo");
	}
	return 0;
}

void pciFreeDrivers(void) {
}

static void pcinull(char * foo, ...)
{
}

static void pcibail(char * foo, ...)
{
    longjmp(pcibuf,1);
}

static int isDisabled(struct pci_dev *p, u_int8_t config[256]) {
	int disabled;
	int i;
#ifdef __i386__
	int limit = 6;
#else
	int limit = 1;
#endif	
	unsigned int devtype, command;
	
	devtype = p->device_class;
	if (p->irq || pciToKudzu(devtype) != CLASS_VIDEO) {
		return 0;
	}
	/* Only check video cards for now. */
	command = config[PCI_COMMAND];
	disabled = 0;
	for (i = 0; i < limit ; i++) {
		int x = PCI_BASE_ADDRESS_0 + 4 * i;
		pciaddr_t pos = p->base_addr[i];
		pciaddr_t len = (p->known_fields & PCI_FILL_SIZES) ? p->size[i] : 0;
		u32 flag = config[x+3] << 24 | config[x+2] << 16 | config[x+1] << 8 | config[x];
		if ((flag == 0xffffffff || !flag) && !pos && !len)
		  continue;
		disabled++;
		if ((flag & PCI_BASE_ADDRESS_SPACE_IO) && (command & PCI_COMMAND_IO))
		  disabled--;
		else if (command & PCI_COMMAND_MEMORY)
		  disabled--;			
	}
	return disabled;
}

struct device * pciProbe(enum deviceClass probeClass, int probeFlags, struct device *devlist) {
    struct pci_dev *p;
    /* This should be plenty. */
    int cardbus_bridges[32];
    int bridgenum = 0;
    int init_list = 0;
    char pcifile[128];
    unsigned int type = kudzuToPci(probeClass),devtype;
    
    if (
	(probeClass & CLASS_OTHER) ||
	(probeClass & CLASS_NETWORK) ||
	(probeClass & CLASS_SCSI) ||
	(probeClass & CLASS_IDE) ||
	(probeClass & CLASS_VIDEO) ||
	(probeClass & CLASS_AUDIO) ||
	(probeClass & CLASS_MODEM) ||
	(probeClass & CLASS_USB) ||
	(probeClass & CLASS_FIREWIRE) ||
	(probeClass & CLASS_SOCKET) ||
	(probeClass & CLASS_CAPTURE) ||
	(probeClass & CLASS_RAID) ||
	(probeClass & CLASS_ATA) ||
	(probeClass & CLASS_SATA)) {
	pacc = pci_alloc();
	if (!pacc) return devlist;
	if (!getAliases(aliases, "pci")) {
		pciReadDrivers(NULL);
		init_list = 1;
	}
	pacc->debug=pacc->warning=pcinull;
	pacc->error=pcibail;
	if (!access("/usr/share/hwdata/pci.ids", R_OK))
		    pacc->id_file_name = "/usr/share/hwdata/pci.ids";
	else if (!access("/etc/pci.ids", R_OK))
		    pacc->id_file_name = "/etc/pci.ids";
	else if (!access("/modules/pci.ids", R_OK))
		    pacc->id_file_name = "/modules/pci.ids";
	else if (!access("./pci.ids", R_OK))
		    pacc->id_file_name = "./pci.ids";
	else if (pcifiledir) {
		snprintf(pcifile,128,"%s/pci.ids",pcifiledir);
		if (!access(pcifile, R_OK))
			pacc->id_file_name = pcifile;
	}
		
		    
	if (!setjmp(pcibuf)) {
	    int order=0;
		
	    pci_init(pacc);
	    pci_scan_bus(pacc);
		
	    memset(cardbus_bridges,'\0',32);
	    /* enumerate cardbus bridges first */
	    for (p = pacc->devices; p; p=p->next) {
		u_int8_t config[256];
		
		memset(config,'\0',256);
	        pci_read_block(p, 0, config, 64);
		if ((config[PCI_HEADER_TYPE] & 0x7f) == PCI_HEADER_TYPE_CARDBUS) {
		    /* Cardbus bridge */
		    pci_read_block(p, 64, config+64, 64);
		    for (bridgenum=0; cardbus_bridges[bridgenum];bridgenum++);
		    cardbus_bridges[bridgenum] = config[PCI_CB_CARD_BUS];
		}
	    }
	    
	    for (p = pacc->devices; p; p=p->next) {
		char *t;
		char *drv, *x_drv;
		u_int8_t config[256];
		int bustype;
		unsigned int subvend, subdev;
		struct pciDevice *dev,*a_dev;
		
		memset(config,'\0',256);
		pci_read_block(p, 0, config, 64);
		if ((config[PCI_HEADER_TYPE] & 0x7f) == PCI_HEADER_TYPE_CARDBUS) {
		    /* Cardbus bridge */
		    pci_read_block(p, 64, config+64, 64);
		    subvend = config[PCI_CB_SUBSYSTEM_VENDOR_ID+1] << 8 | config[PCI_CB_SUBSYSTEM_VENDOR_ID];
		    subdev = config[PCI_CB_SUBSYSTEM_ID+1] << 8 | config[PCI_CB_SUBSYSTEM_ID];
		} else {
		    subvend = config[PCI_SUBSYSTEM_VENDOR_ID+1] << 8 | config[PCI_SUBSYSTEM_VENDOR_ID];
		    subdev = config[PCI_SUBSYSTEM_ID+1] << 8 | config[PCI_SUBSYSTEM_ID];
		}
		pci_fill_info(p, PCI_FILL_IDENT | PCI_FILL_CLASS | PCI_FILL_IRQ | PCI_FILL_BASES | PCI_FILL_ROM_BASE | PCI_FILL_SIZES);
		bustype = PCI_NORMAL;
	        for (bridgenum=0; cardbus_bridges[bridgenum]; bridgenum++) {
			if (p->bus == cardbus_bridges[bridgenum])
			  bustype = PCI_CARDBUS;
		}
		dev = pciNewDevice(NULL);
		dev->vendorId = p->vendor_id;
		dev->deviceId = p->device_id;
		dev->subVendorId = subvend;
		dev->subDeviceId = subdev;
		dev->pciType = bustype;
		asprintf(&t,"v%08Xd%08Xsv%08Xsd%08Xbc%02Xsc%02Xi%02x",p->vendor_id,p->device_id,
			 subvend, subdev,(u_int8_t)(p->device_class >> 8),(u_int8_t)(p->device_class),config[PCI_CLASS_PROG]);
		drv = aliasSearch(aliases, "pci", t);
		x_drv = aliasSearch(aliases, "pcivideo", t);
		free(t);
		if (drv)
			dev->driver = strdup(drv);
		devtype = p->device_class;
		if (x_drv) {
			dev->classprivate = strdup(x_drv);
		}
		/* Check for an i2o device. Note that symbios controllers
		 * also need i2o_scsi module. Dunno how to delineate that
		 * here. */
		if (devtype == 0x0e00) {
                    if ((config[PCI_CLASS_PROG] == 0 || config[PCI_CLASS_PROG] == 1)) { 
				if (dev->driver) free(dev->driver);
				dev->driver = strdup("i2o_block");
			}
		}
		if (devtype == PCI_CLASS_BRIDGE_CARDBUS) {
			if (dev->driver) free(dev->driver);
			dev->driver = strdup("yenta_socket");
		}
		if (isDisabled(p, config)) {
			if (dev->driver) free(dev->driver);
			dev->driver = NULL;
		}
	        if (dev->pciType == PCI_CARDBUS) {
			dev->detached = 1;
		}
		/* Sure, reuse a pci id for an incompatible card. */
		if (dev->vendorId == 0x10ec && dev->deviceId == 0x8139) {
			if (config[PCI_REVISION_ID] >= 0x20) {
				if (dev->driver) free(dev->driver);
				dev->driver = strdup("8139cp");
			} else {
				if (dev->driver) free(dev->driver);
				dev->driver = strdup("8139too");
			}
		}
		/* nForce4 boards show up with their ethernet controller
		 * as a bridge; hack it */
		if (dev->vendorId == 0x10de && (dev->driver != NULL) 
                    && !strcmp(dev->driver,"forcedeth")
		    && devtype == PCI_CLASS_BRIDGE_OTHER)
			    devtype = PCI_CLASS_NETWORK_ETHERNET;
		dev->pcidom = p->domain;
		dev->pcibus = p->bus;
		dev->pcidev = p->dev;
		dev->pcifn = p->func;
		while (1) {
			static int size = 128;
			
			if (dev->desc) free(dev->desc);
			dev->desc = malloc(size);
			dev->desc = pci_lookup_name(pacc, dev->desc, size,
					    PCI_LOOKUP_VENDOR | PCI_LOOKUP_DEVICE,
					    p->vendor_id, p->device_id, 0, 0);
			if (!strncmp(dev->desc,"<pci_lookup_name:",17)) {
				size *= 2;
				dev->desc = NULL;
			} else {
				break;
			}
		}
		    
		if ( (probeFlags & PROBE_ALL) || (dev->driver)) {
		    if (!type || (type<0xff && (type==devtype>>8))
			|| (type== kudzuToPci (pciToKudzu (devtype)))) {
			a_dev = pciNewDevice(dev);
			a_dev->type = pciToKudzu(devtype);
			if (x_drv) {
				a_dev->classprivate = strdup(x_drv);
				a_dev->type = CLASS_VIDEO;
			}
			if (a_dev->vendorId == 0x8086 && (a_dev->deviceId == 0x2652 ||
			    a_dev->deviceId == 0x2653)) {
				struct pciDevice *p;

				if (a_dev->driver) free(a_dev->driver);
				a_dev->driver = strdup("ata_piix");
				p = pciNewDevice(a_dev);
				if (p->driver) free(p->driver);
				p->driver = strdup("ahci");
				if (devlist)
					p->next = devlist;
				devlist = (struct device *)p;
			}
			switch (a_dev->type) {
			case CLASS_NETWORK:
				{
				char *netpath;
				
				asprintf(&netpath,"/sys/bus/pci/devices/%04x:%02x:%02x.%x",a_dev->pcidom,
					 a_dev->pcibus, a_dev->pcidev, a_dev->pcifn);
				__getSysfsDevice((struct device *)a_dev, netpath, "net:",0);
				free(netpath);
				if (a_dev->device) {
					__getNetworkAddr((struct device *)a_dev, a_dev->device);
				} else {
					if (devtype == PCI_CLASS_NETWORK_TOKEN_RING)
						a_dev->device = strdup("tr");
					else if (devtype == PCI_CLASS_NETWORK_FDDI)
						a_dev->device = strdup("fddi");
					else
						a_dev->device = strdup("eth");
				}
				break;
				}
			default:
				break;
			}
			a_dev->index = order;
			order++;
			if (devlist) {
			    a_dev->next = devlist;
			}
			devlist = (struct device *)a_dev;
		    } 
		}
		pciFreeDevice(dev);
	    }
	    pci_cleanup(pacc);
	}
    }
    if (init_list)
	  pciFreeDrivers();
    return devlist;
}

