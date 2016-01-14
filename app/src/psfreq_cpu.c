/**
 * @file psfreq_cpu_init.c
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
 *
 *  Copyright (C) 2014 - 2016 Peter Kenji Yamanaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 * Part of the implementation for the psfreq_cpu_type. This module
 * handles the essential setup when a new psfreq_cpu_type is initialized.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "psfreq_constants.h"
#include "psfreq_cpu.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

/**
 * Sections of the psfreq_cpu_type can be read multiple times during lifetime
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
static unsigned char psfreq_cpu_init_dynamic(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs);

/**
 * Find the total number of online CPUS in the system
 *
 * @return The number of online CPUs
 */
static int psfreq_cpu_init_number_cpus(void);

/**
 * Find the current CPU scaling driver
 *
 * @param sysfs An already initialized sysfs instance
 * @return The name of the CPU scaling driver, undefined if otherwise
 */
static char *psfreq_cpu_init_driver(const psfreq_sysfs_type *sysfs);

/**
 * Returns whether or not the current driver is intel_pstate
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @return Boolean type, true if the current cpu driver is intel_pstate
 */
static unsigned char psfreq_cpu_init_has_pstate(const psfreq_cpu_type *cpu);

/**
 * Returns a CPU frequency as specified by arguments
 *
 * @param sysfs An already initialized sysfs instance
 * @param type The frequency type, scaling or cpuinfo
 * @param what Which frequency, either min or max
 * @return Value of the frequency in KHz
 */
static long psfreq_cpu_init_freq(
                const psfreq_sysfs_type *sysfs,
                const char *const type,
                const char *const what);

/**
 * Returns a CPU vector
 *
 * @param cpu The psfreq_cpu_type instance to use
 * @param what The specific field by which to build the vector
 * @return Vector of CPU values
 */
static char **psfreq_cpu_init_vector(const psfreq_cpu_type *cpu,
                const char *const what);

/**
 * Discover the current scaling_governor
 *
 * @param cpu The psfreq_cpu_type instance to use
 * @param sysfs An already initialized sysfs instance
 * @return Name of the governor, undefined if error
 */
static char *psfreq_cpu_init_governor(const psfreq_cpu_type *cpu,
                                const psfreq_sysfs_type *sysfs);

/**
 * Discover the current system turbo boost value
 *
 * @param sysfs An already initialized sysfs instance
 * @return The current value of turbo boost
 */
static char psfreq_cpu_init_turbo_boost(const psfreq_sysfs_type *sysfs);


/**
 * Get the passed in value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the passed in field as a percent value
 */
static unsigned int psfreq_cpu_get_cpu_pct(const psfreq_cpu_type *cpu,
                const unsigned int val);

static char **const CPU_VECTOR_UNDEFINED = NULL;
static const int CPU_NUMBER_UNDEFINED = -1;
static const unsigned int CPU_FREQUENCY_UNDEFINED = 0;
static const unsigned char HAS_PSTATE = 1;
static const unsigned char NO_PSTATE = 0;
static const unsigned char SUCCESS = 1;
static const unsigned char FAILURE = 0;

/**
 * Initalize a new psfreq_cpu_type instance
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
unsigned char psfreq_cpu_init(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs)
{
        char *m;
        cpu->scaling_driver = psfreq_cpu_init_driver(sysfs);
        if (cpu->scaling_driver == CPU_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->has_pstate = psfreq_cpu_init_has_pstate(cpu);
        if (cpu->has_pstate == NO_PSTATE) {
                free(cpu->scaling_driver);
                return INIT_FAILURE;
        }

        cpu->cpu_num = psfreq_cpu_init_number_cpus();
        if (cpu->cpu_num == CPU_NUMBER_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->vector_scaling_min_freq = psfreq_cpu_init_vector(cpu, "min_freq");
        if (cpu->vector_scaling_min_freq == CPU_VECTOR_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->vector_scaling_max_freq = psfreq_cpu_init_vector(cpu, "max_freq");
        if (cpu->vector_scaling_max_freq == CPU_VECTOR_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->vector_scaling_governor = psfreq_cpu_init_vector(cpu, "governor");
        if (cpu->vector_scaling_governor == CPU_VECTOR_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->cpuinfo_max_freq = psfreq_cpu_init_freq(sysfs, "cpuinfo", "max");
        if (cpu->cpuinfo_max_freq == CPU_FREQUENCY_UNDEFINED) {
                return INIT_FAILURE;
        }

        cpu->cpuinfo_min_freq = psfreq_cpu_init_freq(sysfs, "cpuinfo", "min");
        if (cpu->cpuinfo_min_freq == CPU_FREQUENCY_UNDEFINED) {
                return INIT_FAILURE;
        }

        m = psfreq_sysfs_read(sysfs, "intel_pstate/max_perf_pct");
        if (m == READ_ERROR) {
                return INIT_FAILURE;
        }
        cpu->pst_max = psfreq_strings_to_int(m);
        if (cpu->pst_max == STRING_CONVERT_TO_ERROR) {
                return INIT_FAILURE;
        }
        free(m);

        m = psfreq_sysfs_read(sysfs, "intel_pstate/min_perf_pct");
        if (m == READ_ERROR) {
                return INIT_FAILURE;
        }
        cpu->pst_min = psfreq_strings_to_int(m);
        if (cpu->pst_min == STRING_CONVERT_TO_ERROR) {
                return INIT_FAILURE;
        }
        free(m);

        return psfreq_cpu_init_dynamic(cpu, sysfs);
}

/**
 * Re-initialize the dynamic parts of the psfreq_cpu_type
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
unsigned char psfreq_cpu_reinit(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs)
{
        /*
         * Need to free scaling governor before re-mallocing
         */
        free(cpu->scaling_governor);
        return psfreq_cpu_init_dynamic(cpu, sysfs);
}

/**
 * Free memory held by an initialized psfreq_cpu_type instance
 *
 * @param cpu The psfreq_cpu_type instance to free
 */
void psfreq_cpu_destroy(psfreq_cpu_type *cpu)
{
        int i;
        psfreq_log_debug("psfreq_cpu_destroy",
                        "Free all allocated memory");
        for (i = 0; i < cpu->cpu_num; ++i) {
                psfreq_log_debug("psfreq_cpu_destroy",
                                "free vector_scaling_min_freq[%u]", i);
                free(cpu->vector_scaling_min_freq[i]);

                psfreq_log_debug("psfreq_cpu_destroy",
                                "free vector_scaling_max_freq[%u]", i);
                free(cpu->vector_scaling_max_freq[i]);

                psfreq_log_debug("psfreq_cpu_destroy",
                                "free vector_scaling_governor[%u]", i);
                free(cpu->vector_scaling_governor[i]);
        }

        psfreq_log_debug("psfreq_cpu_destroy",
                        "free vector_scaling_min_freq");
        free(cpu->vector_scaling_min_freq);

        psfreq_log_debug("psfreq_cpu_destroy",
                        "free vector_scaling_max_freq");
        free(cpu->vector_scaling_max_freq);

        psfreq_log_debug("psfreq_cpu_destroy",
                        "free vector_scaling_governor");
        free(cpu->vector_scaling_governor);

        psfreq_log_debug("psfreq_cpu_destroy",
                        "free scaling_governor");
        free(cpu->scaling_governor);

        psfreq_log_debug("psfreq_cpu_destroy",
                        "free scaling_driver");
        free(cpu->scaling_driver);
}

/**
 * Get the cpuinfo_min_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the cpuinfo min freq as a percent value
 */
unsigned int psfreq_cpu_get_cpuinfo_min(const psfreq_cpu_type *cpu)
{
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_get_cpuinfo_min",
                                "cpu is undefined");
                return CPU_FREQUENCY_UNDEFINED;
        }
        return psfreq_cpu_get_cpu_pct(cpu, cpu->cpuinfo_min_freq);
}

/**
 * Get the cpuinfo_max_freq value as a percent
 *
 * @return The value of the cpuinfo min freq as a percent value (hard 100)
 */
unsigned int psfreq_cpu_get_cpuinfo_max(void)
{
        return CPU_FREQUENCY_MAXIMUM;
}

/**
 * Get the scaling_min_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the scaling min freq as a percent value
 */
unsigned int psfreq_cpu_get_scaling_min(const psfreq_cpu_type *cpu)
{
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_get_scaling_min",
                                "cpu is undefined");
                return CPU_FREQUENCY_UNDEFINED;
        }
        return psfreq_cpu_get_cpu_pct(cpu, cpu->scaling_min_freq);
}

/**
 * Get the scaling_max_freq value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the scaling max freq as a percent value
 */
unsigned int psfreq_cpu_get_scaling_max(const psfreq_cpu_type *cpu)
{
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_get_scaling_max",
                                "cpu is undefined");
                return CPU_FREQUENCY_UNDEFINED;
        }
        return psfreq_cpu_get_cpu_pct(cpu, cpu->scaling_max_freq);
}

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
                                 const int *const m)
{
        int i;
        int freq;
        int n;
        int num;
        if (*m >= CPU_FREQUENCY_MAXIMUM) {
                n = psfreq_cpu_get_cpuinfo_max();
        } else if (*m <= CPU_FREQUENCY_MINIMUM) {
                n = psfreq_cpu_get_cpuinfo_min(cpu) + 1;
        } else {
                n = *m;
        }
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_max",
                                "sysfs is undefined");
                return FAILURE;
        }
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_max",
                                "cpu is undefined");
                return FAILURE;
        }

        num = cpu->cpu_num;
        if (num == CPU_NUMBER_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_max",
                                "cpu->cpu_num is undefined");
                return FAILURE;
        }
        if (cpu->vector_scaling_max_freq == CPU_VECTOR_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_max",
                                "cpu->vector_scaling_max_freq is undefined");
                return FAILURE;
        }
        if (psfreq_sysfs_write_num(sysfs, "intel_pstate/max_perf_pct", &n)
                         == WRITE_FAILURE) {
                return FAILURE;
        }

        freq = cpu->cpuinfo_max_freq * ((double) n / CPU_FREQUENCY_MAXIMUM);
        for (i = 0; i < num; ++i) {
                if (psfreq_sysfs_write_num(sysfs,
                        cpu->vector_scaling_max_freq[i], &freq)
                                == WRITE_FAILURE) {
                        return FAILURE;
                }
        }
        return SUCCESS;
}

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
                                 const int *const m)
{
        int i;
        int freq;
        int n;
        int num;

        if (*m >= CPU_FREQUENCY_MAXIMUM) {
                n = psfreq_cpu_get_cpuinfo_max() - 1;
        } else if (*m <= CPU_FREQUENCY_MINIMUM) {
                n = psfreq_cpu_get_cpuinfo_min(cpu);
        } else {
                n = *m;
        }
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_min",
                                "sysfs is undefined");
                return FAILURE;
        }
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_min",
                                "cpu is undefined");
                return FAILURE;
        }

        num = cpu->cpu_num;
        if (num == CPU_NUMBER_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_min",
                                "cpu->cpu_num is undefined");
                return FAILURE;
        }
        if (cpu->vector_scaling_min_freq == CPU_VECTOR_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_min",
                                "cpu->vector_scaling_min_freq is undefined");
                return FAILURE;
        }
        if (psfreq_sysfs_write_num(sysfs, "intel_pstate/min_perf_pct", &n)
                        == WRITE_FAILURE) {
                return FAILURE;
        }

        freq = cpu->cpuinfo_max_freq * ((double) n / CPU_FREQUENCY_MAXIMUM);
        for (i = 0; i < num; ++i) {
                if (psfreq_sysfs_write_num(sysfs,
                        cpu->vector_scaling_min_freq[i], &freq)
                                == WRITE_FAILURE) {
                        return FAILURE;
                }
        }
        return SUCCESS;
}

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
                                 const char *const m)
{
        int i;
        int num;
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_gov",
                                "sysfs is undefined");
                return FAILURE;
        }
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_gov",
                                "cpu is undefined");
                return FAILURE;
        }

        num = cpu->cpu_num;
        if (num == CPU_NUMBER_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_gov",
                                "cpu->cpu_num is undefined");
                return FAILURE;
        }
        if (cpu->vector_scaling_governor == CPU_VECTOR_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_gov",
                                "cpu->vector_scaling_governor is undefined");
                return FAILURE;
        }
        for (i = 0; i < num; ++i) {
                if (psfreq_sysfs_write(sysfs,
                        cpu->vector_scaling_governor[i], m)
                                == WRITE_FAILURE) {
                        return FAILURE;
                }
        }
        return SUCCESS;
}

/**
 * Write the sysfs cpu turbo
 *
 * @param sysfs An already initialized sysfs instance
 * @param m The value to set for the turbo
 * @return Boolean type, true if application is successful, false otherwise
 */
unsigned char psfreq_cpu_set_turbo(const psfreq_sysfs_type *sysfs,
                                   const int *const m)
{
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_set_turbo",
                                "sysfs is undefined");
                return FAILURE;
        }
        if (psfreq_sysfs_write_num(sysfs, "intel_pstate/no_turbo", m)
                        == WRITE_FAILURE) {
                return FAILURE;
        }
        return SUCCESS;
}

/**
 * Retrieve the real CPU frequencies from the CPU
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @return Vector of Strings, 1 entry for each CPU, undefined if errors occur
 */
char **psfreq_cpu_get_real_freqs(const psfreq_cpu_type *cpu)
{
        const char *cmd = "grep MHz /proc/cpuinfo | cut -c12-";
        int size;
        char **res;
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_get_real_freqs",
                                "cpu is undefined");
                return CPU_VECTOR_UNDEFINED;
        }

        size = cpu->cpu_num;
        if (size == CPU_NUMBER_UNDEFINED) {
                return CPU_VECTOR_UNDEFINED;
        }

        res = psfreq_util_read_pipe(cmd, size);
        if (res == READ_ERROR) {
                return CPU_VECTOR_UNDEFINED;
        }
        return res;
}

/**
 * Sections of the psfreq_cpu_type can be read multiple times during lifetime
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @param sysfs An already initialized sysfs instance
 * @return Boolean type, true if init successful, false if otherwise
 */
static unsigned char psfreq_cpu_init_dynamic(psfreq_cpu_type *cpu,
                const psfreq_sysfs_type *sysfs)
{
        cpu->scaling_max_freq = psfreq_cpu_init_freq(sysfs, "scaling", "max");
        if (cpu->scaling_max_freq == CPU_FREQUENCY_UNDEFINED) {
                return INIT_FAILURE;
        }
        cpu->scaling_min_freq = psfreq_cpu_init_freq(sysfs, "scaling", "min");
        if (cpu->scaling_min_freq == CPU_FREQUENCY_UNDEFINED) {
                return INIT_FAILURE;
        }
        cpu->scaling_governor = psfreq_cpu_init_governor(cpu, sysfs);
        if (cpu->scaling_governor == GOV_UNDEFINED) {
                return INIT_FAILURE;
        }
        cpu->pst_turbo = psfreq_cpu_init_turbo_boost(sysfs);
        if (cpu->pst_turbo == TURBO_UNDEFINED) {
                return INIT_FAILURE;
        }
        return INIT_SUCCESS;
}


/**
 * Find the total number of online CPUS in the system
 *
 * @return The number of online CPUs
 */
static int psfreq_cpu_init_number_cpus(void)
{
        const char *const cmd = "grep processor /proc/cpuinfo | wc -l";
        const unsigned int size = 1;
        char **res = psfreq_util_read_pipe(cmd, size);
        unsigned int n;

        if (res == READ_ERROR) {
                psfreq_log_error("psfreq_cpu_init_number_cpus",
                                "Failed to find number of cpus");
                return CPU_NUMBER_UNDEFINED;
        }

        n = psfreq_strings_to_int(res[0]);
        psfreq_log_debug("psfreq_cpu_init_number_cpus",
                        "Free memory held by res");
        free(res[0]);
        free(res);
        psfreq_log_debug("psfreq_cpu_init_number_cpus",
                        "Number of cpus: %u", n);
        return n;
}

/**
 * Find the current CPU scaling driver
 *
 * @param sysfs An already initialized sysfs instance
 * @return The name of the CPU scaling driver, undefined if otherwise
 */
static char *psfreq_cpu_init_driver(const psfreq_sysfs_type *sysfs)
{
        char *driver;

        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_has_pstate",
                                "sysfs is undefined");
                return CPU_UNDEFINED;
        }
        driver = psfreq_sysfs_read(sysfs, "cpu0/cpufreq/scaling_driver");
        if (driver == READ_ERROR) {
                psfreq_log_error("psfreq_cpu_init_has_pstate",
                                "Unable to check for intel_pstate driver");
                return CPU_UNDEFINED;
        }

        return driver;
}

/**
 * Returns whether or not the current driver is intel_pstate
 *
 * @param cpu The psfreq_cpu_type instance to init
 * @return Boolean type, true if the current cpu driver is intel_pstate
 */
static unsigned char psfreq_cpu_init_has_pstate(const psfreq_cpu_type *cpu)
{

        const char *const cmp = "intel_pstate";
        const char *driver = cpu->scaling_driver;
        return psfreq_strings_equals(cmp, driver);
}

/**
 * Returns a CPU frequency as specified by arguments
 *
 * @param sysfs An already initialized sysfs instance
 * @param type The frequency type, scaling or cpuinfo
 * @param what Which frequency, either min or max
 * @return Value of the frequency in KHz
 */
static long psfreq_cpu_init_freq(
                const psfreq_sysfs_type *sysfs,
                const char *const type,
                const char *const what)
{
        char *line;
        char *f;
        long result;
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_freq",
                                "sysfs is undefined");
                return CPU_FREQUENCY_UNDEFINED;
        }

        if (psfreq_strings_asprintf(&f,
                        "cpu0/cpufreq/%s_%s_freq", type, what) < 0) {
                return CPU_FREQUENCY_UNDEFINED;
        }

        line = psfreq_sysfs_read(sysfs, f);
        free(f);
        if (line == READ_ERROR) {
                return CPU_FREQUENCY_UNDEFINED;
        }
        result = psfreq_strings_to_long(line);
        if (result == STRING_CONVERT_TO_ERROR) {
                psfreq_log_error("psfreq_cpu_init_freq",
                                "Unable to convert string '%s' to uint", line);
        }

        free(line);
        return result;
}

/**
 * Returns a CPU vector
 *
 * @param cpu The psfreq_cpu_type instance to use
 * @param what The specific field by which to build the vector
 * @return Vector of CPU values
 */
static char **psfreq_cpu_init_vector(const psfreq_cpu_type *cpu,
                const char *const what)
{
        int num;
        int i;
        char **vector;

        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_vector",
                                "cpu is undefined, failed to find cpu number");
                return CPU_VECTOR_UNDEFINED;
        }

        num = cpu->cpu_num;
        if (num == CPU_NUMBER_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_vector",
                                "Size is undefined, failed to find cpu number");
                return CPU_VECTOR_UNDEFINED;
        }

        vector = malloc(num * sizeof(char *));
        if (vector == NULL) {
                psfreq_log_error("psfreq_cpu_init_vector",
                                "Failed to malloc for vector");
                return CPU_VECTOR_UNDEFINED;
        }

        for (i = 0; i < num; ++i) {
                char *buf = NULL;
                if (psfreq_strings_asprintf(&buf,
                                "cpu%u/cpufreq/scaling_%s", i, what) < 0) {
                        free(vector);
                        return CPU_VECTOR_UNDEFINED;
                }

                psfreq_log_debug("psfreq_cpu_init_vector",
                                "assign '%s' to vector index %d", buf, i);
                vector[i] = buf;
        }
        return vector;
}

/**
 * Discover the current scaling_governor
 *
 * @param cpu The psfreq_cpu_type instance to use
 * @param sysfs An already initialized sysfs instance
 * @return Name of the governor, undefined if error
 */
static char *psfreq_cpu_init_governor(const psfreq_cpu_type *cpu,
                                const psfreq_sysfs_type *sysfs)
{
        const char *f;
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_governor",
                                "cpu is undefined");
                return GOV_UNDEFINED;
        }
        if (cpu->vector_scaling_governor == CPU_VECTOR_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_governor",
                                "cpu->vector_scaling_governor is undefined");
                return GOV_UNDEFINED;
        }

        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_governor",
                                "sysfs is undefined");
                return GOV_UNDEFINED;
        }
        f = cpu->vector_scaling_governor[0];
        return psfreq_sysfs_read(sysfs, f);
}

/**
 * Discover the current system turbo boost value
 *
 * @param sysfs An already initialized sysfs instance
 * @return The current value of turbo boost
 */
static char psfreq_cpu_init_turbo_boost(const psfreq_sysfs_type *sysfs)
{
        char turbo;
        char *line;
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_init_turbo_boost",
                                "sysfs is undefined");
                return TURBO_UNDEFINED;
        }
        line = psfreq_sysfs_read(sysfs, "intel_pstate/no_turbo");
        if (line == READ_ERROR) {
                return TURBO_UNDEFINED;
        }

        turbo = psfreq_strings_to_int(line);
        free(line);
        return turbo;
}

/**
 * Get the passed in value as a percent
 *
 * @param cpu The psfreq_cpu_type instance to free
 * @return The value of the passed in field as a percent value
 */
static unsigned int psfreq_cpu_get_cpu_pct(const psfreq_cpu_type *cpu,
                const unsigned int val)
{
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("psfreq_cpu_get_cpu_pct",
                                "cpu is undefined");
                return CPU_FREQUENCY_UNDEFINED;
        }
        return ((double) val / cpu->cpuinfo_max_freq) * CPU_FREQUENCY_MAXIMUM;
}


