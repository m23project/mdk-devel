/* Copyright 1999-2004 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Portions of this code from pcmcia-cs, copyright David A. Hinds
 * <dahinds@users.sourceforge.net>
 * 
 */

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fd.h>

#include "misc.h"
#include "kudzuint.h"

static void miscFreeDevice(struct miscDevice *dev)
{
	freeDevice((struct device *) dev);
}

static void miscWriteDevice(FILE *file, struct miscDevice *dev)
{
	writeDevice(file, (struct device *)dev);
}

static int miscCompareDevice(struct miscDevice *dev1, struct miscDevice *dev2)
{
	return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct miscDevice *miscNewDevice(struct miscDevice *old)
{
	struct miscDevice *ret;

	ret = malloc(sizeof(struct miscDevice));
	memset(ret, '\0', sizeof(struct miscDevice));
	ret = (struct miscDevice *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_MISC;
	ret->newDevice = miscNewDevice;
	ret->freeDevice = miscFreeDevice;
	ret->writeDevice = miscWriteDevice;
	ret->compareDevice = miscCompareDevice;
	return ret;
}

#if defined(__i386__) || defined(__alpha__)

#include <sys/io.h>

unsigned short i365_base = 0x03e0;
unsigned short tcic_base = 0x240;
#define I365_IDENT		0x00
#define TCIC_SCTRL		0x06
#define TCIC_SCTRL_RESET	0x80
#define TCIC_ADDR		0x02
#define I365_REG(slot, reg)    (((slot) << 6) | (reg))

static unsigned char i365_get(unsigned short sock, unsigned short reg)
{
    unsigned char val = I365_REG(sock, reg);
	printf("vorher outb: %i\n", val);
	fflush(stdout);

	if (val == 0)
		return(0);
    outb(val, i365_base); val = inb(i365_base+1);
    return val;
}

#endif

static char *getFloppyDesc(char *name) {
	int size;
	char *type;
	char desc[64];
	
	size = atoi(name+1);
	
	if (isupper(name[0]))
	  type = "3.5\"";
	else
	  type = "5.25\"";
	if (size > 1000)
	  snprintf(desc, 63, "%s %d.%dMB floppy drive", type, size / 1000, (size % 1000) / 10);
	else 
	  snprintf(desc, 64, "%s %dKB floppy drive", type, size);
	return strdup(desc);
}

struct device *miscProbe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
	int fd, x, rc;
	char path[32], name[32];
	struct miscDevice *miscdev;
	struct floppy_drive_struct ds;

        if (probeClass & CLASS_HD) {
            char *path, *ptr;
            FILE * f;
            char buf[256];
            int ctlNum = 0;
            char ctl[64];

            /* cciss */
            path = "/proc/driver/cciss";
            sprintf(ctl, "%s/cciss%d", path, ctlNum);
            while ((f = fopen(ctl, "r"))) {
                while (fgets(buf, sizeof(buf) - 1, f)) {
                    if (!strncmp(buf, "cciss/", 6)) {
                        ptr = strchr(buf, ':');
                        *ptr = '\0';
                                
                        miscdev = miscNewDevice(NULL);
                        miscdev->type = CLASS_HD;
                        miscdev->desc = strdup("Compaq RAID logical disk");
                        miscdev->device = strdup(buf);
                        if (devlist)
                            miscdev->next = devlist;
                        devlist = (struct device *) miscdev;
                    }
                }
                sprintf(ctl, "%s/cciss%d", path, ++ctlNum);
                fclose(f);
            }

            /* cpqarray */
            ctlNum = 0;
            path = "/proc/driver/cpqarray";
            sprintf(ctl, "%s/ida%d", path, ctlNum);
            while ((f = fopen(ctl, "r"))) {
                while (fgets(buf, sizeof(buf) - 1, f)) {
                    if (!strncmp(buf, "ida/", 4)) {
                        ptr = strchr(buf, ':');
                        *ptr = '\0';
                        
                        miscdev = miscNewDevice(NULL);
                        miscdev->type = CLASS_HD;
                        miscdev->desc = strdup("Compaq RAID logical disk");
                        miscdev->device = strdup(buf);
                        if (devlist)
                            miscdev->next = devlist;
                        devlist = (struct device *) miscdev;
                    }
                }
                sprintf(ctl, "%s/ida%d", path, ++ctlNum);
                fclose(f);
            }
            
            /* dac960 */
            ctlNum = 0;
            sprintf(ctl, "/proc/rd/c%d/current_status", ctlNum);
            
            while ((f = fopen(ctl, "r"))) {
                while(fgets(buf, sizeof(buf) - 1, f)) {
		    char *start;

		    start = strchr(buf, '/');
                    if (start && !strncmp(start, "/dev/rd/", 8)) {
                        ptr = strchr(start, ':');
                        *ptr = '\0';

                        miscdev = miscNewDevice(NULL);
                        miscdev->type = CLASS_HD;
                        miscdev->desc = strdup("DAC960 RAID logical disk");
                        miscdev->device = strdup(start+5); /* skip "/dev/" */
                        if (devlist)
                            miscdev->next = devlist;
                        devlist = (struct device *) miscdev;
                    }
                }
                sprintf(ctl, "/proc/rd/c%d/current_status", ++ctlNum);
                fclose(f);
            }

            /* sx8 */
            if (!access("/sys/block", R_OK))  {
                DIR * dir;
                struct dirent * ent;
                char * c;

                dir = opendir("/sys/block");
                while ((ent = readdir(dir))) {
                    if (strncmp(ent->d_name, "sx8", 3))
                        continue;
                    miscdev = miscNewDevice(NULL);
                    miscdev->type = CLASS_HD;
                    miscdev->desc = strdup("Promise SX8 block device");
                    miscdev->device = strdup(ent->d_name);
                    for (c = miscdev->device; *c; c++)
                        if (*c == '!') *c = '/';
                    if (devlist)
                        miscdev->next = devlist;
                    devlist = (struct device *) miscdev;
                }
		closedir(dir);
            }

            /* I2O */
            if (!access("/sys/bus/i2o/devices", R_OK)) {
                DIR * dir;
                struct dirent * ent;
                char path[128];

                dir = opendir("/sys/bus/i2o/devices");
                while ((ent = readdir(dir))) {

                    if (ent->d_name[0] == '.') continue;
		    snprintf(path, 128, "/sys/bus/i2o/devices/%s", ent->d_name);
		    miscdev = miscNewDevice(NULL);
		    miscdev->type = CLASS_HD;
		    miscdev->desc = strdup("I2O block device");
		    __getSysfsDevice((struct device *)miscdev, path, "block:", 1);
		    if (devlist)
			miscdev->next = devlist;
		    devlist = (struct device *) miscdev;
		}
		closedir(dir);
            } else { /* fall back to i2o_proc stuff */
                ctlNum = 0;
                sprintf(ctl, "/proc/i2o/iop%d/lct", ctlNum);
            
                while ((f = fopen(ctl, "r"))) {
                    int found = 0;
                    int devnum = 0;
                    char local_tid[] = "0x000";
                    char devname[8];
                    while(fgets(buf, sizeof(buf) - 1, f)) {
                        static char *i2o_class = "  Class, SubClass  : Block Device, Direct-Access Read/Write";
                        static char *i2o_local_tid = "  Local TID        : ";
                        static char *i2o_user_tid = "  User TID         : 0xfff";

                        if (!strncmp(buf, i2o_class, strlen(i2o_class))) {
                            found = 1;
                            continue;
                        }
                        if (found && !strncmp(buf, i2o_local_tid, strlen(i2o_local_tid))) {
                            strncpy(local_tid, buf + strlen(i2o_local_tid), 5);
                            continue;
                        }
                        if (found && !strncmp(buf, i2o_user_tid, strlen(i2o_user_tid))) {
                            found = 0;
                            miscdev = miscNewDevice(NULL);
                            miscdev->type = CLASS_HD;
                            miscdev->desc = strdup("I2O block device");
                            sprintf(devname, "i2o/hd%c", 'a' + (devnum ++));
                            miscdev->device = strdup(devname);
                            if (devlist)
                                miscdev->next = devlist;
                            devlist = (struct device *) miscdev;
                        }
                    }
                    sprintf(ctl, "/proc/i2o/iop%d/lct", ++ctlNum);
                    fclose(f);
                }
            }

        }


	if ((probeClass & CLASS_FLOPPY) || (probeClass & CLASS_SOCKET) ||
	    (probeClass & CLASS_CDROM)) {
		if (probeClass & CLASS_FLOPPY) {
			for (x=0; x<=3; x++) {
				snprintf(path, 31, "/dev/fd%d", x);
				fd = open(path, O_RDONLY|O_NONBLOCK);
				if (fd < 0)
					break;
				ioctl(fd, FDRESET, NULL);
				rc = ioctl(fd, FDGETDRVTYP, name);
				if (rc || !name || !strcmp(name, "(null)"))
					goto cont;
				rc = ioctl(fd, FDPOLLDRVSTAT, &ds);
				if (rc)
					goto cont;
				miscdev = miscNewDevice(NULL);
				miscdev->device = strdup(basename(path));
				miscdev->type = CLASS_FLOPPY;
				miscdev->desc = getFloppyDesc(name);
				if (ds.track < 0)
					miscdev->detached = 1;
				if (devlist)
					miscdev->next = devlist;
				devlist = (struct device *) miscdev;
				cont:
				close(fd);
			}
		}
#if defined(__i386__) || defined(__alpha__)
		if (probeClass & CLASS_SOCKET) {
			int val, sock, done, i;
			unsigned short old;
			
			sock = done = 0;
			i = ioperm(i365_base, 4, 1);
			if (i == 0)
				i = ioperm(0x80, 1, 1);

			if (i == 0)
				for (; sock < 2; sock++) {
					printf("vorher\n");
					fflush(stdout);
					val = i365_get(sock, I365_IDENT);
					printf("nachher\n");
					fflush(stdout);
					switch (val) {
					case 0x82:
					case 0x83:
					case 0x84:
					case 0x88:
					case 0x89: 
					case 0x8a:
					case 0x8b:
					case 0x8c:
						break;
					case 0:
						return(0);
					default:
						done = 1;
					}
					if (done) break;
				}
			
			if (sock) {
				miscdev = miscNewDevice(NULL);
				miscdev->type = CLASS_SOCKET;
				miscdev->desc = strdup("Generic i82365-compatible PCMCIA controller");
				miscdev->driver = strdup("i82365");
				if (devlist)
					miscdev->next = devlist;
				devlist = (struct device *) miscdev;
			}
			
			i = ioperm(tcic_base, 16, 1);
			if (i == 0)
				i = ioperm(0x80, 1, 1);
			if (i)
				return devlist;
			
			/* Anything there?? */
			for (i = 0; i < 0x10; i += 2)
				if (inw(tcic_base + i) == 0xffff)
					return devlist;

			/* Try to reset the chip */
			outw(TCIC_SCTRL_RESET, tcic_base + TCIC_SCTRL);
			outw(0, tcic_base + TCIC_SCTRL);
    
			/* Can we set the addr register? */
			old = inw(tcic_base + TCIC_ADDR);
			outw(0, tcic_base + TCIC_ADDR);
			if (inw(tcic_base + TCIC_ADDR) != 0) {
				outw(old, tcic_base + TCIC_ADDR);
				return devlist;
			}
    
			outw(0xc3a5, tcic_base + TCIC_ADDR);
			if (inw(tcic_base + TCIC_ADDR) != 0xc3a5)
				return devlist;

			miscdev = miscNewDevice(NULL);
			miscdev->type = CLASS_SOCKET;
			miscdev->desc = strdup("Generic TCIC-2 PCMCIA controller");
			miscdev->driver = strdup("tcic");
			if (devlist)
				miscdev->next = devlist;
			devlist = (struct device *) miscdev;
		}
#endif
	}
	return devlist;
}
