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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "src/cpuwrite.h"
#include "src/util.h"
#include "src/cpuutil.h"
#include "src/color.h"
#include "src/cpuread.h"

static void internal_set(struct cpu_t *const cpu, const char *const file_name,
        const int32_t value);
static void internal_set_freq(struct cpu_t *const cpu, char **const frequency_files,
        const size_t value);

#if WRITE_MSR >= 1
static void write_msr(struct cpu_t *const cpu, const int32_t value);

/*
 * Checks for the existence of the wrmsr program from the msr-utils package
 * If it can be found on the path then attempt to load the module msr using
 * modprobe and set the turbo boost bit in the CPU to either ON or OFF based
 * on the value passed in as a parameter.
 */
static void write_msr(struct cpu_t *const cpu, const int32_t value)
{
        log_debug("Resolving path for wrmsr and modprobe\n");
        char *cmd = resolve_path_to_file(cpu, "wrmsr");
        char *modprobe = resolve_path_to_file(cpu, "modprobe");
        char *modprobe_cmd;
        log_debug("creating command 'modprobe msr'\n");
        safe_malloc(cpu, asprintf(&modprobe_cmd, "%s msr", modprobe),
                "Failed to allocate memory for modprobe msr",
                cmd, modprobe, NULL);
        log_debug("running command 'modprobe msr'\n");
        safe_malloc(cpu, system(modprobe_cmd), "Failed modprobing msr module",
                cmd, modprobe, modprobe_cmd, NULL);
        char *instruction = (value == 1) ? "0x4000850089" : "0x850089";
        log_debug("cpu instruction bit is %s\n", instruction);
        for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
                char *buffer;
                log_debug("creating command 'wrmsr -p%d 0x1a0 %s'\n", i, instruction);
                safe_malloc(cpu, asprintf(&buffer, "%s -p%d 0x1a0 %s", cmd, i, instruction),
                        "Failed to allocate memory for writing msr of CPU",
                        cmd, modprobe, modprobe_cmd, NULL);
                log_debug("running command 'wrmsr -p%d 0x1a0 %s'\n", i, instruction);
                safe_malloc(cpu, system(buffer), "Failed using wrmsr to write to CPU",
                        cmd, modprobe, modprobe_cmd, NULL);
                log_debug("freeing buffer");
                free(buffer);
        }
        log_debug("freeing command buffers");
        free(modprobe);
        free(modprobe_cmd);
        free(cmd);
}
#endif

/*
 * Write a given integer value into a temporary string buffer.
 * Write the temporary buffer into the file specified by file_name and
 * close the file, freeing the buffer.
 */
static void internal_set(struct cpu_t *const cpu, const char *const file_name,
        const int32_t value)
{
        log_debug("internal_set: value = %d\n", value);
        char *buffer;
        safe_malloc(cpu, asprintf(&buffer, "%d", value),
                "Failed to write bytes into buffer",
                NULL);
        log_debug("internal_set: buffer = %s\n", buffer);
        FILE *file = fopen(file_name, "w");
        check_file(cpu, file, file_name);
        fprintf(file, "%s", buffer);
        fclose(file);
        free(buffer);
        log_debug("internal_set: wrote file %s, closed file and buffer\n", file_name);
}

/*
 * Write a frquency value in kilohertz into the files specified by the
 * frequency_files array
 */
static void internal_set_freq(struct cpu_t *const cpu, char **const frequency_files,
        const size_t value)
{
        log_debug("internal_set_freq: value = %zu\n", value);
        const int32_t scaling_value = cpu->CPU_INFO_MAX_FREQ;
        log_debug("internal_set_freq: scaling_value = %d\n", scaling_value);
        const size_t scaling = scaling_value / 100 * value;
        log_debug("internal_set_freq: scaling = %zu\n", scaling);
        for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
                internal_set(cpu, frequency_files[i], scaling);
        }
}

/*
 * Set the current state of Turbo Boost on the CPU. A turbo value
 * of 1 means OFF and 0 means ON
 */
void set_turbo(struct cpu_t *const cpu, const int32_t turbo)
{
        if (has_pstate(cpu)) {
                log_debug("pstate driver found\n");
                internal_set(cpu, FILE_PSTATE_TURBO, turbo);
        } else {
#if WRITE_MSR >= 1
                log_debug("WRITE_MSR is active\n");
                write_msr(cpu, turbo);
#endif
                log_error("%sError: Not able to set turbo, p-state driver not found%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        }
}

/*
 * Set the current max_perf_pct of the CPU, from a value range of
 * the minimum capable cpu frequency to 100
 */
void set_scaling_max(struct cpu_t *const cpu, const int32_t max)
{
        log_debug("set_scaling_max running with max: %d\n", max);
        internal_set_freq(cpu, cpu->CPU_MAX_FREQ_FILES, max);
        if (has_pstate(cpu)) {
                log_debug("Modifiying %s value\n", FILE_PSTATE_MAX);
                internal_set(cpu, FILE_PSTATE_MAX, max);
        }
}

/*
 * Set the current min_perf_pct of the CPU, from a value range of
 * the minimum capable cpu frequency to 99, 1 less than the capable
 * max_perf_pct
 */
void set_scaling_min(struct cpu_t *const cpu, const int32_t min)
{
        log_debug("set_scaling_min running with min: %d\n", min);
        internal_set_freq(cpu, cpu->CPU_MIN_FREQ_FILES, min);
        if (has_pstate(cpu)) {
                log_debug("Modifiying %s value\n", FILE_PSTATE_MIN);
                internal_set(cpu, FILE_PSTATE_MIN, min);
        }
}
