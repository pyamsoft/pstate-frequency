/**
 * @file psfreq_util.h
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
 * Utility functions file to help with things like reading from command
 * pipes.
 */

#ifndef PSFREQ_UTIL_H
#define PSFREQ_UTIL_H

/*
 * NULL is defined as 0, just hard code it here.
 * No need to include the entire stdlib.h
 */
#define READ_ERROR              0
#define STRING_STRIP_ERROR      0
#define WRITE_SUCCESS           1
#define WRITE_FAILURE           0

/**
 * Read a number of lines from a pipe
 *
 * @param cmd The command which the pipe will be opened from
 * @param size Number of lines to read from the pipe
 * @return Vector containing the contents read from the pipe
 */
char **psfreq_util_read_pipe(const char *const cmd, const unsigned int size);

/**
 * Read a single line buffer from a file
 * @param base The base path of the file to read from
 * @param file The relative path from the base of the file to read from
 * @return The stripped single line read from the file, NULL if otherhwise
 */
char *psfreq_util_read(const char *abs_path);

/**
 * Read a single line buffer from a file
 * @param abs_path The absolute path of the file to read from
 * @return The stripped single line read from the file, NULL if otherhwise
 */
char *psfreq_util_read2(const char *base, const char *file);

/**
 * Write a buffer to a file
 * @param abs_path The absolute path of the file to write to
 * @param buf Buffer to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write(const char *abs_path, const char *buf);

/**
 * Write a buffer to a file
 * @param base The base path of the file to write to
 * @param file The relative path from the base of the file to write to
 * @param buf Buffer to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write2(const char *base, const char *file,
                const char *buf);

/**
 * Write a number to a file
 * @param abs_path The absolute path of the file to write to
 * @param num Number value to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write_num(const char *abs_path, const int *num);

/**
 * Write a number to a file
 * @param base The base path of the file to write to
 * @param file The relative path from the base of the file to write to
 * @param num Number value to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write_num2(const char *base, const char *file,
                const int *num);

#endif
