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

#define _GNU_SOURCE
#include <stdio.h>
#include <src/cpu.h>
#include <src/color.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "src/mhz.h"

#if WRITE_MSR >=1
static void
pyam_cpu_write_msr(
        struct pyam_cpu_t* cpu,
        const int32_t value);
#endif

static void
pyam_cpu_malloc_error(
        struct pyam_cpu_t* cpu,
        const int32_t result,
        const char* const error_message,
        ...);

static double
pyam_cpu_to_num(
        char* const line);

static void
pyam_cpu_file_error(
        struct pyam_cpu_t* const cpu,
        FILE* const file,
        const char* const file_name);

static char*
pyam_cpu_is_file_on_path(
        struct pyam_cpu_t* cpu,
        const char* const file_name);

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const size_t max);

static char*
pyam_cpu_internal_get(
    struct pyam_cpu_t* const cpu,
    FILE* const file,
    const char* file_name);

static void
pyam_cpu_internal_set(
    struct pyam_cpu_t* const cpu,
    const char* const file_name,
    const int32_t value);

static int32_t
pyam_cpu_calculate_cpuinfo_min(
        struct pyam_cpu_t* const cpu);

static int32_t
pyam_cpu_calculate_cpuinfo_max(void);

static double
pyam_cpu_get_cpuinfo_max_freq(
    struct pyam_cpu_t* const cpu);

static double
pyam_cpu_get_cpuinfo_min_freq(
    struct pyam_cpu_t* const cpu);

static void
pyam_cpu_internal_freq(
    struct pyam_cpu_t* const cpu,
    char** const frequency_files,
    const char* const scaling);

static int32_t
pyam_cpu_has_pstate_driver() {
    return access(DIR_PSTATE, F_OK) == 0;
}

static int32_t
pyam_cpu_calculate_mhz(
    struct pyam_cpu_t* const cpu);

static int32_t
pyam_cpu_calculate_mhz(
    struct pyam_cpu_t* const cpu) {
    const double mhz = estimate_MHz();
    const double max_mhz = cpu->CPUINFO_MAX_FREQ / 1000;
    return (mhz / max_mhz) * 100;
}

int32_t
pyam_cpu_get_mhz(
    struct pyam_cpu_t* const cpu) {
    return cpu->CPU_MHZ;
}

char* 
pyam_cpu_get_driver(
    struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPU_SCALING_DRIVER, "r");
    char* result = pyam_cpu_internal_get(cpu, file, FILE_CPU_SCALING_DRIVER);
    fclose(file);
    return result;
}

static int32_t
pyam_cpu_calculate_number(
        struct pyam_cpu_t* const cpu) {
    char* cat = pyam_cpu_is_file_on_path(cpu, "cat");
    if (cat == NULL) {
        fprintf(stderr, "%sCan't find cat binary on $PATH%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
        exit(6);
    }
    char* grep = pyam_cpu_is_file_on_path(cpu, "grep");
    if (grep == NULL) {
        fprintf(stderr, "%sCan't find grep binary on $PATH%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
        exit(7);
    }
    char* wc = pyam_cpu_is_file_on_path(cpu, "wc");
    if (wc == NULL) {
        fprintf(stderr, "%sCan't find wc binary on $PATH%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
        exit(8);
    }

    
    char* cmd;
    pyam_cpu_malloc_error(cpu, 
            asprintf(&cmd, "%s /proc/cpuinfo | %s processor | %s -l", cat, grep, wc),
            "Can't alloc for get_number command",
            cat, grep, wc, NULL);
    FILE* pf = popen(cmd, "r");
    const int32_t value = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, pf, cmd));
    pclose(pf);
    free(cmd);
    free(cat);
    free(grep);
    free(wc);
    return value;
}

/* int32_t */
/* pyam_cpu_get_number( */
/*         struct pyam_cpu_t* const cpu) { */
/*     return cpu->CPU_NUMBER; */
/* } */

static void
pyam_cpu_malloc_error(
        struct pyam_cpu_t* cpu,
        const int32_t result,
        const char* const error_message,
        ...) {
    if (result == -1) {
        fprintf(stderr, "%s%s%s\n", 
                PYAM_COLOR_BOLD_RED, error_message, PYAM_COLOR_OFF);
        pyam_cpu_destroy(cpu);
        va_list list;
        va_start(list, error_message);
        FILE* file = va_arg(list, FILE*);
        while (file != NULL) {
            free(file);
            file = va_arg(list, FILE*);
        }
        va_end(list);
        exit(4);
    }
}

struct pyam_cpu_t
pyam_cpu_create() {
    struct pyam_cpu_t cpu;
    cpu.CPU_NUMBER = pyam_cpu_calculate_number(&cpu);
    cpu.CPU_MAX_FREQ_FILES = malloc(sizeof(char*) * cpu.CPU_NUMBER);
    if (cpu.CPU_MAX_FREQ_FILES == NULL) {
        fprintf(stderr, "%sUnable to malloc CPU_MAX files array%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF); 
        pyam_cpu_destroy(&cpu);
        exit(1);
    }
    for (int32_t i = 0; i < cpu.CPU_NUMBER; ++i) {
        pyam_cpu_malloc_error(&cpu, 
            asprintf(&(cpu.CPU_MAX_FREQ_FILES[i]), 
                    "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", i),
            "Failed to allocate memory for MAX_FREQ_FILES",
            NULL);
    }
    cpu.CPU_MIN_FREQ_FILES = malloc(sizeof(char*) * cpu.CPU_NUMBER);
    if (cpu.CPU_MIN_FREQ_FILES == NULL) {
        fprintf(stderr, "%sUnable to malloc CPU_MIN files array%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF); 
        pyam_cpu_destroy(&cpu);
        exit(2);
    }
    for (int32_t i = 0; i < cpu.CPU_NUMBER; ++i) {
        pyam_cpu_malloc_error(&cpu,
            asprintf(&(cpu.CPU_MIN_FREQ_FILES[i]),
                "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", i),
            "Failed to allocate memory for MIN_FREQ_FILES",
            NULL);
    }
    cpu.CPUINFO_MAX_FREQ = pyam_cpu_get_cpuinfo_max_freq(&cpu);
    cpu.CPUINFO_MIN_FREQ = pyam_cpu_get_cpuinfo_min_freq(&cpu);
    cpu.CPUINFO_MIN_VALUE = pyam_cpu_calculate_cpuinfo_min(&cpu);
    cpu.CPUINFO_MAX_VALUE = pyam_cpu_calculate_cpuinfo_max();
    cpu.CPU_MHZ = pyam_cpu_calculate_mhz(&cpu);
    cpu.HAS_PSTATE = pyam_cpu_has_pstate_driver();
    return cpu;
}

void
pyam_cpu_destroy(
        struct pyam_cpu_t* cpu) {
    for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
        if (cpu->CPU_MAX_FREQ_FILES[i] != NULL) {
            free(cpu->CPU_MAX_FREQ_FILES[i]);
            cpu->CPU_MAX_FREQ_FILES[i] = NULL;
        }
        if (cpu->CPU_MIN_FREQ_FILES[i] != NULL) {
            free(cpu->CPU_MIN_FREQ_FILES[i]);
            cpu->CPU_MIN_FREQ_FILES[i] = NULL;
        }
    }
    if (cpu->CPU_MAX_FREQ_FILES != NULL) {
        free(cpu->CPU_MAX_FREQ_FILES);
    cpu->CPU_MAX_FREQ_FILES = NULL;
    }
    if (cpu->CPU_MIN_FREQ_FILES != NULL) {
        free(cpu->CPU_MIN_FREQ_FILES);
        cpu->CPU_MIN_FREQ_FILES = NULL;
    }
    cpu = NULL;
}

void
pyam_cpu_set_turbo(
        struct pyam_cpu_t* const cpu,
        const int32_t turbo) {
#if WRITE_MSR >=1
    pyam_cpu_write_msr(cpu, turbo);
#endif
    if (cpu->HAS_PSTATE) {
        pyam_cpu_internal_set(cpu, FILE_PSTATE_TURBO, turbo);
    } else {
#if DEBUG >= 1
        fprintf(stderr, "%sError: Not able to set turbo, p-state driver not found%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
#endif
    }
}

void
pyam_cpu_set_max(
        struct pyam_cpu_t* const cpu,
        const int32_t max) {
    pyam_cpu_set_freq(cpu, cpu->CPU_MAX_FREQ_FILES, max);
    if (pyam_cpu_has_pstate_driver()) {
        pyam_cpu_internal_set(cpu, FILE_PSTATE_MAX, max);
    }
}

void
pyam_cpu_set_min(
        struct pyam_cpu_t* const cpu,
        const int32_t min) {
    pyam_cpu_set_freq(cpu, cpu->CPU_MIN_FREQ_FILES, min);
    if (pyam_cpu_has_pstate_driver()) {
        pyam_cpu_internal_set(cpu, FILE_PSTATE_MIN, min);
    }
}

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const size_t max) {
    const int32_t scaling_value = cpu->CPUINFO_MAX_FREQ;
    const size_t scaling_max = scaling_value / 100 * max;
    char* buffer;
    pyam_cpu_malloc_error(cpu,
            asprintf(&buffer, "%zu\n", scaling_max),
            "Failed to allocate memory for set_freq",
            NULL);
    pyam_cpu_internal_freq(cpu, frequency_files, buffer);
    free(buffer);
}

static void
pyam_cpu_internal_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const char* scaling) {
    for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
        FILE* file = fopen(frequency_files[i], "w");
        if (file == NULL) {
            fprintf(stderr, "%sError: internal_freq opening file: %s%s\n", 
                    PYAM_COLOR_BOLD_RED, frequency_files[i], PYAM_COLOR_OFF);
            pyam_cpu_destroy(cpu);
            exit(3);
        }
        fprintf(file, "%s\n", scaling);
        fclose(file);
    }
}

int32_t
pyam_cpu_get_min(
        struct pyam_cpu_t* const cpu) {
    const double min = (pyam_cpu_get_min_freq(cpu) / cpu->CPUINFO_MAX_FREQ);
    return (min * 100);
}

int32_t
pyam_cpu_get_turbo(
        struct pyam_cpu_t* const cpu) {
    if (cpu->HAS_PSTATE) {
        FILE* file = fopen(FILE_PSTATE_TURBO, "r");
        const int32_t result = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, file, FILE_PSTATE_TURBO));
        fclose(file);
        return result;
    }
#if DEBUG >= 1
    fprintf(stderr, "%sError: Not able to get turbo, p-state driver not found%s\n",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
#endif
    return -1;
}

static void
pyam_cpu_file_error(
        struct pyam_cpu_t* const cpu,
        FILE* const file,
        const char* const file_name) {
    if (file == NULL) {
        fprintf(stderr, "%sError opening '%s' Exiting.%s\n", 
                PYAM_COLOR_BOLD_RED, file_name, PYAM_COLOR_OFF);
        pyam_cpu_destroy(cpu);
        exit(15);
    }
}

int32_t
pyam_cpu_get_max(
        struct pyam_cpu_t* const cpu) {
    const double max = (pyam_cpu_get_max_freq(cpu) / cpu->CPUINFO_MAX_FREQ);
    return (max * 100);
}

double
pyam_cpu_get_max_freq(
        struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPU_MAX_FREQ, "r");
    const int32_t result = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, file, FILE_CPU_MAX_FREQ));
    fclose(file);
    return result;
}

double
pyam_cpu_get_min_freq(
        struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPU_MIN_FREQ, "r");
    const int32_t result = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, file, FILE_CPU_MIN_FREQ));
    fclose(file);
    return result;
}

static double
pyam_cpu_get_cpuinfo_max_freq(
        struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPUINFO_MAX_FREQ, "r");
    const int32_t result = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, file, FILE_CPUINFO_MAX_FREQ));
    fclose(file);
    return result;
}

double
pyam_cpu_get_cpuinfo_max(
        struct pyam_cpu_t* const cpu) {
    return cpu->CPUINFO_MAX_VALUE;
}

double
pyam_cpu_get_cpuinfo_min(
        struct pyam_cpu_t* const cpu) {
    return cpu->CPUINFO_MIN_VALUE;
}

static int32_t
pyam_cpu_calculate_cpuinfo_min(
        struct pyam_cpu_t* const cpu) {
    const double min = cpu->CPUINFO_MIN_FREQ / cpu->CPUINFO_MAX_FREQ;
    return min * 100;
}

static int32_t
pyam_cpu_calculate_cpuinfo_max() {
    return 100;
}

static double
pyam_cpu_get_cpuinfo_min_freq(
        struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPUINFO_MIN_FREQ, "r");
    const int32_t result = pyam_cpu_to_num(pyam_cpu_internal_get(cpu, file, FILE_CPUINFO_MIN_FREQ));
    fclose(file);
    return result;
}

static void
pyam_cpu_internal_set(
        struct pyam_cpu_t* const cpu,
        const char* const file_name,
        const int32_t value) {
    char* buffer;
    pyam_cpu_malloc_error(cpu, asprintf(&buffer, "%d", value),
            "Failed to write bytes into buffer",
            NULL);
    FILE* file = fopen(file_name, "w");    
    pyam_cpu_file_error(cpu, file, file_name);
    fprintf(file, "%s", buffer);
    fclose(file);
    free(buffer);
}

char*
pyam_cpu_get_governor(
        struct pyam_cpu_t* const cpu) {
    FILE* file = fopen(FILE_CPU_GOVERNOR, "r");
    char* result = pyam_cpu_internal_get(cpu, file, FILE_CPU_GOVERNOR);
    fclose(file);
    return result;
}

void
pyam_cpu_set_governor(
        struct pyam_cpu_t* const cpu,
        const int32_t governor) {
    // if is valid governor
        pyam_cpu_internal_set(cpu, FILE_CPU_GOVERNOR, governor);
}

static char*
pyam_cpu_internal_get(
        struct pyam_cpu_t* const cpu,
        FILE* const file,
        const char* file_name) {
    pyam_cpu_file_error(cpu, file, file_name);
    char* line = NULL;
    size_t n = 0;
    if (getline(&line, &n, file) == -1) {
        fprintf(stderr, "%sCould not malloc for getline in internal_get%s\n",
                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
        pyam_cpu_destroy(cpu);
        exit(5);
    }
    return line;
}

static double
pyam_cpu_to_num(
        char* const line) {
    const double value = strtol(line, NULL, 10);
    free(line);
    return value;
}

static char*
pyam_cpu_is_file_on_path(
        struct pyam_cpu_t* cpu,
        const char* const file_name) {
    char* const REAL_PATH = getenv("PATH");
    char* PATH;
    char* DEFAULT_PATH = "/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/bin:/sbin";
    char* USING_PATH = REAL_PATH == NULL ? DEFAULT_PATH : REAL_PATH;
    pyam_cpu_malloc_error(cpu, asprintf(&PATH, "%s", USING_PATH), "Unable to copy PATH", NULL);
    const char* const delimiter = ":"; 
    char* token;
    token = strtok(PATH, delimiter);
    while (token != NULL) {
        char* cmd;
        pyam_cpu_malloc_error(cpu, 
                asprintf(&cmd, "%s/%s", token, file_name), 
                "Error allocating memory for path",
                PATH, NULL);
        if (access(cmd, F_OK) != -1) {
            free(PATH);
            return cmd;
        }
        token = strtok(NULL, delimiter);
        free(cmd);
    }
    free(PATH);
    return NULL;
}

#if WRITE_MSR >= 1
static void
pyam_cpu_write_msr(
        struct pyam_cpu_t* cpu,
        const int32_t value) {
    char* cmd = pyam_cpu_is_file_on_path(cpu, "wrmsr");
    if (cmd == NULL) {
        return;
    }
    char* modprobe = pyam_cpu_is_file_on_path(cpu, "modprobe");
    if (modprobe == NULL) {
        free(cmd);
        return;
    }
    char* modprobe_cmd;
    pyam_cpu_malloc_error(cpu, asprintf(&modprobe_cmd, "%s msr", modprobe),
            "Failed to allocate memory for modprobe msr",
            cmd, modprobe, NULL);
    pyam_cpu_malloc_error(cpu, system(modprobe_cmd), "Failed modprobing msr module", 
            cmd, modprobe, modprobe_cmd, NULL);
    char* instruction = value == 1 ? "0x4000850089" : "0x850089";
    for (int32_t i = 0; i < cpu->CPU_NUMBER; ++i) {
        char* buffer;
        pyam_cpu_malloc_error(cpu, asprintf(&buffer, "%s -p%d 0x1a0 %s", cmd, i, instruction),
            "Failed to allocate memory for writing msr of CPU",
            cmd, modprobe, modprobe_cmd, NULL);
        pyam_cpu_malloc_error(cpu, system(buffer), "Failed using wrmsr to write to CPU", 
                cmd, modprobe, modprobe_cmd, NULL);
        free(buffer);
    }
    free(modprobe);
    free(modprobe_cmd);
    free(cmd);
}
#endif
