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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "psfreq_log.h"
#include "psfreq_util.h"

void psfreq_read_pipe(char** lines, const char *const cmd,
                const uint8_t *size)
{
        psfreq_log_debug("psfreq_read_pipe", "Check for non-zero size");
        if (*size == 0) {
                psfreq_log_error("psfreq_read_pipe",
                                "Size is 0, which would"
                                " result in empty array");
                return NULL;
        }

        psfreq_log_debug("psfreq_read_pipe",
                        "Attempt to open pipe '%s'", cmd);
        FILE *const pipe = popen(cmd, "r");
        if (pipe == NULL) {
                psfreq_log_error("psfreq_read_pipe",
                                "Failed to open pipe '%s'", cmd);
                return NULL;
        }
        size_t n = 0;
        for (uint8_t i = 0; i < *size; ++i) {
                char *line = NULL;
                psfreq_log_debug("psfreq_read_pipe",
                                "Attempt to getline from pipe");
                if (getline(&line, &n, pipe) < 0) {
                        psfreq_log_error("psfreq_read_pipe",
                                        "Failed to read from pipe");
                        free(line);
                        pclose(pipe);
                        return NULL;
                } else {
                        psfreq_log_debug("psfreq_read_pipe",
                                        "Assign line '%s' to array %d",
                                        line, i);
                        lines[i] = line;
                }
        }
        psfreq_log_debug("psfreq_read_pipe", "Close pipe");
        pclose(pipe);
}

