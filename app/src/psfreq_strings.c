/**
 * @file psfreq_strings.c
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

#define _GNU_SOURCE
#include <stdio.h>

#include "psfreq_color.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"

/**
 * Concatinate two strings together
 *
 * @param s1 Base string that will be concatinated upon
 * @param s2 String that will be appended to s1
 * @return String which is the combination of s1 and s2
 */
char *psfreq_strings_concat(const char *s1, const char *s2)
{
        char *result;
        /* asprintf returns -1 on failure */
        if (asprintf(&result, "%s%s", s1, s2) < 0) {
                psfreq_log_error("psfreq_strings_concat",
                        "asprintf returned a -1, indicating a failure during\n"
                        "either memory allocation or some other error.");
        }
        return result;
}

