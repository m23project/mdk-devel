/* Copyright 2005 Red Hat, Inc.
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "alias.h"
#include "kudzu.h"

#include "kudzuint.h"

#include <fcntl.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline void addAliasToList(struct aliaslist *list, char *match, char *module) {
	struct alias *entry = malloc(sizeof (struct alias));
	entry->match = strdup(match);
	entry->module = strdup(module);

	entry->next = list->alias;
	list->alias = entry;
}

static inline struct aliaslist *addAliasList(struct aliaslist *list, char *bus) {
	struct aliaslist *entry = malloc(sizeof (struct alias));
	entry->bus = strdup(bus);
	entry->alias = NULL;
	entry->next = list;
	return entry;
}

void freeAliasList(struct aliaslist *list) {
	while (list) {
		struct aliaslist *tmplist = list;

		list = list->next;
		free(tmplist->bus);
		while (tmplist->alias) {
			struct alias *tmp = tmplist->alias;
		
			tmplist->alias = tmplist->alias->next;
			free(tmp->match);
			free(tmp->module);
			free(tmp);
		}
	}
}

struct aliaslist *readAliases(struct aliaslist *alist, char *filename, char *bustype) {
	int fd;
	
	char *b, *buf, *path = NULL;
	
	if (filename) {
		fd = open(filename, O_RDONLY);
		if (fd < 0)
			return alist;
	} else {
		asprintf(&path,"/lib/modules/%s/modules.alias", kernel_ver);
		fd = open(path, O_RDONLY);
		if (fd < 0) {
			fd = open("/modules/modules.alias", O_RDONLY);
			if (fd < 0) {
				fd = open("./modules.alias", O_RDONLY);
				if (fd < 0) {
					free(path);
					return alist;
				}
			} 
		}
		free(path);
	}
	b = buf = __bufFromFd(fd);
	if (!buf) return alist;
	
	while (buf && *buf) {
		char *bus, *alias, *module, *comment;
		bus = buf;
		buf = strchr(buf,'\n');
		if (buf) {
			*buf = '\0';
			buf++;
		}
		if (!strncmp(bus, "alias ",6)) {
			bus += 6;
			
			if ((alias = strchr(bus,':'))) {
				struct aliaslist *list;

				*alias = '\0';
				alias++;
				module = strchr(alias,' ');
				*module = '\0';
				module++;

                                if ((comment = strpbrk(module, " \t")) != NULL)
                                   *comment = '\0';

				if (!bustype || !strcmp(bustype,bus)) {
					list = getAliases(alist, bus);
					if (!list) {
						list = addAliasList(alist, bus);
						alist = list;
					}
					addAliasToList(list, alias, module);
				}
			}
		}
	}
	free(b);
	return alist;
}

char *aliasSearch(struct aliaslist *list, char *bus, char *device) {
	while (list) {
		if (!strcmp(list->bus, bus)) {
			struct alias *alias = list->alias;
			
			while (alias) {
				if (!fnmatch(alias->match, device, 0)) {
					return alias->module;
				}
				alias = alias->next;
			}
		}
		list = list->next;
	}
	return NULL;
}
