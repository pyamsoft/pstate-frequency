/*
    pstate_frequency Easier control of the Intel p-state driver

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    For questions please contact P.Yam Software at pyam.soft@gmail.com

*/

#define _GNU_SOURCE
#include <stdio.h>
#include "src/cpu.h"
#include "src/cpuread.h"
#include "src/color.h"
#include "src/cpuutil.h"
#include "src/util.h"
#include <stdlib.h>

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
        return cpu;
}

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
        log_debug("Freeing cpu\n");
        cpu = NULL;
}
