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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "parallel.h"
#include "modules.h"


static char *procDirs[] = { "/proc/sys/dev/parport", "/proc/parport"
};

static int devCmp(const void * a, const void * b) {
	const struct parallelDevice * one = a;
	const struct parallelDevice * two = b;
	int x=0, y=0;
	
	x = strcmp(one->pnpmfr,two->pnpmfr);
	y = strcmp(one->pnpmodel,two->pnpmodel);
	if (x)
	  return x;
	else
	  return y;
}


static void parallelFreeDevice( struct parallelDevice *dev ) {
	if (dev->pnpmodel) free(dev->pnpmodel);
	if (dev->pnpmfr) free(dev->pnpmfr);
	if (dev->pnpmodes) free(dev->pnpmodes);
	if (dev->pnpdesc) free(dev->pnpdesc);
	freeDevice( (struct device *)dev);
}

static void parallelWriteDevice( FILE *file, struct parallelDevice *dev) 
{
	writeDevice(file, (struct device *)dev);
	if (dev->pnpmodel)
	  fprintf(file,"pnpmodel: %s\n",dev->pnpmodel);
	if (dev->pnpmfr)
	  fprintf(file,"pnpmfr: %s\n",dev->pnpmfr);
	if (dev->pnpmodes)
	  fprintf(file,"pnpmodes: %s\n",dev->pnpmodes);
	if (dev->pnpdesc)
	  fprintf(file,"pnpdesc: %s\n",dev->pnpdesc);
}

static int parallelCompareDevice( struct parallelDevice *dev1, struct parallelDevice *dev2)
{
	int x,y;

	x=compareDevice((struct device *)dev1, (struct device *)dev2);
	if (x && x!=2)
	  return x;
	if ((y=devCmp((void *)dev1, (void *)dev2)))
	  return y;
	return x;
}

struct parallelDevice *parallelNewDevice( struct parallelDevice *old) {
	struct parallelDevice *ret;
    
	ret = malloc( sizeof(struct parallelDevice) );
	memset(ret, '\0', sizeof (struct parallelDevice));
	ret=(struct parallelDevice *)newDevice((struct device *)old,(struct device *)ret);
	ret->bus = BUS_PARALLEL;
	ret->newDevice = parallelNewDevice;
	ret->freeDevice = parallelFreeDevice;
	ret->writeDevice = parallelWriteDevice;
	ret->compareDevice = parallelCompareDevice;
	if (old && old->bus == BUS_PARALLEL) {
		if (old->pnpmodel)
		  ret->pnpmodel=strdup(old->pnpmodel);
		if (old->pnpmfr)
		  ret->pnpmfr=strdup(old->pnpmfr);
		if (old->pnpmodes)
		  ret->pnpmodes=strdup(old->pnpmodes);
		if (old->pnpdesc)
		  ret->pnpdesc=strdup(old->pnpdesc);
	}
	return ret;
}

static struct parallelDevice *readProbeInfo(char *ppath) {
	int lpfile,bytes;
	char *probebuf,*ptr;
	char *mfr,*model,*desc,*func,*modes;
	struct parallelDevice *pardev;
	
	lpfile=open(ppath,O_RDONLY);
	if (lpfile==-1) return NULL;
	probebuf=calloc(8192,sizeof(char));
	bytes=read(lpfile,probebuf,8192);
	if (bytes<=0) {
		close(lpfile);
		return NULL;
	}
	
	mfr=model=desc=func=modes=NULL;
	ptr = probebuf;
	while (probebuf[0]!='\0') {
		while (*ptr && *ptr !='\n') ptr++;
		if (*ptr) {
			*ptr='\0';
			/* strip trailing semicolon, too */
			*(ptr-1)='\0';
			ptr++;
		}
		if (!strncmp(probebuf,"MFG:",4) || !strncmp(probebuf,"MANUFACTURER:",13)) {
			mfr=strdup(strstr(probebuf,":")+1);
		}
		if (!strncmp(probebuf,"MDL:",4) || !strncmp(probebuf,"MODEL:",6)) {
			model=strdup(strstr(probebuf,":")+1);
		}
		if (!strncmp(probebuf,"CLS:",4) || !strncmp(probebuf,"CLASS:",6)) {
			func=strdup(strstr(probebuf,":")+1);
		}
		if (!strncmp(probebuf,"CMD:",4) || !strncmp(probebuf,"COMMAND SET:",12)) {
			modes=strdup(strstr(probebuf,":")+1);
		}
		if (!strncmp(probebuf,"DES:",4) || !strncmp(probebuf,"DESCRIPTION:",12)) {
			desc=strdup(strstr(probebuf,":")+1);
		}
		probebuf=ptr;
	}
	if (!mfr && !model) return NULL;
	if ((mfr && !strcmp(mfr,"Unknown vendor"))&&(model && !strcmp(model,"Unknown device"))) return NULL;
	pardev = parallelNewDevice(NULL);
	if (desc)
		pardev->desc=strdup(desc);
	else {
		desc=malloc(strlen(mfr)+strlen(model)+2);
		if (mfr && model) {
			snprintf(desc,strlen(mfr)+strlen(model)+2,"%s %s",mfr,model);
		}
		pardev->desc=strdup(desc);
	}
	pardev->pnpmfr = strdup(mfr);
	pardev->pnpmodel = strdup(model);
	if (modes)
		pardev->pnpmodes = strdup(modes);
	if (desc)
		pardev->pnpdesc = strdup(desc);
	if (func) {
		if (!strcmp(func,"PRINTER")) {
			pardev->type=CLASS_PRINTER;
		} else if (!strcmp(func,"MODEM")) {
			pardev->type=CLASS_MODEM;
		} else if (!strcmp(func,"NET")) {
			pardev->type=CLASS_NETWORK;
		} else if (!strcmp(func,"HDC")) {
			pardev->type=CLASS_HD;
		} else if (!strcmp(func,"FDC")) {
			pardev->type=CLASS_FLOPPY;
		} else if (!strcmp(func,"SCANNER")) {
			pardev->type=CLASS_SCANNER;
		} else {
			pardev->type=CLASS_OTHER;
		}
	} else pardev->type=CLASS_OTHER;
	
	if (mfr) free(mfr);
	if (model) free(model);
	if (func) free(func);
	if (modes) free(modes);
	if (desc) free(desc);
	return pardev;
}

struct device *parallelProbe( enum deviceClass probeClass, int probeFlags,
			 struct device *devlist) {
	DIR *dir;
	struct parallelDevice *pardev;
	struct dirent *dent;
	char path[256];
	int procdir = 0;
    
	if (
	    (probeClass & CLASS_OTHER) ||
	    (probeClass & CLASS_NETWORK) ||
	    (probeClass & CLASS_FLOPPY) || 
	    (probeClass & CLASS_CDROM) ||
	    (probeClass & CLASS_HD) ||
	    (probeClass & CLASS_TAPE) ||
	    (probeClass & CLASS_SCANNER)  ||
	    (probeClass & CLASS_PRINTER)
	    ) {
		dir=opendir(procDirs[0]);
		if (!dir) {
			dir=opendir(procDirs[1]);
			if (!dir)
			  goto out;
			else
			  procdir=1;
		} else {
			procdir=0;
		}
		while ((dent=readdir(dir))) {
			if (dent->d_name[0]=='.') continue;
			snprintf(path,256,"%s/%s/autoprobe",procDirs[procdir],dent->d_name);
			pardev=readProbeInfo(path);
			if (!pardev) {
				continue;
			}
			if (pardev->type & probeClass) {
				if (!strncmp(dent->d_name,"parport",7))
					snprintf(path,256,"/dev/lp%s",dent->d_name+7);
				else
					snprintf(path,256,"/dev/lp%s",dent->d_name);
				pardev->device = strdup(basename(path));
				if (devlist)
				  pardev->next = devlist;
				devlist = (struct device *) pardev;
			} else {
				pardev->freeDevice(pardev);
			}
		}
		closedir(dir);
	}
out:
	return devlist;
}
