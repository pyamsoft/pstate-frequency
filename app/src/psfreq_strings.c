/**
 * @file psfreq_strings.c
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
 *
 *  Copyright (C) 2014 - 2016 Peter Kenji Yamanaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 * A couple of general purpose functions which can be applied to various
 * character array string like structures in C.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psfreq_color.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"

static const unsigned int RADIX_DECIMAL = 10;

/**
 * A simple version of vasprintf
 *
 * @param strp String pointer to store result into
 * @param fmt Formatting string
 * @param ap Arguments to fmt string
 * @return Integer return value of vasprintf
 */
static int psfreq_strings_vasprintf(char **strp, const char *fmt, va_list ap);

/**
 * Cast a string to a double value
 *
 * @param s String to cast to double
 * @return Double representation of string contents
 */
static double psfreq_strings_to_double(const char *const s);

/**
 * Checks that a string is composed of only digit characters
 *
 * @param s String to cast to double
 * @return boolean representation, true if string is only digit chars, false
 * if otherwise
 */
static unsigned char psfreq_strings_isdigits(const char *s);

/**
 * Concatinate two strings together
 *
 * @param s1 Base string that will be concatinated upon
 * @param s2 String that will be appended to s1
 * @return String which is the combination of s1 and s2
 */
char *psfreq_strings_concat(const char *const s1, const char *const s2)
{
        char *result = NULL;
        if (psfreq_strings_asprintf(&result, "%s%s", s1, s2) < 0) {
                psfreq_log_error("psfreq_strings_concat",
                        "asprintf returned a -1, indicating a failure during\n"
                        "either memory allocation or some other error.");
                return STRING_CONCAT_ERROR;
        }
        return result;
}

/**
 * Checks that the string s starts with the same characters as the string p
 *
 * @param s1 The string to compare against
 * @param s2 The prefix to compare with
 * @return Boolean, true if string s has prefix p, false if otherwise
 */
unsigned char psfreq_strings_starts_with(const char *s, const char *p)
{
        char c1 = *s;
        char c2 = *p;
        const size_t size = strlen(s);
        const size_t ssize = strlen(p);
        if (size < ssize) {
                psfreq_log_debug("psfreq_strings_starts_with",
                                "Comparison string larger than test string");
                return STRING_COMPARE_FAILURE;
        }
        psfreq_log_debug("psfreq_strings_starts_with",
                        "Check if string '%s' starts with '%s'", s, p);
        while (c2 != NULL_CHAR) {
                if (c1 != c2) {
                        return STRING_COMPARE_FAILURE;
                }
                p++;
                s++;
                c1 = *s;
                c2 = *p;
        }
        return STRING_COMPARE_SUCCESS;
}

/**
 * Checks that the string s and p are equivalent in contents
 *
 * @param s1 First string
 * @param s2 Second string
 * @return Boolean, true if string s and p have equivalent contents,
 * false if otherwise
 */
unsigned char psfreq_strings_equals(const char *s, const char *p)
{
        const size_t size = strlen(s);
        const size_t ssize = strlen(p);
        if (size != ssize) {
                psfreq_log_debug("psfreq_strings_equals",
                                "Different size strings");
                return STRING_COMPARE_FAILURE;
        }
        psfreq_log_debug("psfreq_strings_equals",
                        "Check if string '%s' is '%s'", s, p);

        return (strncmp(s, p, size) == 0);
}

/**
 * Cast a string to integer value
 *
 * @param s Sting to cast
 * @return Integer value of casted string, -1 if failure
 */
int psfreq_strings_to_int(const char *const s)
{
        return (int) psfreq_strings_to_double(s);
}

/**
 * Cast a string to long integer value
 *
 * @param s String to cast
 * @return Long integer value of casted string, -1 if failure
 */
long psfreq_strings_to_long(const char *const s)
{
        return (long) psfreq_strings_to_double(s);
}

/**
 * Allocate a string representation of an integer value
 *
 * @param i Integer to cast into a string
 * @return String representation of integer, NULL otherwise
 */
char *psfreq_strings_from_int(const int *const i)
{
        char *buf = NULL;
        if (psfreq_strings_asprintf(&buf, "%d", *i) < 0) {
                psfreq_log_error("psfreq_strings_from_int",
                        "asprintf returned a -1, indicating a failure during\n"
                        "either memory allocation or some other error.");
                return STRING_CONVERT_FROM_ERROR;
        }
        return buf;
}

/**
 * A simple version of asprintf
 *
 * @param strp String pointer to store result into
 * @param fmt Formatting string
 * @param ... Arguments to fmt string
 * @return Integer return value of vasprintf
 */
int psfreq_strings_asprintf(char **strp, const char *fmt, ...)
{
        int ret;
        va_list ap;

        va_start(ap, fmt);
        ret = psfreq_strings_vasprintf(strp, fmt, ap);
        va_end(ap);

        return ret;
}

/**
 * A simple version of vasprintf
 *
 * @param strp String pointer to store result into
 * @param fmt Formatting string
 * @param ap Arguments to fmt string
 * @return Integer return value of vasprintf
 */
static int psfreq_strings_vasprintf(char **strp, const char *fmt, va_list ap)
{
        va_list aap;
        size_t needed_size;
        char *buf;

        va_copy(aap, ap);
        needed_size = vsnprintf(NULL, 0, fmt, aap) + 1;
        va_end(aap);

        buf = malloc(needed_size);
        if (buf == NULL) {
                psfreq_log_error("psfreq_strings_vasprintf",
                                "Error allocating memory for string");
                return VASPRINTF_FAILURE;
        }

        *strp = buf;

        return vsnprintf(buf, needed_size, fmt, ap);
}

/**
 * Cast a string to a double value
 *
 * @param s String to cast to double
 * @return Double representation of string contents
 */
static double psfreq_strings_to_double(const char *const s)
{
        const unsigned char numeric = psfreq_strings_isdigits(s);
        if (numeric == STRING_COMPARE_FAILURE) {
                return STRING_CONVERT_TO_ERROR;
        }

        const double v = strtol(s, NULL, RADIX_DECIMAL);
        psfreq_log_debug("psfreq_strings_to_double",
                        "Convert string '%s' to double value", s);
        return v;
}

/**
 * Checks that a string is composed of only digit characters
 *
 * @param s String to cast to double
 * @return boolean representation, true if string is only digit chars, false
 * if otherwise
 */
static unsigned char psfreq_strings_isdigits(const char *s)
{
        if (s == NULL) {
                psfreq_log_error("psfreq_strings_isdigits",
                                "String is NULL");
                return STRING_COMPARE_FAILURE;
        }
        char c = *s;
        while (c != NULL_CHAR) {
                if (isdigit(c) == STRING_COMPARE_FAILURE) {
                        psfreq_log_error("psfreq_strings_isdigits",
                                        "String is not all digits");
                        return STRING_COMPARE_FAILURE;
                }
                s++;
                c = *s;
        }
        return STRING_COMPARE_SUCCESS;
}
