/**
 * @file psfreq_cpu.h
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
 * Defines the psfreq_cpu_type and the functions that can interact with it
 * to help control the intel_pstate cpu driver.
 */

#ifndef PSFREQ_CPU_H
#define PSFREQ_CPU_H

#define CPU_UNDEFINED           NULL

#include <stdbool.h>

#include "psfreq_sysfs.h"

typedef struct psfreq_cpu_type {
        bool has_pstate;
        unsigned int cpu_num;
        char **vector_scaling_max_freq;
        char **vector_scaling_min_freq;
        char **vector_scaling_governor;
        unsigned int cpuinfo_min_freq;
        unsigned int cpuinfo_max_freq;
        unsigned int scaling_min_freq;
        unsigned int scaling_max_freq;
        char pst_turbo;
        char pst_max;
        char pst_min;
        char *scaling_governor;
        char *scaling_driver;
} psfreq_cpu_type;

bool psfreq_cpu_init(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

bool psfreq_cpu_reinit(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

void psfreq_cpu_destroy(psfreq_cpu_type *cpu);

unsigned int psfreq_cpu_get_cpuinfo_min(const psfreq_cpu_type *cpu);

unsigned int psfreq_cpu_get_cpuinfo_max(void);

unsigned int psfreq_cpu_get_scaling_min(const psfreq_cpu_type *cpu);

unsigned int psfreq_cpu_get_scaling_max(const psfreq_cpu_type *cpu);

bool psfreq_cpu_set_max(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const int *const m);

bool psfreq_cpu_set_min(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const int *const m);

bool psfreq_cpu_set_gov(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const char *const m);

bool psfreq_cpu_set_turbo(const psfreq_sysfs_type *sysfs,
                                   const int *const m);

char **psfreq_cpu_get_real_freqs(const psfreq_cpu_type *cpu);

#endif

