/*
 * Command line handling of dmidecode
 * This file is part of the dmidecode project.
 *
 *   (C) 2005 Jean Delvare <khali@linux-fr.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

struct opt
{
	const char* devmem;
	unsigned int flags;
	u8 *type;
	u8 string_type;
	u8 string_offset;
};
extern struct opt opt;

#define FLAG_VERSION            (1<<0)
#define FLAG_HELP               (1<<1)
#define FLAG_DUMP               (1<<2)
#define FLAG_QUIET              (1<<3)

int parse_command_line(int argc, char * const argv[]);
void print_help(void);
