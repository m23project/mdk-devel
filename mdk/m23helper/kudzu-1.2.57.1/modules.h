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

#ifndef _MODULES_H_
#define _MODULES_H_

/* Handle stuff in modules.conf */

struct module {
	char *name;
	int loaded;
};

struct confModules {
	/* <numlines> lines. Lines may be NULL. */
	char **lines;
	int numlines;
	/* have we backed up an old entry?
	 * We only want to do this once per 'session'.
	 */
	int madebackup;
	
};

/* Flags to add/removeFoo */
#define	CM_REPLACE	1	/* replace matching lines */
#define CM_COMMENT	(1<<1)	/* comment out matching lines */

/* Returns an allocated (but empty) confModules */
struct confModules *newConfModules();
void freeConfModules(struct confModules *cf);
struct confModules *readConfModules(char *filename);
int writeConfModules(struct confModules *cf, char *filename);

int addLine(struct confModules *cf, char *line, int flags);
int addAlias(struct confModules *cf, char *alias, char *aliasdef, int flags);
int addOptions(struct confModules *cf, char *module, char *modopts, int flags);
int removeLine(struct confModules *cf, char *line, int flags);
int removeAlias(struct confModules *cf, char *alias, int flags);
int removeOptions(struct confModules *cf, char *module, int flags);
char *getAlias(struct confModules *cf, char *alias);

/* Returns:
 *  -1  if no alias is found
 *  0 if the exact alias found
 * <number> if a numbered alias is found
 * For example:
 *   isAliased(cf,"eth","tulip")
 * returns '2' if cf has
 *   alias eth2 tulip
 */
int isAliased(struct confModules *cf, char *alias, char *module);
int isLoaded(char *module);
int getLogLevel();
void setLogLevel(int level);
#endif
