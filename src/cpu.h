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
 * For questions please contact P.Yam Software at pyam.soft@gmail.com
 *
 */

#ifndef C_PYAM_CPU_H_
#define C_PYAM_CPU_H_

#include <stdint.h>

#define FILE_SCALING_DRIVER     "/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver"
#define FILE_INFO_MAX_FREQ      "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define FILE_INFO_MIN_FREQ      "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
#define FILE_SCALING_MAX_FREQ   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define FILE_SCALING_MIN_FREQ   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
#define FILE_PSTATE_TURBO       "/sys/devices/system/cpu/intel_pstate/no_turbo"
#define FILE_PSTATE_MAX         "/sys/devices/system/cpu/intel_pstate/max_perf_pct"
#define FILE_PSTATE_MIN         "/sys/devices/system/cpu/intel_pstate/min_perf_pct"
#define FILE_SCALING_MIN_NAME   "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq"
#define FILE_SCALING_MAX_NAME   "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq"

struct cpu_t {
        char** CPU_MAX_FREQ_FILES;
        char** CPU_MIN_FREQ_FILES;
        int32_t CPU_NUMBER;
	char* CPU_DRIVER;
	double CPU_INFO_MIN;
	double CPU_INFO_MIN_FREQ;
	double CPU_INFO_MAX_FREQ;
};

struct cpu_t cpu_create(void);
void cpu_destroy(struct cpu_t *cpu);

#endif
