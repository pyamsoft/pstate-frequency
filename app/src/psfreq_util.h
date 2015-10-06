/**
 * @file psfreq_util.h
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 * Utility functions file to help with things like reading from command
 * pipes.
 */

#ifndef PSFREQ_UTIL_H
#define PSFREQ_UTIL_H

#include <stdlib.h>

char **psfreq_util_read_pipe(const char *const cmd, const unsigned char *size);
char *psfreq_util_read(const char *abs_path);
char *psfreq_util_read2(const char *base, const char *file);
unsigned char psfreq_util_write(const char *abs_path, const char *buf);
unsigned char psfreq_util_write2(const char *base, const char *file,
                                                   const char *buf);
unsigned char psfreq_util_write_num(const char *abs_path, const int *num);
unsigned char psfreq_util_write_num2(const char *base, const char *file,
                                                   const int *num);

#endif
