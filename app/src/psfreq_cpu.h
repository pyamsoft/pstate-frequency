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

#include "psfreq_sysfs.h"

/*
 * NULL is defined as 0, just hard code it here.
 * No need to include the entire stdlib.h
 */
#define CPU_UNDEFINED   0
#define INIT_FAILURE    0
#define INIT_SUCCESS    1

typedef struct psfreq_cpu_type {
        unsigned char has_pstate;
        int cpu_num;
        char **vector_scaling_max_freq;
        char **vector_scaling_min_freq;
        char **vector_scaling_governor;
        long cpuinfo_min_freq;
        long cpuinfo_max_freq;
        long scaling_min_freq;
        long scaling_max_freq;
        char pst_turbo;
        char pst_max;
        char pst_min;
        char *scaling_governor;
        char *scaling_driver;
} psfreq_cpu_type;

/**
 * Initalize a new psfreq_cpu_type instance
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
unsigned char psfreq_cpu_init(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

/**
 * Re-initialize the dynamic parts of the psfreq_cpu_type
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
unsigned char psfreq_cpu_reinit(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

/**
 * Free memory held by an initialized psfreq_cpu_type instance
 *
 * @param cpu The psfreq_cpu_type instance to free
 */
void psfreq_cpu_destroy(psfreq_cpu_type *cpu);

/**
 * Get the cpuinfo_min_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the cpuinfo min freq as a percent value
 */
unsigned int psfreq_cpu_get_cpuinfo_min(const psfreq_cpu_type *cpu);

/**
 * Get the cpuinfo_max_freq value as a percent
 *
 * @return The value of the cpuinfo min freq as a percent value (hard 100)
 */
unsigned int psfreq_cpu_get_cpuinfo_max(void);

/**
 * Get the scaling_min_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the scaling min freq as a percent value
 */
unsigned int psfreq_cpu_get_scaling_min(const psfreq_cpu_type *cpu);

/**
 * Get the scaling_max_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the scaling max freq as a percent value
 */
unsigned int psfreq_cpu_get_scaling_max(const psfreq_cpu_type *cpu);

/**
 * Write the sysfs cpu max
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @param m The value to set for the max
 * @return Boolean type, true if application is successful, false otherwise
 */
unsigned char psfreq_cpu_set_max(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const int *const m);

/**
 * Write the sysfs cpu min
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @param m The value to set for the min
 * @return Boolean type, true if application is successful, false otherwise
 */
unsigned char psfreq_cpu_set_min(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const int *const m);

/**
 * Write the sysfs cpu governor
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @param m The value to set for the governor
 * @return Boolean type, true if application is successful, false otherwise
 */
unsigned char psfreq_cpu_set_gov(const psfreq_cpu_type *cpu,
                                 const psfreq_sysfs_type *sysfs,
                                 const char *const m);

/**
 * Write the sysfs cpu turbo
 *
 * @param sysfs An already initialized sysfs instance
 * @param m The value to set for the turbo
 * @return Boolean type, true if application is successful, false otherwise
 */
unsigned char psfreq_cpu_set_turbo(const psfreq_sysfs_type *sysfs,
                                   const int *const m);

/**
 * Retrieve the real CPU frequencies from the CPU
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @return Vector of Strings, 1 entry for each CPU, undefined if errors occur
 */
char **psfreq_cpu_get_real_freqs(const psfreq_cpu_type *cpu);

#endif

