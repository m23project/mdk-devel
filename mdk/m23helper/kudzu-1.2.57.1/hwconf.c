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
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include <linux/sockios.h>
#include <net/if.h>

#include <libgen.h>
#include <libintl.h>
#include <locale.h>

#include <popt.h>

#define _(String) gettext((String))
#define N_(String) (String)

#include "kudzu.h"

#include "kudzuint.h"

#include "modules.h"

#define FILENAME "hwconf"

static int madebak=0;
static int timeout=0;
static int probeonly=0;
static int safe=0;

struct device **storedDevs;
int numStored=0;
struct device **currentDevs;
int numCurrent=0;

int configuredX = -1;

static char *_module_file = NULL;
static float _kernel_release;

char *checkConfFile()
{
	char path[_POSIX_PATH_MAX];
	struct stat sbuf;
	
	snprintf(path,_POSIX_PATH_MAX,"/etc/sysconfig/%s",FILENAME);
	if (stat(path,&sbuf)==-1) {
		snprintf(path,_POSIX_PATH_MAX,"./%s",FILENAME);
		if (stat(path,&sbuf)==-1) {
			return NULL;
		}
	}
	return strdup(path);
}

#ifdef __sparc__
/* We load a default keymap so that the user can even
   move around and select what he wants when changing from
   pc to sun keyboard or vice versa. */
int installDefaultKeymap(int sunkbd)
{
	char buf[256], *keymap;
	sprintf (buf, "/bin/loadkeys %s < /dev/tty0 > /dev/tty0 2>/dev/null",
		 sunkbd ? "sunkeymap" : "us");
	system (buf);
	openlog("kudzu",0,LOG_USER);
	syslog(LOG_NOTICE,_("ran loadkeys %s"), sunkbd ? "sunkeymap" : "us");
	closelog();
}

/* Return 0 if /etc/sysconfig/keyboard is ok, -1 if not */
int checkKeyboardConfig(struct device *dev)
{
	char buf[256], *p;
	FILE *f = fopen("/etc/sysconfig/keyboard", "r");
	if (!f) return 0;

	if (!f) {
		if (errno == ENOENT && dev->device && 
		    !strcmp (dev->device, "console"))
			return 0;
		return -1;
	}
	if (dev->device && !strcmp (dev->device, "console"))
		return -1;
	while (fgets(buf, sizeof(buf), f) != NULL) {
		p = strstr(buf, "KEYBOARDTYPE=");
		if (p == NULL) continue;
		if (strstr (p, "pc") != NULL) {
			if (dev->device == NULL)
				return 0;
			installDefaultKeymap(1);
			return -1;
		} else if (strstr (p, "sun") != NULL) {
			if (dev->device != NULL)
				return 0;
			installDefaultKeymap(0);
			return -1;
		}
		return -1;
	}
	fclose(f);
	return -1;
}
#endif

static inline int isSerialish(struct device *dev) {
	return (dev->device && (
			    !strcmp(dev->device, "console") || 
			    !strncmp(dev->device, "ttyS", 4) || 
			    !strcmp(dev->device, "hvc0") ||
			    !strcmp(dev->device, "ttySG0") ||
			    !strncmp(dev->device, "hvsi", 4) ||
                            !strncmp(dev->device, "xvc", 3)));
}

/* Return 0 if /etc/inittab is ok, non-zero if not.
   Bit 0 is set if at least one line needs commenting out,
   bit 1 if the /sbin/agetty console line is present, but commented out.
 */
int checkInittab(struct device *dev)
{
	char buf[1024], *p;
	int ret = 0, comment, hasVideo = 0, x, foundgetty=0, isSerial = isSerialish(dev);
	FILE *f = fopen("/etc/inittab", "r");
	if (!f) return 0;

	for (x=0; currentDevs[x]; x++) {
		if (currentDevs[x]->type == CLASS_VIDEO) {
			hasVideo = 1;
			break;
		}
	}
	
	while (fgets(buf, sizeof(buf), f) != NULL) {
		for (p = buf; isspace (*p); p++);
		comment = *p == '#';
		if (comment) p++;
		if (*p == '#') p++;
		while (*p && *p != ':') p++;
		if (!*p) continue;
		p++;
		while (*p && *p != ':') p++;
		if (strncmp (p, ":respawn:", 9)) continue;
		p += 9;
		while (*p && isspace (*p)) p++;
		if (!strncmp (p, "/sbin/mingetty", 14)) {
			if (!comment && isSerial && !hasVideo)
				ret |= 1;
		} else if (!strncmp (p, "/sbin/agetty", 12) || !strncmp(p, "/sbin/mgetty", 12)) {
			p += 12;
			if (!isspace (*p)) continue;
			if (dev->device && strstr (p, dev->device)) {
				foundgetty++;
				if (!comment && !isSerial)
					ret |= 1;
				else if (comment && isSerial)
					ret |= 2;
			}
		}
	}
	if (!foundgetty && isSerial)
		ret |= 1;
	fclose(f);
	return ret;
}

int rewriteInittab(struct device *dev)
{
	char buf[1024], *p;
	int ret, check;
	char *comment;
	int isSerial = isSerialish(dev);
	int hasVideo = 0, x;
	FILE *f;
	FILE *g;
	char speed[10];	
	
	for (x=0; currentDevs[x]; x++) {
		if (currentDevs[x]->type == CLASS_VIDEO) {
			hasVideo = 1;
			break;
		}
	}

	check = checkInittab(dev);
	if (!check)
		return 1;
	if (isSerial) {
		for (p = dev->desc; *p && !isdigit(*p); p++);
		ret = atoi (p);
		if (!strcmp(dev->desc,"pSeries LPAR console"))
			ret=38400;
		switch (ret) {
		default:
			ret = 9600;
			/* Fall through */
		case 9600:
		case 19200:
		case 38400:
		case 2400:
		case 57600:
		case 115200:
		case 230400:
			sprintf (speed, "%d", ret);
			break;
		}
	}
	f = fopen("/etc/inittab", "r");
	if (!f) return -1;
	g = fopen("/etc/inittab-", "w");
	if (!g) {
		fclose (f);
		return -1;
	}
	while (fgets(buf, sizeof(buf), f) != NULL) {
		for (p = buf; isspace (*p); p++);
		comment = NULL;
		if (*p == '#') comment = p++;
		if (*p == '#') {
			/* To protect /sbin/mingetty and /sbin/agetty respawn lines from being
			   automagically uncommented, just add two ## as in
			   ##7:2345:respawn:/sbin/mingetty tty7
			 */
			fputs (buf, g);
			continue;
		}
		if (isSerial && !strncmp(p, "id:5:", 5) && !hasVideo) {
			/* Running X from serial console is generally considered a bad idea. */
			fputs ("id:3:initdefault:\n", g);
			continue;
		}
		while (*p && *p != ':') p++;
		if (!*p) {
			fputs (buf, g);
			continue;
		}
		p++;
		while (*p && *p != ':') p++;
		if (strncmp (p, ":respawn:", 9)) {
			fputs (buf, g);
			continue;
		}
		p += 9;
		while (*p && isspace (*p)) p++;
		if (!strncmp (p, "/sbin/mingetty", 14)) {
			if (isSerial) {
				if (!(check & 2)) {
					fprintf (g, "co:2345:respawn:/sbin/agetty %s %s vt100-nav\n", dev->device, speed);
					check |= 2;
				}
				if (!comment && !hasVideo) {
					fprintf (g, "#%s", buf);
					continue;
				} else {
					fputs(buf,g);
					continue;
				}
			} else if (comment) {
				if (comment != buf)
					fwrite (buf, 1, comment - buf, g);
				fputs (comment + 1, g);
				continue;
			}
		} else if (!strncmp (p, "/sbin/agetty", 12) || !strncmp(p, "/sbin/mgetty", 12)) {
			p += 12;
			if (!isspace (*p)) {
				fputs (buf, g);
				continue;
			}
			if ((p = strstr (p, dev->device)) != NULL) {
				if (!isSerial && !comment) {
					fprintf (g, "#%s", buf);
					continue;
				} else if (isSerial && comment) {
					char *q;
					if (comment != buf)
						fwrite (buf, 1, comment - buf, g);
					fwrite (comment + 1, 1, p + 7 - comment - 1, g);
					while (isspace (*p)) p++;
					for (q = p; *q && strchr ("DTF0123456789", *q); q++);
					while (isspace (*q)) q++; 
					if (q != p && *q)
						fprintf (g, " %s %s", speed, q);
					else
						fprintf (g, " %s vt100-nav\n", speed);
					continue;
				}
			}
		}
		if (strncmp(buf,"co:",3) || !isSerial)
			fputs (buf, g);
	}
	fclose(f);
	fclose(g);
	unlink("/etc/inittab");
	rename("/etc/inittab-", "/etc/inittab");
	return 0;
}

int rewriteSecuretty(struct device *dev)
{
	char buf[256], *p = NULL;
	char buf2[64];
	FILE *f;

	snprintf(buf2, 64, "%s\n", dev->device);
	f = fopen("/etc/securetty", "r+");
	if (!f) return -1;
	while (fgets (buf, sizeof(buf), f) != NULL) {
		if (!strcmp (buf, buf2)) {
			fclose(f);
			return 0;
		}
		p = strchr (buf, '\n');
	}
	if (!p)
		fputs("\n", f);
	fputs (buf2, f);
	fclose(f);
	return 0;
}

int checkForModule(char *the_kernel_ver, char *modulename)
{
	struct stat sbuf;
	char path[512], mod_name[100];
	char *buf;
	int fd;
	
	snprintf(path,512,"/lib/modules/%s/modules.dep",the_kernel_ver);
	if (!stat(path,&sbuf)) {
		fd = open(path,O_RDONLY);
		buf = mmap(0,sbuf.st_size,PROT_READ,MAP_SHARED,fd,0);
		if (!buf || buf == MAP_FAILED)
			return 0;
		close(fd);
		snprintf(mod_name,100,"/%s.ko:",modulename);
		if (strstr(buf,mod_name)) {
			munmap(buf,sbuf.st_size);
			return 1;
		}
		snprintf(mod_name,100,"/%s.ko.gz:",modulename);
		if (strstr(buf,mod_name)) {
			munmap(buf,sbuf.st_size);
			return 1;
		}
		munmap(buf,sbuf.st_size);
	}
	return 0;
}

int isAvailable(char *modulename)
{
	if (checkForModule(kernel_ver, modulename))
		return 1;
	return 0;
}

int isConfigurable(struct device *dev) {
	struct stat tmpstat;
	
	switch (dev->type) {
	 case CLASS_NETWORK:
	 case CLASS_KEYBOARD:
	 case CLASS_AUDIO:
		return 1;
	 case CLASS_VIDEO:
		if (!stat("/usr/bin/system-config-display", &tmpstat) &&
		     !stat("/usr/bin/Xorg", &tmpstat))
			return 1;
		else
			return 0;
	 default:
		return 0;
	}
	return 0;
}

int isConfigured(struct device *dev)
{
	struct confModules *cf;
	char path[256], path2[256];
	struct stat sbuf;
	int ret=0;
	
	memset(path,'\0',256);
	memset(path2,'\0',256);
	cf = readConfModules(_module_file);
	switch (dev->type) {
	 case CLASS_NETWORK:
		if (!dev->driver)
		  ret=1;
		/* Assume cardbus stuff is unconfigured */
		if (dev->bus == BUS_PCMCIA)
			break;
		if (dev->bus != BUS_PCI || ((struct pciDevice *)dev)->pciType != PCI_CARDBUS)
		  if (cf)
		    if (dev->device && dev->driver && isAliased(cf,dev->device,dev->driver)!=-1)
		      ret = 1;
		break;
	 case CLASS_SATA:
	 case CLASS_ATA:
	 case CLASS_RAID:
	 case CLASS_SCSI:
	 case CLASS_IDE:
		if (!dev->driver)
		  ret=1;
		if (cf)
		  if (isAliased(cf,"scsi_hostadapter",dev->driver)!=-1)
		    ret=1;
		break;
	 case CLASS_VIDEO:
	 case CLASS_MONITOR:
		/* Assume on initial runs that if X is configured, we got the right card */
#ifndef __sparc__
		if (!stat("/etc/X11/xorg.conf",&sbuf))
		  ret = 1;
#else
		ret = 1;
#endif
		if (!dev->driver)
		  ret=1;
		break;
	 case CLASS_AUDIO:
		if (!dev->driver)
		  ret=1;
		if (cf) {
			if (isAliased(cf,"sound",dev->driver)!=-1)
			  ret = 1;
			if (isAliased(cf,"sound-card-",dev->driver)!=-1)
			  ret = 1;
			if (isAliased(cf,"sound-slot-",dev->driver)!=-1)
			  ret = 1;
			if (isAliased(cf,"snd-card-",dev->driver)!=-1)
			  ret = 1;
		}
		break;
	 case CLASS_KEYBOARD:
#ifdef __sparc__
		if (!checkKeyboardConfig(dev) && !checkInittab(dev))
			ret = 1;
#else
		if (!checkInittab(dev))
			ret = 1;
#endif
		break;	 
	 default:
		/* If we don't know how to configure it, assume it's configured. */
		ret = 1;
		break;
	}
	if (cf)
	  freeConfModules(cf);
	return ret;
}

int do_rename(char *src, char *target) {
	int sock;
	struct ifreq ifr;
	int ret;
	        
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
		return 1;
	        
	memset(&ifr,'\0', sizeof(struct ifreq));
	strncpy(ifr.ifr_name, src, IFNAMSIZ);
	strncpy(ifr.ifr_newname, target, IFNAMSIZ);
	ret = ioctl(sock, SIOCSIFNAME, &ifr);
	close(sock);
	return ret;
}

void rename_device_if_needed(struct device *dev) {
	glob_t globres;
	char *pattern = "/sys/class/net/*/address";
	char *addr, *path;
	int x;
	
	if (!dev->classprivate || !dev->device) return;
	if (glob(pattern, 0, NULL, &globres))
		return;
	for (x = 0 ; x < globres.gl_pathc ; x++) {
		addr = __readString(globres.gl_pathv[x]);
		path = basename(dirname(globres.gl_pathv[x]));
		if (!strcmp(dev->classprivate,addr) && strcmp(dev->device, path)) {
			do_rename(path, dev->device);
		}
	}
	globfree(&globres);
}

int configure(struct device *dev)
{
	struct confModules *cf;
	char path[256];
	int index;

	switch (dev->type) {
	 case CLASS_NETWORK:
	        if (!dev->device || !dev->driver) break;
		cf = readConfModules(_module_file);
		if (!cf) 
		  cf = newConfModules();
		cf->madebackup = madebak;
		if (isAliased(cf,dev->device,dev->driver)) {
			if ( (dev->bus != BUS_PCI && dev->bus != BUS_PCMCIA) || 
			     ((struct pciDevice *)dev)->pciType != PCI_CARDBUS) {
				addAlias(cf,dev->device,dev->driver,CM_REPLACE);
				writeConfModules(cf,_module_file);
				madebak = cf->madebackup;
				openlog("kudzu",0,LOG_USER);
				syslog(LOG_NOTICE,_("aliased %s as %s"),path,dev->driver);
				closelog();
			}
		}
		freeConfModules(cf);
		{
			FILE *f;
			char path1[256];
			
			snprintf(path, 256, "/etc/sysconfig/network-scripts/ifcfg-%s",dev->device);
			if (!access(path, F_OK)) {
				snprintf(path1, 256, "/etc/sysconfig/network-scripts/ifcfg-%s.bak",
					 dev->device);
				rename(path, path1);
			}
			f = fopen(path,"w");
			if (!f) break;
			fprintf(f,"# %s\nDEVICE=%s\nONBOOT=yes\nBOOTPROTO=dhcp\n",dev->desc,dev->device);
			if (dev->classprivate)
				fprintf(f,"HWADDR=%s\n",(char *)dev->classprivate);
			fclose(f);
			/* nasty hack - make sure device is right */
			rename_device_if_needed(dev);
		}
		break;
	 case CLASS_VIDEO:
		if (!dev->classprivate) break;
		snprintf(path,256,"/usr/bin/system-config-display --noui --set-driver=\"%s\"",(char *)dev->classprivate);
		configuredX = system(path);
		openlog("kudzu",0,LOG_USER);
		syslog(LOG_NOTICE,"ran system-config-display for %s",(char *)dev->classprivate);
		closelog();
		break;
	 case CLASS_AUDIO:
		if (!dev->driver) break;
		cf = readConfModules(_module_file);
		if (!cf)
			cf = newConfModules();
		cf->madebackup = madebak;
		if (
		    isAliased(cf,"sound-slot-",dev->driver)==-1 &&
		    isAliased(cf,"sound-card-",dev->driver)==-1 &&
		    isAliased(cf,"snd-card-",dev->driver)==-1
		    ) {
			snprintf(path,256,"snd-card-%d",dev->index);
			addAlias(cf,path,dev->driver,CM_REPLACE);
			snprintf(path,256,"options snd-card-%d index=%d",dev->index,dev->index);
			addLine(cf,path,CM_REPLACE);
			snprintf(path,256,"options %s index=%d",dev->driver,dev->index);
			addLine(cf,path,CM_REPLACE);
			snprintf(path,256,"remove %s { /usr/sbin/alsactl store %d >/dev/null 2>&1 || : ; }; /sbin/modprobe -r --ignore-remove %s",dev->driver, dev->index, dev->driver);
			addLine(cf,path,CM_REPLACE);
			writeConfModules(cf,_module_file);
			madebak = cf->madebackup;
			openlog("kudzu",0,LOG_USER);
			snprintf(path,256,"snd-card-%d",dev->index);
			syslog(LOG_NOTICE,_("aliased %s as %s"),path,dev->driver);
			closelog();
		}
		break;
	 case CLASS_KEYBOARD:
		if (!rewriteInittab(dev))
			system("[ -x /sbin/telinit -a -p /dev/initctl -a -f /proc/1/exe -a -d /proc/1/root ] && /sbin/telinit q >/dev/null 2>&1");
		if (isSerialish(dev))
			rewriteSecuretty(dev);
		break;
	 default:
		break;
	}
	return 0;
}

int unconfigure(struct device *dev)
{
	struct confModules *cf;
	char path[256], path1[256];
	char *tmpalias;
	int index,needed;
	
	switch (dev->type) {
	 case CLASS_NETWORK:
		if (!dev->driver || !dev->device) break;
		cf = readConfModules(_module_file);
		if (!cf)
		  cf = newConfModules();
		cf->madebackup = madebak;
		needed = 0;
		if (!isAliased(cf, dev->device, dev->driver)) {
			removeAlias(cf,dev->device,CM_REPLACE);
		}
		snprintf(path, 256, "/etc/sysconfig/network-scripts/ifcfg-%s",
			dev->device);
		if (!access(path, F_OK)) {
			snprintf(path1, 256, "/etc/sysconfig/network-scripts/ifcfg-%s.bak",
				 dev->device);
			rename(path, path1);
		}
		snprintf(path, 256, "/etc/sysconfig/networking/devices/ifcfg-%s",
			dev->device);
		if (!access(path, F_OK))
			unlink(path);
		writeConfModules(cf,_module_file);
		madebak = cf->madebackup;
		freeConfModules(cf);
		break;
	 case CLASS_AUDIO:
		if (!dev->driver) break;
		cf = readConfModules(_module_file);
		if (!cf)
		  cf = newConfModules();
		cf->madebackup = madebak;
		index = 0;
		while (1) {
			snprintf(path,256,"snd-card-%d",index);
			tmpalias=getAlias(cf,path);
			if (tmpalias && !strcmp(tmpalias,dev->driver)) {
				int x;
				
				needed = 0;
				
				for (x=0;currentDevs[x];x++) { 
					if (currentDevs[x]->driver &&
					    !strcmp(currentDevs[x]->driver,dev->driver)) {
						needed = 1;
						break;
					}
				}
				removeAlias(cf,path,CM_REPLACE);
				removeOptions(cf,path,CM_REPLACE);
				if (!needed) {
					snprintf(path,256,"options snd-card-%d index=%d",dev->index,dev->index);
					removeLine(cf,path,CM_REPLACE);
					snprintf(path,256,"remove %s { /usr/sbin/alsactl store >/dev/null 2>&1 || : ; }; /sbin/modprobe -r --ignore-remove %s",dev->driver, dev->driver);
					removeLine(cf,path,CM_REPLACE);
					
				}
			} else if (!tmpalias)
			  break;
			index++;
		}
		snprintf(path,256,"synth%d",index);
		tmpalias=getAlias(cf,path);
		if (tmpalias && !strcmp(tmpalias,dev->driver)) {
			int x;

			needed = 0;
				
			for (x=0;currentDevs[x];x++) { 
				if (currentDevs[x]->driver &&
				    !strcmp(currentDevs[x]->driver,dev->driver)) {
					needed = 1;
					break;
				}
			}
			if (!needed)
			  removeAlias(cf,path,CM_REPLACE);
		}
		writeConfModules(cf,_module_file);
		madebak = cf->madebackup;
		freeConfModules(cf);
		break;
	 default:
		break;
	}
	return 0;
}

void configMenu(struct device *oldDevs, struct device *newDevs, int runFirst)
{
	struct device *dev, *tmpdev;
	
	/* First, make sure we have work to do... */
	dev = oldDevs;
	for ( ; dev; dev=dev->next) {
		if (isConfigurable(dev) &&
		    !(dev->bus == BUS_PCI && !dev->driver)) {
			if (!dev->detached) {
				
				/* If the device only changed in the driver used, ignore it */
				tmpdev = newDevs;
				for ( ; tmpdev ; tmpdev = tmpdev->next) {
					if (tmpdev->compareDevice(tmpdev,dev) == 2) {
						oldDevs = listRemove(oldDevs,dev);
						newDevs = listRemove(newDevs,tmpdev);
						continue;
					}
				}
				continue;
			} else {
				/* Add detached devices to current list */
				currentDevs = realloc(currentDevs,(numCurrent+2)*sizeof(struct device *));
				currentDevs[numCurrent] = dev;
				currentDevs[numCurrent+1] = NULL;
				numCurrent++;
			}
		}
		oldDevs = listRemove(oldDevs, dev);
	}
	dev = newDevs;
	for ( ; dev; dev=dev->next) {
		if (isConfigurable(dev) &&
		    !(dev->bus == BUS_PCI && !dev->driver)) {
		  if (!runFirst || !isConfigured(dev)) {
			  switch (dev->type) {
			   case CLASS_NETWORK:
			   case CLASS_AUDIO:
				  if (isAvailable(dev->driver))
				    continue;
				  break;
			   default:
				  continue;
			  }

		  }
		}
		newDevs = listRemove(newDevs, dev);
	}
	if (!oldDevs && !newDevs)
	  return;
	
	dev = oldDevs;
	for ( ; dev ; dev = dev->next ) {
		unconfigure(dev);
	}
	dev = newDevs;
	for ( ; dev ; dev = dev->next) {
		configure(dev);
	}
	if (configuredX >= 0) {
		if (configuredX) {
			close(open("/var/run/Xconfig-failed",O_CREAT|O_EXCL,0644));
		} else {
			close(open("/var/run/Xconfig",O_CREAT|O_EXCL,0644));
		}
	}
}

static void setupKernelVersion() {
	unsigned int major, sub, minor;
	
	if (!kernel_ver) {
		struct utsname utsbuf;
		
		uname(&utsbuf);
		kernel_ver = strdup(utsbuf.release);
	}
	sscanf(kernel_ver, "%u.%u.%u", &major, &sub, &minor);
	_kernel_release = major + (float)(sub/10.0);
	if (_kernel_release >= 2.5)
		_module_file = "/etc/modprobe.conf";
	else
		_module_file = "/etc/modules.conf";
}

int main(int argc, char **argv) 
{
	char *confFile;
	char *debugFile=NULL;
	int runFirst=0;
	int ret;
	int rc;
	int x;
	char *bus = NULL, *class = NULL;
	enum deviceBus probeBus = BUS_UNSPEC & ~BUS_SERIAL;
	enum deviceClass probeClass = CLASS_UNSPEC;
	poptContext context;
	struct device **oldDevs, **newDevs;
	struct poptOption options[] = {
		POPT_AUTOHELP
		{ "safe", 's', POPT_ARG_NONE, &safe, 0,
		  _("do only 'safe' probes that won't disturb hardware"),
		  NULL
		},
		{ "timeout", 't', POPT_ARG_INT, &timeout, 0,
		  _("set timeout in seconds"), NULL
		},
		{ "probe", 'p', POPT_ARG_NONE, &probeonly, 0,
		  _("probe only, print information to stdout"),
			NULL
		},
		{ "bus", 'b', POPT_ARG_STRING, &bus, 0,
		  _("probe only the specified 'bus'"),
			NULL
		},
		{ "class", 'c', POPT_ARG_STRING, &class, 0,
			  _("probe only for the specified 'class'"),
			NULL
		},
		{ "file", 'f', POPT_ARG_STRING, &debugFile, 0,
			_("read probed hardware from a file"),
			_("file to read hardware info from")
		},
		{ "kernel", 'k', POPT_ARG_STRING, &kernel_ver, 0,
				_("search for modules for a particular kernel version"),
			_("kernel version")
		},
		{ "quiet", 'q',  POPT_ARG_NONE, &x, 0,
			_("do configuration that doesn't require user input"),
			NULL
		},
		{ 0, 0, 0, 0, 0, 0 }
	};

	setlocale(LC_ALL, "");
	bindtextdomain("kudzu", "/usr/share/locale");
	textdomain("kudzu");
	
	context = poptGetContext("kudzu", argc, (const char **)argv, options, 0);
	while ((rc = poptGetNextOpt(context)) > 0) {
	}
	if (( rc < -1)) {
		fprintf(stderr, "%s: %s\n",
			poptBadOption(context, POPT_BADOPTION_NOALIAS),
			poptStrerror(rc));
		exit(-1);
	}
	
	if (getuid() && !probeonly) {
		fprintf(stderr,
			_("\nERROR - You must be root to run kudzu.\n"));
		exit(1);
	}
 
	setlocale(LC_ALL, "C");
	
	setupKernelVersion();
	
	if (!(confFile=checkConfFile())) {
		runFirst=1;
	}
	
	if (bus) {
		for (x=0; bus[x]; x++)
		  bus[x] = toupper(bus[x]);
		for (x=0; buses[x].string && strcmp(buses[x].string,bus); x++);
		if (buses[x].string)
		  probeBus = buses[x].busType;
	}
	if (class) {
		for (x=0; class[x]; x++)
		  class[x] = toupper(class[x]);
		for (x=0; classes[x].string && strcmp(classes[x].string,class); x++);
		if (classes[x].string)
		  probeClass = classes[x].classType;
	}
	initializeBusDeviceList(probeBus);
	if (runFirst || probeonly) {
		storedDevs = malloc(sizeof(struct device *));
		storedDevs[0] = NULL;
			       
	} else {
		storedDevs = readDevices(confFile);
		if (!storedDevs) {
			storedDevs = malloc(sizeof(struct device *));
			storedDevs[0] = NULL;
		}
	}
	   
	while (storedDevs[numStored]) numStored++;
	if (debugFile)
	  currentDevs = readDevices(debugFile); 
	else {
		if (safe)
		  currentDevs = probeDevices(probeClass, probeBus, (PROBE_ALL|PROBE_SAFE));
		else
		  currentDevs = probeDevices(probeClass, probeBus, PROBE_ALL);
	}
	if (!currentDevs) {
		currentDevs = malloc(sizeof(struct device *));
		currentDevs[0] = NULL;
	}
	if (probeonly) {
		for (x=0; currentDevs[x]; x++)
		  currentDevs[x]->writeDevice(stdout, currentDevs[x]);
		freeDeviceList();
		exit(0);
	}
	while (currentDevs[numCurrent]) numCurrent++;
	ret = listCompare(storedDevs, currentDevs, &oldDevs, &newDevs);
	freeDeviceList();
	if (!ret) {
		writeDevices(confFile,currentDevs);
		exit(0);
	} else {
		/* List-ify oldDevs, newDevs */
		if (oldDevs[0]) {
			oldDevs[0]->next=NULL;
			for (x=1;oldDevs[x];x++)
				oldDevs[x-1]->next = oldDevs[x];
			oldDevs[x-1]->next = NULL;
		}
		if (newDevs[0]) {
			newDevs[0]->next = NULL;
			for (x=1;newDevs[x];x++) 
				newDevs[x-1]->next = newDevs[x];
			newDevs[x-1]->next = NULL;
		}
		setlocale(LC_ALL, "");
	        configMenu((*oldDevs),(*newDevs),runFirst);
	}
	if (!runFirst)
	  writeDevices(confFile,currentDevs);
	else
	  writeDevices("/etc/sysconfig/hwconf",currentDevs);
	
	return 0;
}
