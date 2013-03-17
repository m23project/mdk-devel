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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/types.h>

#include "kudzu.h"
#include "modules.h"

struct listent {
	char *name;
	enum deviceClass type;
	char **newnames;
	struct listent *next;
};

struct listent *modlist = NULL;

int readModlist() {
	int fd, i;
	struct stat sbuf;
	struct listent *tmpmod, *curmod;
	char *buf, *ptr, *next;
	
	curmod = modlist;
	fd = open("/usr/share/hwdata/upgradelist", O_RDONLY);
	if (fd < 0) {
		fd = open("./upgradelist", O_RDONLY);
		if (fd < 0) 
		  return 1;
	}
	fstat(fd, &sbuf);
	buf = alloca(sbuf.st_size+1);
	if (read(fd, buf, sbuf.st_size)!=sbuf.st_size) {
		close(fd);
		return 1;
	}
	close(fd);
	buf[sbuf.st_size] = '\0';
	while (buf && *buf) {
		int num = 0;
		
		next = buf;
		while (*next && *next != '\n') next++;
		if (*next) {
			*next = '\0';
			next++;
		}
		if (!*buf || *buf == '#') {
			buf = next;
			continue;
		}
		ptr = buf;
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (*ptr && isspace(*ptr)) ptr++;
		tmpmod = malloc(sizeof(struct listent));
		tmpmod->name = strdup(buf);
		buf = ptr;
		while (*ptr && !isspace(*ptr)) ptr++;
		if (*ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (*ptr && isspace(*ptr)) ptr++;
		for (i=0; classes[i].string && strcmp(classes[i].string,buf); i++);
		if (classes[i].string)
		  tmpmod->type = classes[i].classType;
		else
		  tmpmod->type = CLASS_OTHER;
		tmpmod->next = NULL;
		tmpmod->newnames = NULL;
		buf = ptr;
		do {
			while (*ptr && !isspace(*ptr) && *ptr != '/') ptr++;
			if (isspace(*ptr) || *ptr == '/') {
				*ptr = '\0';
				ptr++;
			}
			tmpmod->newnames = realloc(tmpmod->newnames,
						   (num+2)* sizeof(char *));
			tmpmod->newnames[num] = strdup(buf);
			tmpmod->newnames[num+1] = NULL;
			num++;
			buf = ptr;
		} while (*ptr);
		if (curmod) {
			tmpmod->next = curmod;
		}
		curmod = tmpmod;
		buf = next;
	}
	modlist = curmod;
	return 0;
}

int main(int argc, char **argv) {
	struct listent *l;
	struct confModules *cf;
	struct device **devs;
	struct device *tmpdev;
	char alias[64];
	char *aliasfoo = NULL;
	int rc, x, i, changed = 0;

	readModlist();
	
	if (argc >= 2)
		kernel_ver = argv[1];
	
	cf = readConfModules("/etc/modprobe.conf");
	if (!cf)
	  return 0;
        /* thrown in here for lack of any place better. */
	if ((aliasfoo = getAlias(cf, "sound"))) {
		removeAlias(cf,"sound",CM_REPLACE);
		addAlias(cf,"snd-card-0", aliasfoo, CM_REPLACE);
		changed++;
	}
	for (x = 0; ; x++) {
		char path[256];
		snprintf(alias, 64, "sound-slot-%d", x);
		aliasfoo = getAlias(cf,alias);
		if (!aliasfoo)
			break;
		removeAlias(cf,alias,CM_REPLACE);
		snprintf(alias, 64, "snd-card-%d",x);
		addAlias(cf,alias, aliasfoo, CM_REPLACE);
		snprintf(path,256,"options snd-card-%d index=%d",x,x);
		addLine(cf,path,CM_REPLACE);
		snprintf(path,256,"options %s index=%d",aliasfoo,x);
		addLine(cf,path,CM_REPLACE);
		snprintf(path,256,"remove %s { /usr/sbin/alsactl store %d >/dev/null 2>&1 || : ; }; /sbin/modprobe -r --ignore-remove %s",aliasfoo, x, aliasfoo);
		addLine(cf,path,CM_REPLACE);
		snprintf(path,256,"post-install sound-slot-%d /bin/aumix-minimal -f /etc/.aumixrc -L >/dev/null 2>&1 || :", x);
		removeLine(cf,path,CM_REPLACE);
		snprintf(path,256,"pre-remove sound-slot-%d /bin/aumix-minimal -f /etc/.aumixrc -S >/dev/null 2>&1 || :", x);
		removeLine(cf,path,CM_REPLACE);
		snprintf(path,256,"install %s /sbin/modprobe --ignore-install %s && /usr/sbin/alsactl restore >/dev/null 2>&1 || :", aliasfoo, aliasfoo);
		removeLine(cf,path,CM_REPLACE);
		openlog("kudzu",0,LOG_USER);
		syslog(LOG_NOTICE,"changed sound-slot-%d alias to snd-card-%d, adjusted mixer calls", x, x);
		closelog();
		changed++;
	}
	if ((aliasfoo = getAlias(cf, "midi"))) {
		removeAlias(cf,"midi",CM_REPLACE);
		addAlias(cf,"synth0", aliasfoo, CM_REPLACE);
		changed++;
	}
	devs = probeDevices(CLASS_UNSPEC, BUS_PCI|BUS_ISAPNP, (PROBE_ALL|PROBE_SAFE));
	if (!devs) {
		exit(0);
	}
	if (devs[0]) {
		devs[0]->next = NULL;
		for (x = 1; devs[x]; x++)
		  devs[x-1]->next = devs[x];
		devs[x-1]->next = NULL;
	}
	for (l=modlist; l ; l = l->next) {
		x = -1;
		switch (l->type) {
		 case CLASS_NETWORK:
			while ( (rc = isAliased(cf,"eth",l->name)) != -1) {
				if (x == rc) break;
				x = rc;
				snprintf(alias, 64, "eth%d",x);
				for (tmpdev = devs[0]; tmpdev; tmpdev = tmpdev->next) {
				  if (!tmpdev->driver) continue;
				  for (i=0; l->newnames[i]; i++)
				    if (!strcmp(l->newnames[i], tmpdev->driver)) {
					    addAlias(cf,alias, tmpdev->driver, CM_REPLACE);
					    changed++;
					    openlog("kudzu",0,LOG_USER);
					    syslog(LOG_NOTICE,"changed %s alias from %s to %s",
						   alias,l->name, tmpdev->driver);
					    closelog();
				    }
				}
			}
			break;
		 case CLASS_SCSI:
		 case CLASS_ATA:
		 case CLASS_SATA:
			while ( (rc = isAliased(cf,"scsi_hostadapter",l->name)) != -1) {
				if (x == rc) break;
				x = rc;
				if (x)
				  snprintf(alias, 64, "scsi_hostadapter%d",x);
				else
				  snprintf(alias, 64, "scsi_hostadapter");
				for (tmpdev = devs[0]; tmpdev; tmpdev = tmpdev->next) {
				  if (!tmpdev->driver) continue;
				  for (i=0; l->newnames[i]; i++)
				    if (!strcmp(l->newnames[i], tmpdev->driver)) {
					    addAlias(cf,alias, tmpdev->driver, CM_REPLACE);
					    changed++;
					    openlog("kudzu",0,LOG_USER);
					    syslog(LOG_NOTICE,"changed %s alias from %s to %s",
						   alias,l->name, tmpdev->driver);
					    closelog();
				    }
				}
			}
			break;
		 case CLASS_AUDIO:
			while ( (rc = isAliased(cf,"snd-card-",l->name)) != -1) {
				if (x == rc) break;
				x = rc;
				snprintf(alias, 64, "snd-card-%d",x);
				for (tmpdev = devs[0]; tmpdev; tmpdev = tmpdev->next) {
				  if (!tmpdev->driver) continue;
				  for (i=0; l->newnames[i]; i++)
				    if (!strcmp(l->newnames[i], tmpdev->driver)) {
					    addAlias(cf,alias, tmpdev->driver, CM_REPLACE);
					    changed++;
					    openlog("kudzu",0,LOG_USER);
					    syslog(LOG_NOTICE,"changed %s alias from %s to %s",
						   alias,l->name, tmpdev->driver);
					    closelog();
				    }
				}
			}
			break;
		 case CLASS_USB:
			while ( (rc = isAliased(cf,"usb-controller",l->name)) != -1) {
				if (x == rc) break;
				x = rc;
				if (x)
				  snprintf(alias, 64, "usb-controller%d",x);
				else
				  snprintf(alias, 64, "usb-controller");
				for (tmpdev = devs[0]; tmpdev; tmpdev = tmpdev->next) {
				  if (!tmpdev->driver) continue;
				  for (i=0; l->newnames[i]; i++)
				    if (!strcmp(l->newnames[i], tmpdev->driver)) {
					    addAlias(cf,alias, tmpdev->driver, CM_REPLACE);
					    changed++;
					    openlog("kudzu",0,LOG_USER);
					    syslog(LOG_NOTICE,"changed %s alias from %s to %s",
						   alias,l->name, tmpdev->driver);
					    closelog();
				    }
				}
			}
			break;
		 default:
			break;
		}
	}

	if (changed) {
		writeConfModules(cf, "/etc/modprobe.conf");
	}
	return 0;
}
