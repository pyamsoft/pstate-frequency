/**
 * @file psfreq_strings.h
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
#ifndef PSFREQ_STRINGS_H
#define PSFREQ_STRINGS_H

/*
 * NULL is defined as 0, just hard code it here.
 * No need to include the entire stdlib.h
 */
#define NULL_CHAR                       '\0'
#define VASPRINTF_FAILURE               -1
#define STRING_CONCAT_ERROR             0
#define STRING_CONVERT_TO_ERROR         -1
#define STRING_CONVERT_FROM_ERROR       0
#define STRING_COMPARE_SUCCESS          1
#define STRING_COMPARE_FAILURE          0

/**
 * Concatinate two strings together
 *
 * @param s1 Base string that will be concatinated upon
 * @param s2 String that will be appended to s1
 * @return String which is the combination of s1 and s2
 */
char *psfreq_strings_concat(const char *const s1, const char *const s2);

/**
 * Checks that the string s starts with the same characters as the string p
 *
 * @param s1 The string to compare against
 * @param s2 The prefix to compare with
 * @return Boolean, true if string s has prefix p, false if otherwise
 */
unsigned char psfreq_strings_starts_with(const char *s, const char *p);

/**
 * Checks that the string s and p are equivalent in contents
 *
 * @param s1 First string
 * @param s2 Second string
 * @return Boolean, true if string s and p have equivalent contents,
 * false if otherwise
 */
unsigned char psfreq_strings_equals(const char *s, const char *p);

/**
 * Cast a string to integer value
 *
 * @param s String to cast
 * @return Integer value of casted string, -1 if failure
 */
int psfreq_strings_to_int(const char *const s);

/**
 * Cast a string to long integer value
 *
 * @param s Sting to cast
 * @return Long integer value of casted string, -1 if failure
 */
long psfreq_strings_to_long(const char *const s);

/**
 * Allocate a string representation of an integer value
 *
 * @param i Integer to cast into a string
 * @return String representation of integer, NULL otherwise
 */
char *psfreq_strings_from_int(const int *const i);

/**
 * A simple version of asprintf
 *
 * @param strp String pointer to store result into
 * @param fmt Formatting string
 * @param ... Arguments to fmt string
 * @return Integer return value of vasprintf
 */
int psfreq_strings_asprintf(char **strp, const char *fmt, ...);

#endif

