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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "src/cpuutil.h"
#include "src/util.h"
#include "src/color.h"

/*
 * Given a file_name, resolve whether the file exists on the given environment path.
 * Because of the expected usage of this function, any file which cannot be found results
 * in a critical error in the program and it will be unable to recover. Exit the program if the
 * specified file cannot be resolved.
 */
char* resolve_path_to_file(struct cpu_t *const cpu, const char *const file_name)
{
        log_debug("getting the path and copying it\n");
        char *const real_path = getenv("PATH");
        char *default_path = "/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/bin:/sbin";
        char *copy_path = real_path == NULL ? default_path : real_path;
        char *path;
        safe_malloc(cpu, asprintf(&path, "%s", copy_path), "Unable to copy path", NULL);
        char *token;
        token = strtok(path, ":");
        while (token != NULL) {
                char *cmd;
                safe_malloc(cpu, asprintf(&cmd, "%s/%s", token, file_name),
                        "Error allocating memory for path", NULL);
                if (access(cmd, F_OK) != -1) {
                        log_debug("free resources, path is resolved\n");
                        free(path);
                        return cmd;
                }
                token = strtok(NULL, ":");
                log_debug("free resources, path still not resolved\n");
                free(cmd);
        }
        fprintf(stderr, "%sCan't find %s binary on $PATH%s\n",
                PYAM_COLOR_BOLD_RED, file_name, PYAM_COLOR_OFF);
        free(path);
        cpu_destroy(cpu);
        exit(1);
}

/*
 * A wrapper which checks the success of any function which follows the
 * returns structure of a malloc call. If the malloc call returns a -1 meaning
 * failure, exit the program and clean up any of the currently malloced values held in the
 * varargs array
 */
void safe_malloc(struct cpu_t *const cpu, const int32_t result,
        const char *const error_message, ...)
{
        log_debug("safe_malloc: result of malloc is %d\n", result);
        if (result == -1) {
                fprintf(stderr, "%s%s%s\n",
                        PYAM_COLOR_BOLD_RED, error_message, PYAM_COLOR_OFF);
                va_list list;
                va_start(list, error_message);
                FILE *file = va_arg(list, FILE*);
                while (file != NULL) {
                                log_error("Freeing file\n");
                                free(file);
                                file = va_arg(list, FILE*);
                }
                va_end(list);
                log_debug("safe_malloc: destroy_cpu\n");
                cpu_destroy(cpu);
                exit(2);
        }
}

/*
 * Check that a file was successfully opened. If the file state is
 * -1 meaning failure, exit from the program because it is mission critical
 * that a given file be available
 */
void check_file(struct cpu_t *const cpu, FILE *const file,
        const char *const file_name)
{
        log_debug("checking if file %s is NULL\n", file_name);
        if (file == NULL) {
                fprintf(stderr, "%sError opening '%s' Exiting.%s\n",
                        PYAM_COLOR_BOLD_RED, file_name, PYAM_COLOR_OFF);
                cpu_destroy(cpu);
                exit(3);
        }
        log_debug("file %s is SAFE\n", file_name);
}

