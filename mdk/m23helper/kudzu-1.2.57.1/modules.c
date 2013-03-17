/* Copyright 1999-2004 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* Handle stuff in /etc/modprobe.conf. */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "modules.h"

static char *getLine(char **data) {
	char *x, *y;
	    
	if (!*data) return NULL;
	x=*data;
	while (*x && (*x != '\n')) x++;
	if (*x) {
		x++;
	} else {
		if (x-*data) {
			y=malloc(x-*data+1);
			y[x-*data] = 0;
			y[x-*data-1] = '\n';
			memcpy(y,*data,x-*data);
		} else {
			y=NULL;
		}
		*data = NULL;
		return y;
	}
	y = malloc(x-*data);
	y[x-*data-1] = 0;
	memcpy(y,*data,x-*data-1);
	*data = x;
	
	x = y+strlen(y)-1;
	while (isspace(*x)) x--;
	(*(x+1))='\0';
	return y;
}

static char **toArray(char *line, int *num) {
	/* Converts a long string into an array of lines. */
	char **lines;
	char *tmpline;
	int x, dup;
	*num = 0;
	lines = NULL;
	    
	while ((tmpline=getLine(&line))) {
		dup = 0;
		for (x=0;x<(*num);x++) {
			if (!strcmp(lines[x],tmpline)) dup=1;
		}
		
		if (!dup) {
			if (!*num)
			  lines = (char **) malloc(sizeof(char *));
			else
			  lines = (char **) realloc(lines, (*num+1)*sizeof(char *));
			lines[*num] = tmpline;
			(*num)++;
		}
	}
	return lines;
}

struct confModules *newConfModules()
{
	struct confModules *ret;
	
	ret=malloc(sizeof(struct confModules));
	ret->numlines=0;
	ret->lines=NULL;
	ret->madebackup = 0;
	return ret;
}

void freeConfModules(struct confModules *cf)
{
	int x;
	
	if (cf==NULL) {
		printf("freeConfModules called with NULL pointer. Don't do that.\n");
		abort();
	}
	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x]) free(cf->lines[x]);
	}
	free(cf->lines);
	free(cf);
}

struct confModules *readConfModules(char *filename)
{
	int fd,x,newlen;
	char *buf;
	struct stat sbuf;
	struct confModules *ret;
	char *tmp;
	
	if (!filename) return NULL;

	fd = open(filename,O_RDONLY);
	if (fd==-1) return NULL;
	stat(filename,&sbuf);
	buf=malloc(sbuf.st_size+1);
	if (!buf) return NULL;
	if (read(fd,buf,sbuf.st_size)!=sbuf.st_size) {
		close(fd);
		return NULL;
	}
	close(fd);
	buf[sbuf.st_size] = '\0';
	ret = malloc (sizeof(struct confModules));
	ret->lines = toArray(buf,&x);
	ret->numlines = x;
	/* Handle continued lines */
	for (x=0;x<ret->numlines;x++) {
		if (ret->lines[x]) 
			if (*(ret->lines[x]+strlen(ret->lines[x])-1)=='\\') {
			if (x+1<ret->numlines) {
				(*(ret->lines[x]+strlen(ret->lines[x])-1))='\0';
				newlen=strlen(ret->lines[x])+strlen(ret->lines[x+1])+2;
				tmp=malloc(newlen);
				snprintf(tmp,newlen,"%s %s",ret->lines[x],
						    ret->lines[x+1]);
				free(ret->lines[x]);
				free(ret->lines[x+1]);
				ret->lines[x]=tmp;
				ret->lines[x+1]=NULL;
			}
		}
	}
	ret->madebackup = 0;
	free(buf);
	return ret;
}


int writeConfModules(struct confModules *cf, char *filename)
{
	char fname[256];
	struct stat sbuf;
	int fd,x;
	
	if (!filename) return 1;
	if (!stat(filename,&sbuf) && cf->madebackup==0) {
		snprintf(fname,256,"%s~",filename);
		if (rename(filename,fname)) return 1;
	} 
	fd = open(filename,O_WRONLY|O_EXCL|O_CREAT,0644);
	if (fd==-1) return 1;
	
	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x]) {
			write(fd,cf->lines[x],strlen(cf->lines[x]));
			write(fd,"\n",1);
		}
	}
	close(fd);
	return 0;
}

int addLine(struct confModules *cf, char *line, int flags)
{
	int x;
	
        if (flags & CM_REPLACE || flags & CM_COMMENT)
	  removeLine(cf,line,flags);

	for (x=0;x<cf->numlines && cf->lines[x];x++);
	if (x!=cf->numlines)
	  cf->lines[x]=line;
	else {
		cf->numlines++;
		cf->lines = realloc(cf->lines,(cf->numlines)*sizeof(char *));
		cf->lines[cf->numlines-1] = strdup(line);
	}
	return 0;
}


int addAlias(struct confModules *cf, char *alias, char *aliasdef, int flags)
{
	int x;
	char *tmp;
	
        if (flags & CM_REPLACE || flags & CM_COMMENT)
	  removeAlias(cf,alias,flags);
        x=strlen(alias)+strlen(aliasdef)+10;
	tmp = malloc(x);
	snprintf(tmp,x,"alias %s %s",alias,aliasdef);
	addLine(cf,tmp,flags);
	return 0;
}

int addOptions(struct confModules *cf, char *module, char *modopts, int flags)
{
	int x;
	char *tmp;
	
        if (flags & CM_REPLACE || flags & CM_COMMENT)
	  removeOptions(cf,module,flags);
        x=strlen(module)+strlen(modopts)+12;
	tmp = malloc(x);
	snprintf(tmp,x,"options %s %s",module,modopts);
	addLine(cf,tmp,flags);
	return 0;
}

int removeLine(struct confModules *cf, char *line, int flags)
{
	int x;
	char *tmp;

	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x])
		  if (!strcmp(cf->lines[x],line)) {
			  if (flags & CM_COMMENT) {
				  tmp = malloc(strlen(cf->lines[x])+2);
				  snprintf(tmp,strlen(cf->lines[x])+2,"#%s",cf->lines[x]);
				  free(cf->lines[x]);
				  cf->lines[x] = tmp;
			  } else
			    cf->lines[x] = NULL;
		}
	}
	return 0;
}

int removeAlias(struct confModules *cf, char *alias, int flags)
{
	int x;
	char *tmp;

	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x])
		  if (!strncmp(cf->lines[x],"alias ",6)) {
			  tmp=cf->lines[x]+6;
			  while (isspace(*tmp)) tmp++;
			  if (!strncmp(tmp,alias,strlen(alias)) &&
			      isspace(*(tmp+strlen(alias)))) {
				  if (flags & CM_COMMENT) {
					  tmp = malloc(strlen(cf->lines[x])+2);
					  snprintf(tmp,strlen(cf->lines[x])+2,"#%s",cf->lines[x]);
					  free(cf->lines[x]);
					  cf->lines[x] = tmp;
				  } else
				    cf->lines[x] = NULL;
				}
		  }
	}
	return 0;
}

int removeOptions(struct confModules *cf, char *module, int flags)
{
	int x;
	char *tmp;

	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x])
		  if (!strncmp(cf->lines[x],"options ",8)) {
			  tmp=cf->lines[x]+8;
			  while (isspace(*tmp)) tmp++;
			  if (!strncmp(tmp,module,strlen(module)) &&
			      isspace(*(tmp+strlen(module)))) {
				  if (flags & CM_COMMENT) {
					  tmp = malloc(strlen(cf->lines[x])+2);
					  snprintf(tmp,strlen(cf->lines[x])+2,"#%s",cf->lines[x]);
					  free(cf->lines[x]);
					  cf->lines[x] = tmp;
				  } else
				    cf->lines[x] = NULL;
			  }
		  }
	}
	return 0;
}

char *getAlias(struct confModules *cf, char *alias)
{
	int x;
	char *tmp,*ret=NULL;

	for (x=0;x<cf->numlines;x++) {
		if (cf->lines[x])
		  if (!strncmp(cf->lines[x],"alias ",6)) {
			  tmp=cf->lines[x]+6;
			  while (isspace(*tmp)) tmp++;
			  if (!strncmp(tmp,alias,strlen(alias)) &&
			      isspace(*(tmp+strlen(alias)))) {
				  ret = malloc(strlen(cf->lines[x]));
				  tmp=tmp+strlen(alias);
				  while(isspace(*tmp)) tmp++;
				  strncpy(ret,tmp,strlen(cf->lines[x]));
			  }
		  }
	}
	return ret;
}

int isAliased(struct confModules *cf, char *alias, char *module)
{
	char tmp[128];
	char *modalias;
	int x=0,retval=-1;
	
	if ( (modalias=getAlias(cf,alias)) && (!strcmp(module,modalias)))
	  retval = 0;
	else
	  while (1) {
		snprintf(tmp,128,"%s%d",alias,x);
		if ( (modalias=getAlias(cf,tmp)) && (!strcmp(module,modalias))) {
			retval=x;
			break;
		}
		if (!modalias && x!=0) break;
		x++;
	  }
	return retval;
}

int getLogLevel() {
#define CTL_KERN	1
#define KERN_PRINTK	23
	int sysctl_name[] = { CTL_KERN, KERN_PRINTK };
	int args[10];
	size_t len = sizeof(args);
	
	sysctl(sysctl_name, sizeof(sysctl_name)/sizeof(sysctl_name[0]),
	       args, &len, 0, 0);
	return args[0];
}

void setLogLevel(int level) {
	syscall(SYS_syslog,8,NULL,level);
}

#define QM_INFO 5
struct module_info
{
	unsigned long addr;
	unsigned long size;
	unsigned long flags;
	long usecount;
};

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

#ifdef TESTING
int main() {
	struct confModules *foo;
	int x;
	
	foo=newConfModules();
	addAlias(foo,"eth0","eepro100",CM_REPLACE);
	addAlias(foo,"eth0","tulip",CM_COMMENT);
	addOptions(foo,"eth0","debug=foo",0);
	addLine(foo,"alias parport_lowlevel parport_pc",CM_COMMENT);
	writeConfModules(foo,"foomod");
	printf("=%s=\n",getAlias(foo,"eth0"));
	printf("=%s=\n",getAlias(foo,"scuzzy_hostadapter"));
	freeConfModules(foo);
}
	
#endif
