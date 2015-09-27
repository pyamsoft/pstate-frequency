/*
 * pstate-frequency Easier control of the Intel p-state driver
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
 * For questions please contact pyamsoft at pyam(dot)soft(at)gmail(dot)com
 */

#ifndef PSFREQ_CPU_H
#define PSFREQ_CPU_H

#include <stdbool.h>
#include <stdint.h>

#include "psfreq_sysfs.h"

struct psfreq_cpu_type {
        bool has_pstate;
        uint8_t cpu_num;
        uint32_t cpuinfo_min_freq;
        uint32_t cpuinfo_max_freq;
        char **vector_scaling_max_freq;
        char **vector_scaling_min_freq;
        char **vector_scaling_governor;
};

void psfreq_cpu_init(struct psfreq_cpu_type *cpu,
                const struct psfreq_sysfs_type *sysfs);

#endif
