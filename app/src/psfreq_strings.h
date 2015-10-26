/**
 * @file psfreq_strings.h
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
 * A couple of general purpose functions which can be applied to various
 * character array string like structures in C.
 */
#ifndef PSFREQ_STRINGS_H
#define PSFREQ_STRINGS_H

#define RADIX_DECIMAL   10

#include <string.h>
#include <stdarg.h>

char *psfreq_strings_concat(const char *const s1, const char *const s2);
unsigned char psfreq_strings_starts_with(const char *s, const char *p);
unsigned char psfreq_strings_equals(const char *s, const char *p);
int psfreq_strings_to_int(const char *const s);
unsigned int psfreq_strings_to_uint(const char *const s);
char *psfreq_strings_from_int(const int *const i);
int psfreq_strings_asprintf(char **strp, const char *fmt, ...);

#endif

