/**
 * @file psfreq_sysfs.c
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
 * psfreq_sysfs_type makes it easy for operations which involve the sysfs
 * interfaces which are exposed on modern Linux systems.
 *
 * The sysfs requires two parameters in order to be effective, an instance
 * of the psfreq_cpu_type and the base path for the sysfs cpu device interfaces
 * which is usually located at /sys/devices/system/cpu
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psfreq_log.h"
#include "psfreq_strings.h"
#include "psfreq_sysfs.h"

/**
 * Initialize a new psfreq_sysfs_type
 *
 * @return A new instance of psfreq_sysfs_type using the default base path
 */
struct psfreq_sysfs_type psfreq_sysfs_init(void)
{
        psfreq_log_debug("psfreq_sysfs_init",
                        "Create a new psfreq_sysfs_type instance.");
        struct psfreq_sysfs_type sysfs;
        /* sysfs.base_path = "/sys/devices/system/cpu/"; */
        sysfs.base_path = "/home/peter/";
        return sysfs;
}

/**
 * Write a given value to a file
 *
 * @param sysfs psfreq_sysfs_type instance to use
 * @param file relative file path from sysfs->base_path to write to
 * @param buf buffer containing the value to write to file
 * @return Boolean value, true if writing to the file was successfully
 *         performed, false otherwise
 */
bool psfreq_sysfs_write(const struct psfreq_sysfs_type *sysfs,
                const char *file, const char *buf)
{
        const char *const path = sysfs->base_path;
        psfreq_log_debug("psfreq_sysfs_write",
                        "Concat strings: '%s' and '%s'",
                        path, file);
        char *const abs_path = psfreq_strings_concat(path, file);
        if (abs_path == NULL) {
                psfreq_log_error("psfreq_sysfs_write",
                                "Concat strings: '%s' and '%s' has failed.\n"
                                "Function will return false.",
                                path, file);
                return false;
        }

        psfreq_log_debug("psfreq_sysfs_write",
                        "Attempt to open file: '%s'",
                        abs_path);
        FILE *const f = fopen(abs_path, "w");
        if (f == NULL) {
                psfreq_log_error("psfreq_sysfs_write",
                                "File '%s' failed to open for writing.",
                                abs_path);
                free(abs_path);
                return false;
        }

        psfreq_log_debug("psfreq_sysfs_write",
                        "Attempt to write buffer '%s' to file: '%s'",
                        buf, abs_path);
        if (fprintf(f, "%s\n", buf) < 0) {
                psfreq_log_error("psfreq_sysfs_write",
                                "Failed to write buffer: %s to file '%s'.",
                                buf, abs_path);
                free(abs_path);
                fclose(f);
                return false;
        }

        psfreq_log_debug("psfreq_sysfs_write",
                        "Close file: '%s'",
                        abs_path);
        free(abs_path);
        fclose(f);
        return true;
}

char *psfreq_sysfs_read(const struct psfreq_sysfs_type *sysfs,
                const char *file)
{
        const char *const path = sysfs->base_path;
        psfreq_log_debug("psfreq_sysfs_read",
                        "Concat strings: '%s' and '%s'",
                        path, file);
        char *const abs_path = psfreq_strings_concat(path, file);
        if (abs_path == NULL) {
                psfreq_log_error("psfreq_sysfs_read",
                                "Concat strings: '%s' and '%s' has failed.\n"
                                "Function will return false.",
                                path, file);
                return NULL;
        }

        psfreq_log_debug("psfreq_sysfs_read",
                        "Attempt to open file: '%s'",
                        abs_path);
        FILE *const f = fopen(abs_path, "r");
        if (f == NULL) {
                psfreq_log_error("psfreq_sysfs_read",
                                "File '%s' failed to open for reading.",
                                abs_path);
                free(abs_path);
                return NULL;
        }

        psfreq_log_debug("psfreq_sysfs_read",
                        "Attempt to read buffer from file: '%s'",
                        abs_path);
        char *line = NULL;
        size_t n = 0;
        psfreq_log_debug("psfreq_sysfs_read",
                        "Getting a line from file %s\n", abs_path);
        if (getline(&line, &n, f) < 0) {
                psfreq_log_error("psfreq_sysfs_read",
                                "Failed to read buffer from file '%s'.",
                                abs_path);
                free(line);
                free(abs_path);
                fclose(f);
                return NULL;
        }

        psfreq_log_debug("psfreq_sysfs_read",
                        "Close file: '%s'",
                        abs_path);
        free(abs_path);
        fclose(f);
        return line;
}
