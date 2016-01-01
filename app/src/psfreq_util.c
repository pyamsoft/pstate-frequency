/**
 * @file psfreq_util.c
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
/*
 * For popen using C99
 */
#define _POSIX_C_SOURCE 2
#include <stdio.h>

#include "psfreq_log.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

static char *psfreq_util_strip_string_end(char *s);

char **psfreq_util_read_pipe(const char *const cmd, const unsigned int *size)
{
        char **lines;
        FILE *pipe;
        unsigned int i;
        const size_t n = 20;
        if (cmd == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "cmd is NULL");
                return NULL;
        }
        if (*size == 0) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Size is 0, which would"
                                " result in empty array");
                return NULL;
        }

        lines = malloc(*size * sizeof(char *));
        if (lines == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Failed to malloc for lines");
                return NULL;
        }

        pipe = popen(cmd, "r");
        if (pipe == NULL) {
                psfreq_log_error("psfreq_util_read_pipe",
                                "Failed to open pipe '%s'", cmd);
                free(lines);
                return NULL;
        }
        for (i = 0; i < *size; ++i) {
                char line[n];
                if (fgets(line, n, pipe) == NULL) {
                        psfreq_log_error("psfreq_util_read_pipe",
                                        "Failed to read from pipe");
                        free(lines);
                        pclose(pipe);
                        return NULL;
                } else {
                        psfreq_log_debug("psfreq_util_read_pipe",
                                "Assign line '%s' to array %d", line, i);
                        lines[i] = psfreq_util_strip_string_end(line);
                }
        }
        pclose(pipe);
        return lines;
}

char *psfreq_util_read2(const char *base, const char *file)
{
        char *r;
        char *abs_path;
        abs_path = psfreq_strings_concat(base, file);
        if (abs_path == NULL) {
                return NULL;
        }
        r = psfreq_util_read(abs_path);
        free(abs_path);
        return r;
}

char *psfreq_util_read(const char *abs_path)
{
        FILE *f;
        const size_t n = 20;
        char line[n];

        f = fopen(abs_path, "r");
        if (f == NULL) {
                psfreq_log_error("psfreq_util_read",
                                "File '%s' failed to open for reading.",
                                abs_path);
                return NULL;
        }

        if (fgets(line, n, f) == NULL) {
                psfreq_log_error("psfreq_util_read",
                                "Failed to read buffer from file '%s'.",
                                abs_path);
                fclose(f);
                return NULL;
        }

        fclose(f);
        return psfreq_util_strip_string_end(line);
}

bool psfreq_util_write(const char *abs_path, const char *buf)
{
        FILE *f;
        if (buf == NULL) {
                psfreq_log_error("psfreq_util_write",
                                "buf is NULL, exit.");
                return false;
        }

        f = fopen(abs_path, "w");
        if (f == NULL) {
                psfreq_log_error("psfreq_util_write",
                                "File '%s' failed to open for writing.",
                                abs_path);
                return false;
        }

        if (fprintf(f, "%s\n", buf) < 0) {
                psfreq_log_error("psfreq_util_write",
                                "Failed to write buffer: %s to file '%s'.",
                                buf, abs_path);
                fclose(f);
                return false;
        }

        fclose(f);
        return true;
}

bool psfreq_util_write2(const char *base, const char *file,
                const char *buf)
{
	char *abs_path;
	bool r;
        psfreq_log_debug("psfreq_util_write2",
                        "Concat strings: '%s' and '%s'",
                        base, file);
        abs_path = psfreq_strings_concat(base, file);
        if (abs_path == NULL) {
                psfreq_log_error("psfreq_util_write2",
                                "Concat strings: '%s' and '%s' has failed.\n"
                                "Function will return false.",
                                base, file);
                return false;
        }

	r = psfreq_util_write(abs_path, buf);
	free(abs_path);
	return r;
}

bool psfreq_util_write_num(const char *abs_path, const int *num)
{
        char *s = psfreq_strings_from_int(num);
        const bool r = psfreq_util_write(abs_path, s);
        free(s);
        return r;
}

bool psfreq_util_write_num2(const char *base, const char *file,
                const int *num)
{
        char *s = psfreq_strings_from_int(num);
        const bool r = psfreq_util_write2(base, file, s);
        free(s);
        return r;
}


static char *psfreq_util_strip_string_end(char *s)
{
        unsigned int i;
        char *ns = NULL;
        if (s == NULL) {
                psfreq_log_error("psfreq_util_strip_string_end",
                                "String is NULL");
                return NULL;
        }
        i = strlen(s) - 1;
        if ((i > 0) && (s[i] == '\n')) {
                s[i] = '\0';
        }

        if (psfreq_strings_asprintf(&ns, "%s", s) < 0) {
                return NULL;
        }
        return ns;
}
