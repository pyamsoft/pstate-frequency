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

#ifndef C_PYAM_CPUREAD_H_
#define C_PYAM_CPUREAD_H_

#include "src/cpu.h"

#if INCLUDE_UDEV_RULE >= 1

char *get_power_type(struct cpu_t *const cpu, char *const file_name);
int32_t get_online_status(struct cpu_t *const cpu, char *const file_name);

#endif

double get_scaling_max(struct cpu_t *const cpu);
double get_scaling_min(struct cpu_t *const cpu);
int32_t get_turbo(struct cpu_t *const cpu);
double get_scaling_max_freq(struct cpu_t *const cpu);
double get_scaling_min_freq(struct cpu_t *const cpu);
char *get_driver(struct cpu_t *const cpu);
char *get_scheduler(struct cpu_t *const cpu);
char *get_governor(struct cpu_t *const cpu);
double get_info_min(struct cpu_t *const cpu);
double get_info_max_freq(struct cpu_t *const cpu);
double get_info_min_freq(struct cpu_t *const cpu);
int32_t has_pstate(struct cpu_t *const cpu);
int32_t get_cpu_number(struct cpu_t *const cpu);

#endif
