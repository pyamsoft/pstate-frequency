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

#define FILE_CPU_NUMBER         "/tmp/cpunumber.txt"
#define DIR_PSTATE              "/sys/devices/system/cpu/intel_pstate"
#define FILE_CPU_SCALING_DRIVER "/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver"
#define FILE_CPU_GOVERNOR       "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
#define FILE_CPUINFO_MAX_FREQ   "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define FILE_CPUINFO_MIN_FREQ   "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
#define FILE_PSTATE_TURBO       "/sys/devices/system/cpu/intel_pstate/no_turbo"
#define FILE_CPU_MAX_FREQ       "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define FILE_CPU_MIN_FREQ       "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"

struct pyam_cpu_t {
    char** CPU_MAX_FREQ_FILES;
    char** CPU_MIN_FREQ_FILES;
};

struct pyam_cpu_t
pyam_cpu_create(void);

void
pyam_cpu_set_turbo(
        struct pyam_cpu_t* const cpu,
        const int32_t turbo);

int32_t
pyam_cpu_get_number(
        struct pyam_cpu_t* const cpu);

int32_t
pyam_cpu_get_mhz(
        struct pyam_cpu_t* const cpu);

void
pyam_cpu_set_max(
        struct pyam_cpu_t* const cpu,
        const int32_t max);

char* 
pyam_cpu_get_driver(
        struct pyam_cpu_t* const cpu);

void
pyam_cpu_set_min(
        struct pyam_cpu_t* const cpu,
        const int32_t min);

int32_t
pyam_cpu_get_min(
        struct pyam_cpu_t* const cpu);

int32_t
pyam_cpu_get_turbo(
        struct pyam_cpu_t* const cpu);

double
pyam_cpu_get_max_freq(
        struct pyam_cpu_t* const cpu);

double
pyam_cpu_get_min_freq(
        struct pyam_cpu_t* const cpu);

int32_t
pyam_cpu_get_max(
        struct pyam_cpu_t* const cpu);

double
pyam_cpu_get_cpuinfo_max(void);

double
pyam_cpu_get_cpuinfo_min(
        struct pyam_cpu_t* const cpu);

char*
pyam_cpu_get_governor(
        struct pyam_cpu_t* const cpu);

void
pyam_cpu_set_governor(
        struct pyam_cpu_t* const cpu,
        const int32_t governor);

char**
pyam_cpu_get_available_governors(
        struct pyam_cpu_t* const cpu);

void
pyam_cpu_destroy(
        struct pyam_cpu_t* cpu);

#endif  // C_PYAM_CPU_H_
