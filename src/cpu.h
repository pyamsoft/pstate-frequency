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

#ifndef C_PYAM_CPU_H_
#define C_PYAM_CPU_H_

#include <stdint.h>
#include <stddef.h>

#define FILE_CPU_MAX            "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define FILE_CPU_MIN            "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
#define FILE_PSTATE_TURBO       "/sys/devices/system/cpu/intel_pstate/no_turbo"
#define FILE_PSTATE_MAX_PERCENT "/sys/devices/system/cpu/intel_pstate/max_perf_pct"
#define FILE_PSTATE_MIN_PERCENT "/sys/devices/system/cpu/intel_pstate/min_perf_pct"
#define FILE_CPU_FREQ           "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"

struct pyam_cpu_t {
    char** CPU_FREQ_FILES;
};

struct pyam_cpu_t
pyam_cpu_create(void);

void
pyam_cpu_set_turbo(
        const int32_t turbo);

int32_t
pyam_cpu_get_number(void);

int32_t
pyam_cpu_get_mhz(void);

void
pyam_cpu_set_max(
        struct pyam_cpu_t* const cpu,
        const int32_t max);

int32_t
pyam_cpu_get_min(void);

int32_t
pyam_cpu_get_turbo(void);

int32_t
pyam_cpu_get_freq(void);

int32_t
pyam_cpu_get_max(void);

void
pyam_cpu_destroy(
        struct pyam_cpu_t* cpu);

#endif  // C_PYAM_CPU_H_
