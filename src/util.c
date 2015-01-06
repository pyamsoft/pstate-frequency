/*
 * pstate_frequency Easier control of the Intel p-state driver
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
 * For questions please contact pyamsoft at pyam.soft@gmail.com
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "src/util.h"

/*
 * The requested state of debugging. Initialized to 0 for OFF, 1 for ON, -1 for QUIET
 */
int32_t debug = 0;

/*
 * Check if a given string starts with a given prefix, return 1 for success and
 * 0 for failure
 */
int32_t str_starts_with(const char *restrict string, const char *restrict prefix)
{
        while (*prefix) {
                if (*prefix++ != *string++)
                        return 0;
        }
        return 1;
}

/*
 * Convert a given string into a decimal double value
 */
double str_to_num(char *const line)
{
        const double value = strtol(line, NULL, 10);
        free(line);
        return value;
}

/*
 * Guarantee that a given value is within the specified number range
 * Return the limits of each range if this is not the case
 */
double fix_value_range(const double value, const double min, const double max)
{
        if (value < min)
                return min;
        else if (value > max)
                return max;
        else
                return value;
}

/*
 * Check whether or not a string is entirely composed of digit values
 * Return a 1 for success and a 0 for failure
 */
int32_t string_is_digits(const char *const string)
{
        const int32_t length = strlen(string);
        for (int32_t i = 0; i < length; ++i) {
                if (!isdigit(string[i]))
                        return 0;
        }
        return 1;
}

/*
 * Logging based on the status of a debug flag
 * Used for messages which are not problematic occurences
 */
void log_debug(const char *const format, ...)
{
        if (debug == 1) {
                va_list arg;
                va_start(arg, format);
                vfprintf(stdout, format, arg);
                va_end (arg);
        }
}

/*
 * Logging based on the status of a debug flag
 * Used for messages which are problematic occurences
 */
void log_error(const char *const format, ...)
{
        if (debug == 1) {
                va_list arg;
                va_start(arg, format);
                vfprintf(stderr, format, arg);
                va_end (arg);
        }
}
