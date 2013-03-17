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


#ifndef _KUDZU_INTERNAL_H_
#define _KUDZU_INTERNAL_H_

char *__bufFromFd(int fd);
char *__readRaw(char *name);
char *__readString(char *name);
int __readHex(char *name);
int __readInt(char *name);
int __getSysfsDevice(struct device *dev, char *path, char *type, int return_multiple);
int __getNetworkAddr(struct device *dev, char *devname);
#endif
