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

#include <stdbool.h>
#include <stdint.h>

#include "psfreq_sysfs.h"

typedef struct psfreq_cpu_type {
        bool has_pstate;
        uint8_t cpu_num;
        uint32_t cpuinfo_min_freq;
        uint32_t cpuinfo_max_freq;
        char **vector_scaling_max_freq;
        char **vector_scaling_min_freq;
        char **vector_scaling_governor;
} psfreq_cpu_type;

void psfreq_cpu_init(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

void psfreq_cpu_destroy(psfreq_cpu_type *cpu);

uint32_t psfreq_cpu_get_min(const psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

#endif
