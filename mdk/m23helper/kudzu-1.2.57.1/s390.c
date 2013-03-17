/* Copyright 2004 Red Hat, Inc.
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
#include <sys/ioctl.h>
#include <linux/fd.h>

#include "s390.h"
#include "modules.h"

static void s390FreeDevice(struct s390Device *dev)
{
	freeDevice((struct device *) dev);
}

static void s390WriteDevice(FILE *file, struct s390Device *dev)
{
	writeDevice(file, (struct device *)dev);
}

static int s390CompareDevice(struct s390Device *dev1, struct s390Device *dev2)
{
	return compareDevice( (struct device *)dev1, (struct device *)dev2);
}

struct s390Device *s390NewDevice(struct s390Device *old)
{
	struct s390Device *ret;

	ret = malloc(sizeof(struct s390Device));
	memset(ret, '\0', sizeof(struct s390Device));
	ret = (struct s390Device *) newDevice((struct device *) old, (struct device *) ret);
	ret->bus = BUS_S390;
	ret->newDevice = s390NewDevice;
	ret->freeDevice = s390FreeDevice;
	ret->writeDevice = s390WriteDevice;
	ret->compareDevice = s390CompareDevice;
	return ret;
}

struct device *s390Probe(enum deviceClass probeClass, int probeFlags,
			struct device *devlist)
{
#if defined(__s390__) || defined(__s390x__)
    struct s390Device *s390dev;
    char devname[9];
    FILE * fp;
    char * line;


    if (probeClass & CLASS_HD) {
	int ret;

	if (access("/proc/dasd/devices", R_OK))
	    goto dasddone;

	/* Read from /proc/dasd/devices */
	fp = fopen("/proc/dasd/devices", "r");
	if (fp < 0) {
	    /* fprintf(stderr, "failed to open /proc/dasd/devices!\n");*/
	    goto dasddone;
	}

        line = (char *)malloc(100*sizeof(char));
        while (fgets (line, 100, fp) != NULL) {
	  ret = sscanf (line, "%*[A-Za-z0-9.](ECKD) at ( %*d: %*d) is %s :\ %*s",
			devname);
	  if (ret == 1) {
	    s390dev = s390NewDevice(NULL);
	    s390dev->device = strdup(devname);
	    s390dev->desc = strdup("IBM ECKD DASD");
	    s390dev->type = CLASS_HD;
	    if (devlist)
	      s390dev->next = devlist;
	    devlist = (struct device *) s390dev;
	  }
	}
        if (fp) fclose(fp);

dasddone:
        ;
    } 

    if (probeClass & CLASS_NETWORK) {
	int devnum = -1;
	int ret;

	if (!access("/sys/class/net", R_OK)) {
	    DIR * dir;
	    struct dirent * ent;
            char *linkname = NULL;
            char *buf = NULL, * buff = NULL;
            int len = 0;
	    if ((dir = opendir("/sys/class/net"))) {
		while ((ent = readdir(dir))) {
		    if (ent->d_name[0] == '.') continue;
		    if (strncmp(ent->d_name, "ctc", 3) == 0) {
			ret = sscanf (ent->d_name, "ctc%d", &devnum);
			if ((ret == 1) && (devnum != -1)) {
			    s390dev = s390NewDevice(NULL);
			    s390dev->device = strdup(ent->d_name);
			    s390dev->desc = strdup("IBM CTC");
			    s390dev->driver = strdup("ctc");
			    s390dev->type = CLASS_NETWORK;
			    if (devlist)
				s390dev->next = devlist;
			    devlist = (struct device *) s390dev;
			}
		    } else if (strncmp(ent->d_name, "escon", 5) == 0) {
			ret = sscanf (ent->d_name, "escon%d", &devnum);
			if ((ret == 1) && (devnum != -1)) {
			    s390dev = s390NewDevice(NULL);
			    s390dev->device = strdup(ent->d_name);
			    s390dev->desc = strdup("IBM ESCON");
			    s390dev->driver = strdup("ctc");
			    s390dev->type = CLASS_NETWORK;
			    if (devlist)
				s390dev->next = devlist;
			    devlist = (struct device *) s390dev;
			}
		    } else if (strncmp(ent->d_name, "hsi", 3) == 0) {
			ret = sscanf (ent->d_name, "hsi%d", &devnum);
			if ((ret == 1) && (devnum != -1)) {
			    s390dev = s390NewDevice(NULL);
			    s390dev->device = strdup(ent->d_name);
			    s390dev->desc = strdup("IBM HiperSockets");
			    s390dev->driver = strdup("qeth");
			    s390dev->type = CLASS_NETWORK;
			    if (devlist)
				s390dev->next = devlist;
			    devlist = (struct device *) s390dev;
			}
		    } else if (strncmp(ent->d_name, "eth", 3) == 0) {
                        linkname = (char *)malloc(sizeof(char)*256);
                        buff = buf = (char *)malloc(sizeof(char)*256);
                        sprintf(linkname, "/sys/class/net/%s/device/driver", ent->d_name);
                        len = readlink(linkname, buf, 255);
                        buf[len] = '\0';
                        buf = rindex(buf, '/');
                        if(buf && strstr(buf, "lcs")) {
			    ret = sscanf (ent->d_name, "eth%d", &devnum);
			    if ((ret == 1) && (devnum != -1)) {
			        s390dev = s390NewDevice(NULL);
			        s390dev->device = strdup(ent->d_name);
			        s390dev->desc = strdup("IBM LCS");
			        s390dev->driver = strdup("lcs");
			        s390dev->type = CLASS_NETWORK;
			        if (devlist)
				    s390dev->next = devlist;
			        devlist = (struct device *) s390dev;
			    }
		        } else {
			    ret = sscanf (ent->d_name, "eth%d", &devnum);
		            if ((ret == 1) && (devnum != -1)) {
		                s390dev = s390NewDevice(NULL);
			        s390dev->device = strdup(ent->d_name);
		                s390dev->desc = strdup("IBM QETH");
		                s390dev->driver = strdup("qeth");
		                s390dev->type = CLASS_NETWORK;
		                if (devlist)
			            s390dev->next = devlist;
		                devlist = (struct device *) s390dev;
                            }
                        }
                        free(linkname);
                        free(buff);
		    } else if (strncmp(ent->d_name, "tr", 2) == 0) {
                        linkname = (char *)malloc(sizeof(char)*256);
                        buff = buf = (char *)malloc(sizeof(char)*256);
                        sprintf(linkname, "/sys/class/net/%s/device/driver", ent->d_name);
                        len = readlink(linkname, buf, 255);
                        buf[len] = '\0';
                        buf = rindex(buf, '/');
                        if(buf && strstr(buf, "lcs")) {
    			    ret = sscanf (ent->d_name, "tr%d", &devnum);
    			    if ((ret == 1) && (devnum != -1)) {
    			        s390dev = s390NewDevice(NULL);
    			        s390dev->device = strdup(ent->d_name);
    			        s390dev->desc = strdup("IBM Token Ring (LCS)");
    			        s390dev->driver = strdup("lcs");
    			        s390dev->type = CLASS_NETWORK;
    			        if (devlist)
    				    s390dev->next = devlist;
    			        devlist = (struct device *) s390dev;
    			    }
		        } else { 
    			    ret = sscanf (ent->d_name, "tr%d", &devnum);
		            if ((ret == 1) && (devnum != -1)) {
			        s390dev = s390NewDevice(NULL);
    			        s390dev->device = strdup(ent->d_name);
    			        s390dev->desc = strdup("IBM Token Ring (QETH)");
			        s390dev->driver = strdup("qeth");
			        s390dev->type = CLASS_NETWORK;
			        if (devlist)
			           s390dev->next = devlist;
			        devlist = (struct device *) s390dev;
		            }
	                }
                        free(linkname);
                        free(buff);
		    }
		}
		closedir(dir);
	    }
	}
	if (!access("/sys/bus/iucv/drivers/netiucv", R_OK)) {
	    DIR * dir;
	    struct dirent * ent;
	    if ((dir = opendir("/sys/bus/iucv/drivers/netiucv"))) {
		while ((ent = readdir(dir))) {
		    if (ent->d_name[0] == '.') continue;
		    if (strncmp(ent->d_name, "netiucv", 7) == 0) {
			ret = sscanf (ent->d_name, "netiucv%d", &devnum);
			if ((ret == 1) && (devnum != -1)) {
			    s390dev = s390NewDevice(NULL);
			    s390dev->device = malloc(10);
			    snprintf(s390dev->device, 9, "iucv%d", devnum);
			    s390dev->desc = strdup("IBM IUCV");
			    s390dev->driver = strdup("netiucv");
			    s390dev->type = CLASS_NETWORK;
			    if (devlist)
				s390dev->next = devlist;
			    devlist = (struct device *) s390dev;
			}
		    }
		}
		closedir(dir);
	    }
	}
    } 
#endif
    return devlist;
}
