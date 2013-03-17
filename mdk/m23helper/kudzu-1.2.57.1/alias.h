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

#ifndef _KUDZU_ALIAS_H
#define _KUDZU_ALIAS_H

#include <string.h>

struct alias {
	struct alias *next;
	char *match;
	char *module;
};

struct aliaslist {
	struct aliaslist *next;
	char *bus;
	struct alias *alias;
};

void freeAliasList(struct aliaslist *list);
struct aliaslist *readAliases(struct aliaslist *alist, char *filename, char *bustype);
char *aliasSearch(struct aliaslist *list, char *bus, char *device);

static inline struct aliaslist *getAliases(struct aliaslist *list, char *bus) {
	while (list && strcmp(bus,list->bus))
		list = list->next;
	return list;
}

#endif
