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
#include <stdlib.h>

#include "src/cpu.h"
#include "src/cpuread.h"
#include "src/color.h"
#include "src/cpuutil.h"
#include "src/util.h"

static void internal_cpu_destroy(struct cpu_t *const cpu);

/*
 * Creates a new cpu_t object and initializes memory for the following:
 * CPU_MIN_FREQ_FILES array 
 * CPU_MAX_FREQ_FILES array
 * CPU_NUMBER int
 * and char strings allocated for
 * CPU_DRIVER, CPU_GOVERNOR, and IO_SCHED
 */
struct cpu_t cpu_create()
{
        struct cpu_t cpu;
        cpu.CPU_NUMBER = get_cpu_number(&cpu);
        cpu.CPU_MAX_FREQ_FILES = malloc(sizeof(char*) * cpu.CPU_NUMBER);
        if (cpu.CPU_MAX_FREQ_FILES == NULL) {
                fprintf(stderr, "%sUnable to malloc CPU_MAX files array%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                cpu_destroy(&cpu);
                exit(1);
        }
        for (int32_t i = 0; i < cpu.CPU_NUMBER; ++i) {
                safe_malloc(&cpu, asprintf(&(cpu.CPU_MAX_FREQ_FILES[i]),
                        FILE_SCALING_MAX_NAME, i),
                "Failed to allocate memory for MAX_FREQ_FILES",
                NULL);
        }
        cpu.CPU_MIN_FREQ_FILES = malloc(sizeof(char*) * cpu.CPU_NUMBER);
        if (cpu.CPU_MIN_FREQ_FILES == NULL) {
                fprintf(stderr, "%sUnable to malloc CPU_MAX files array%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                cpu_destroy(&cpu);
                exit(1);
        }
        for (int32_t i = 0; i < cpu.CPU_NUMBER; ++i) {
                safe_malloc(&cpu, asprintf(&(cpu.CPU_MIN_FREQ_FILES[i]),
                        FILE_SCALING_MIN_NAME, i),
                "Failed to allocate memory for MIN_FREQ_FILES",
                NULL);
        }
	cpu.CPU_INFO_MAX_FREQ = get_info_max_freq(&cpu);
	cpu.CPU_INFO_MIN_FREQ = get_info_min_freq(&cpu);
	cpu.CPU_INFO_MIN = get_info_min(&cpu);
	cpu.CPU_DRIVER = get_driver(&cpu);
	cpu.CPU_GOVERNOR = get_governor(&cpu);
	cpu.IO_SCHED = get_scheduler(&cpu);
        return cpu;
}

/*
 * Frees memory held by the arrays
 * CPU_MIN_FREQ_FILES
 * CPU_MAX_FREQ_FILES
 * and memory held by char strings
 */
void cpu_destroy(struct cpu_t *cpu)
{
        log_debug("Freeing allocated CPU resources\n");
        if (cpu->CPU_MIN_FREQ_FILES != NULL) {
                for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
                        if (cpu->CPU_MIN_FREQ_FILES[i] != NULL) {
                                log_debug("Freeing CPU %d\n", i);
                                free(cpu->CPU_MIN_FREQ_FILES[i]);
                                cpu->CPU_MIN_FREQ_FILES[i] = NULL;
                        }
                }
                log_debug("Freeing CPU_MIN_FREQ_FILES\n");
                free(cpu->CPU_MIN_FREQ_FILES);
                cpu->CPU_MIN_FREQ_FILES = NULL;
        }
        if (cpu->CPU_MAX_FREQ_FILES != NULL) {
                for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
                        if (cpu->CPU_MAX_FREQ_FILES[i] != NULL) {
                                log_debug("Freeing CPU %d\n", i);
                                free(cpu->CPU_MAX_FREQ_FILES[i]);
                                cpu->CPU_MAX_FREQ_FILES[i] = NULL;
                        }
                }
                free(cpu->CPU_MAX_FREQ_FILES);
                log_debug("Freeing CPU_MAX_FREQ_FILES\n");
                cpu->CPU_MAX_FREQ_FILES = NULL;
        }
	internal_cpu_destroy(cpu);
        log_debug("Freeing cpu\n");
        cpu = NULL;
}

/*
 * Free the allocated character strings
 */
static void internal_cpu_destroy(struct cpu_t *const cpu)
{
	if (cpu->CPU_DRIVER != NULL) {
		log_debug("Freeing CPU Driver\n");
		free(cpu->CPU_DRIVER);
		cpu->CPU_DRIVER = NULL;
	}
	if (cpu->CPU_GOVERNOR != NULL) {
		log_debug("Freeing CPU_GOVERNOR\n");
		free(cpu->CPU_GOVERNOR);
		cpu->CPU_GOVERNOR = NULL;

	}
	if (cpu->IO_SCHED != NULL) {
		log_debug("Freeing IO_SCHED\n");
		free(cpu->IO_SCHED);
		cpu->IO_SCHED = NULL;
	}
}
