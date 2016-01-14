/**
 * @file psfreq_util.c
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
/*
 * For popen using C99
 */
#define _POSIX_C_SOURCE 2
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "psfreq_log.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

static const char NEWLINE_CHAR = '\n';

/**
 * Strip the newline from the end of a string if it contains one
 *
 * @param s String to strip newline from
 * @return The string with newline stripped
 */
static char *psfreq_util_strip_string_end(char *s);

/**
 * Read a number of lines from a pipe
 *
 * @param cmd The command which the pipe will be opened from
 * @param size Number of lines to read from the pipe
 * @return Vector containing the contents read from the pipe
 */
char **psfreq_util_read_pipe(const char *const cmd, const unsigned int size)
{
        char **lines;
        FILE *pipe;
        unsigned int i;
        const size_t n = 20;
        if (cmd == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "cmd is NULL");
                return READ_ERROR;
        }
        if (size == 0) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Size is 0, which results in empty array");
                return READ_ERROR;
        }

        lines = malloc(size * sizeof(char *));
        if (lines == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Failed to malloc for lines");
                return READ_ERROR;
        }

        pipe = popen(cmd, "r");
        if (pipe == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Failed to open pipe '%s'", cmd);
                free(lines);
                return READ_ERROR;
        }
        for (i = 0; i < size; ++i) {
                char line[n];
                if (fgets(line, n, pipe) == NULL) {
                        psfreq_log_error("psfreq_util_read_pipe",
                                        "Failed to read from pipe");
                        free(lines);
                        pclose(pipe);
                        return READ_ERROR;
                } else {
                        psfreq_log_debug("psfreq_util_read_pipe",
                                "Assign line '%s' to array %d", line, i);
                        lines[i] = psfreq_util_strip_string_end(line);
                        if (lines[i] == STRING_STRIP_ERROR) {
                                psfreq_log_error("psfreq_util_read_pipe",
                                        "Failed to strip string %d", i);
                        }
                }
        }
        pclose(pipe);
        return lines;
}

/**
 * Read a single line buffer from a file
 * @param base The base path of the file to read from
 * @param file The relative path from the base of the file to read from
 * @return The stripped single line read from the file, NULL if otherhwise
 */
char *psfreq_util_read2(const char *base, const char *file)
{
        char *r;
        char *abs_path;
        abs_path = psfreq_strings_concat(base, file);
        if (abs_path == STRING_CONCAT_ERROR) {
                return READ_ERROR;
        }
        r = psfreq_util_read(abs_path);
        free(abs_path);
        return r;
}

/**
 * Read a single line buffer from a file
 * @param abs_path The absolute path of the file to read from
 * @return The stripped single line read from the file, NULL if otherhwise
 */
char *psfreq_util_read(const char *abs_path)
{
        FILE *f;
        const size_t n = 20;
        char line[n];
        char *stripped;

        f = fopen(abs_path, "r");
        if (f == NULL) {
                psfreq_log_error("psfreq_util_read",
                                "File '%s' failed to open for reading.",
                                abs_path);
                return READ_ERROR;
        }

        if (fgets(line, n, f) == NULL) {
                psfreq_log_error("psfreq_util_read",
                                "Failed to read buffer from file '%s'.",
                                abs_path);
                fclose(f);
                return READ_ERROR;
        }

        fclose(f);
        stripped = psfreq_util_strip_string_end(line);
        if (stripped == STRING_STRIP_ERROR) {
                psfreq_log_error("psfreq_util_read",
                        "Failed to strip line from file");
        }
        return stripped;
}

/**
 * Write a buffer to a file
 * @param abs_path The absolute path of the file to write to
 * @param buf Buffer to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write(const char *abs_path, const char *buf)
{
        FILE *f;
        if (buf == NULL) {
                psfreq_log_error("psfreq_util_write",
                                "buf is NULL, exit.");
                return WRITE_FAILURE;
        }

        f = fopen(abs_path, "w");
        if (f == NULL) {
                psfreq_log_error("psfreq_util_write",
                                "File '%s' failed to open for writing.",
                                abs_path);
                return WRITE_FAILURE;
        }

        if (fprintf(f, "%s\n", buf) < 0) {
                psfreq_log_error("psfreq_util_write",
                                "Failed to write buffer: %s to file '%s'.",
                                buf, abs_path);
                fclose(f);
                return WRITE_FAILURE;
        }

        fclose(f);
        return WRITE_SUCCESS;
}

/**
 * Write a buffer to a file
 * @param base The base path of the file to write to
 * @param file The relative path from the base of the file to write to
 * @param buf Buffer to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write2(const char *base, const char *file,
                const char *buf)
{
	char *abs_path;
	unsigned char r;
        psfreq_log_debug("psfreq_util_write2",
                        "Concat strings: '%s' and '%s'",
                        base, file);
        abs_path = psfreq_strings_concat(base, file);
        if (abs_path == STRING_CONCAT_ERROR) {
                psfreq_log_error("psfreq_util_write2",
                                "Concat strings: '%s' and '%s' has failed",
                                base, file);
                return WRITE_FAILURE;
        }

	r = psfreq_util_write(abs_path, buf);
	free(abs_path);
	return r;
}

/**
 * Write a number to a file
 * @param abs_path The absolute path of the file to write to
 * @param num Number value to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write_num(const char *abs_path, const int *num)
{
        char *s = psfreq_strings_from_int(num);
        const unsigned char r = psfreq_util_write(abs_path, s);
        free(s);
        return r;
}

/**
 * Write a number to a file
 * @param base The base path of the file to write to
 * @param file The relative path from the base of the file to write to
 * @param num Number value to write to file
 * @return Boolean, true if file was written to, false if otherwise
 */
unsigned char psfreq_util_write_num2(const char *base, const char *file,
                const int *num)
{
        char *s = psfreq_strings_from_int(num);
        const unsigned char r = psfreq_util_write2(base, file, s);
        free(s);
        return r;
}

/**
 * Strip the newline from the end of a string if it contains one
 *
 * @param s String to strip newline from
 * @return The string with newline stripped
 */
static char *psfreq_util_strip_string_end(char *s)
{
        unsigned int i;
        char *ns = NULL;
        if (s == NULL) {
                psfreq_log_error("psfreq_util_strip_string_end",
                                "String is NULL");
                return STRING_STRIP_ERROR;
        }
        i = strlen(s) - 1;
        if ((i > 0) && (s[i] == NEWLINE_CHAR)) {
                s[i] = NULL_CHAR;
        }

        if (psfreq_strings_asprintf(&ns, "%s", s) < 0) {
                return STRING_STRIP_ERROR;
        }
        return ns;
}
