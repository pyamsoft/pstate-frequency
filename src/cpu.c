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
#include <stdlib.h>
#include <unistd.h>
#include "src/mhz.h"

static char*
pyam_cpu_get_file_contents_str(
        struct pyam_cpu_t* const cpu,
        const char* file_name);

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const size_t max);

static double
pyam_cpu_internal_get(
        struct pyam_cpu_t* const cpu,
        const char* const file_name);

static void
pyam_cpu_internal_set(
        struct pyam_cpu_t* const cpu,
        const char* const file_name,
        const int32_t value);

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

int32_t
pyam_cpu_get_mhz(
        struct pyam_cpu_t* const cpu) {
    const double mhz = estimate_MHz();
    const double max_mhz = pyam_cpu_get_cpuinfo_max_freq(cpu) / 1000;
    return (mhz / max_mhz) * 100;
}

char* 
pyam_cpu_get_driver(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_get_file_contents_str(cpu, FILE_CPU_SCALING_DRIVER);
}

static char*
pyam_cpu_get_file_contents_str(
        struct pyam_cpu_t* const cpu,
        const char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Error: internal_get opening file: %s\n", FILE_CPU_SCALING_DRIVER);
        pyam_cpu_destroy(cpu);
        exit(10);
    }
    char* line = NULL;
    size_t n = 0;
    getline(&line, &n, file);
    fclose(file);
    return line;
}

int32_t
pyam_cpu_get_number(
        struct pyam_cpu_t* const cpu) {
    if (access(FILE_CPU_NUMBER, F_OK) == -1) {
        // Create the file
        char* command;
        if (asprintf(&command, "cat /proc/cpuinfo | grep processor | wc -l > %s", FILE_CPU_NUMBER) == -1) {
            printf("Unable to alloc system command: %s\n", command);
            pyam_cpu_destroy(cpu);
            exit(6);
        }
        // Make it world readable
        char* readable;
        if (asprintf(&readable, "chmod a+r %s", FILE_CPU_NUMBER) == -1) {
            printf("Unable to alloc system command: %s\n", readable);
            pyam_cpu_destroy(cpu);
            exit(6);
        }
        system(command);
        system(readable);
        free(command);
        free(readable);
    }
    return pyam_cpu_internal_get(cpu, FILE_CPU_NUMBER);
}

struct pyam_cpu_t
pyam_cpu_create() {
    struct pyam_cpu_t cpu;
    const int32_t cpu_number = pyam_cpu_get_number(&cpu);
    cpu.CPU_MAX_FREQ_FILES = malloc(sizeof(char*) * cpu_number);
    if (cpu.CPU_MAX_FREQ_FILES == NULL) {
        printf("Unable to malloc CPU_MAX files array\n"); 
        pyam_cpu_destroy(&cpu);
        exit(4);
    }
    for (int32_t i = 0; i < cpu_number; ++i) {
        if (asprintf(&(cpu.CPU_MAX_FREQ_FILES[i]), 
                "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", i) == -1) {
            printf("Failed to allocate memory for CPU_MAX files array[%d]\n", i);
            pyam_cpu_destroy(&cpu);
            exit(4);
        }
    }
    cpu.CPU_MIN_FREQ_FILES = malloc(sizeof(char*) * cpu_number);
    if (cpu.CPU_MIN_FREQ_FILES == NULL) {
        printf("Unable to malloc CPU_MIN files array\n"); 
        pyam_cpu_destroy(&cpu);
        exit(4);
    }
    for (int32_t i = 0; i < cpu_number; ++i) {
        if (asprintf(&(cpu.CPU_MIN_FREQ_FILES[i]),
                "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq", i) == -1) {
            printf("Failed to allocate memory for CPU_MIN files array[%d]\n", i);
            pyam_cpu_destroy(&cpu);
            exit(4);
        }
    }
    return cpu;
}

void
pyam_cpu_destroy(
        struct pyam_cpu_t* cpu) {
    const int32_t cpu_number = pyam_cpu_get_number(cpu);
    for (int32_t i = 0; i < cpu_number; ++i) {
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
    if (pyam_cpu_has_pstate_driver()) {
        pyam_cpu_internal_set(cpu, FILE_PSTATE_TURBO, turbo);
    }
#if DEBUG >= 1
    printf("Error: Not able to set turbo, p-state driver not found\n");
#endif
}

void
pyam_cpu_set_max(
        struct pyam_cpu_t* const cpu,
        const int32_t max) {
    pyam_cpu_set_freq(cpu, cpu->CPU_MAX_FREQ_FILES, max);

    /* It appears that this is not needed, the p-state driver will */
    /* adjust itself when the frequency changes */
    /* pyam_cpu_internal_set(FILE_PSTATE_MAX_PERCENT, max); */
}

void
pyam_cpu_set_min(
        struct pyam_cpu_t* const cpu,
        const int32_t min) {
    pyam_cpu_set_freq(cpu, cpu->CPU_MIN_FREQ_FILES, min);
    /* It appears that this is not needed, the p-state driver will */
    /* adjust itself when the frequency changes */
    /* pyam_cpu_internal_set(FILE_PSTATE_MIN_PERCENT, min); */
}

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const size_t max) {
    const int32_t scaling_value = pyam_cpu_get_cpuinfo_max_freq(cpu);
    const size_t scaling_max = scaling_value / 100 * max;
    char* buffer;
    if (asprintf(&buffer, "%zu\n", scaling_max) == -1) {
        printf("Failed to allocate memory for set_freq\n");
        pyam_cpu_destroy(cpu);
        exit(5);
    }
    pyam_cpu_internal_freq(cpu, frequency_files, buffer);
    free(buffer);
}

static void
pyam_cpu_internal_freq(
        struct pyam_cpu_t* const cpu,
        char** const frequency_files,
        const char* scaling) {
    const int32_t cpu_number = pyam_cpu_get_number(cpu);
    for (int32_t i = 0; i < cpu_number; ++i) {
        FILE* file = fopen(frequency_files[i], "w");
        if (file == NULL) {
            printf("Error: internal_freq opening file: %s\n", frequency_files[i]);
            pyam_cpu_destroy(cpu);
            exit(7);
        }
        fprintf(file, "%s\n", scaling);
        fclose(file);
    }
}

int32_t
pyam_cpu_get_min(
        struct pyam_cpu_t* const cpu) {
    const double min = (pyam_cpu_get_min_freq(cpu) / pyam_cpu_get_cpuinfo_max_freq(cpu));
    return (min * 100);
}

int32_t
pyam_cpu_get_turbo(
        struct pyam_cpu_t* const cpu) {
    if (pyam_cpu_has_pstate_driver()) {
        return pyam_cpu_internal_get(cpu, FILE_PSTATE_TURBO);
    }
#if DEBUG >= 1
    printf("Error: Not able to get turbo, p-state driver not found\n");
#endif
    return -1;
}

int32_t
pyam_cpu_get_max(
        struct pyam_cpu_t* const cpu) {
    const double max = (pyam_cpu_get_max_freq(cpu) / pyam_cpu_get_cpuinfo_max_freq(cpu));
    return (max * 100);
}

double
pyam_cpu_get_max_freq(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_internal_get(cpu, FILE_CPU_MAX_FREQ);
}

double
pyam_cpu_get_min_freq(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_internal_get(cpu, FILE_CPU_MIN_FREQ);
}

static double
pyam_cpu_get_cpuinfo_max_freq(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_internal_get(cpu, FILE_CPUINFO_MAX_FREQ);
}

double
pyam_cpu_get_cpuinfo_max() {
    return 100;
}

double
pyam_cpu_get_cpuinfo_min(
        struct pyam_cpu_t* const cpu) {
    const double min = pyam_cpu_get_cpuinfo_min_freq(cpu) / pyam_cpu_get_cpuinfo_max_freq(cpu);
    return min * 100;
}

static double
pyam_cpu_get_cpuinfo_min_freq(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_internal_get(cpu, FILE_CPUINFO_MIN_FREQ);
}

static void
pyam_cpu_internal_set(
        struct pyam_cpu_t* const cpu,
        const char* const file_name,
        const int32_t value) {
    char* buffer;
    if (asprintf(&buffer, "%d", value) == -1) {
        printf("Failed to write bytes into buffer\n");
        pyam_cpu_destroy(cpu);
        exit(6);
    }
    FILE* file = fopen(file_name, "w");    
    if (file == NULL) {
        printf("Error: internal_set opening file: %s\n", file_name);
        pyam_cpu_destroy(cpu);
        exit(8);
    }
    fprintf(file, "%s", buffer);
    fclose(file);
    free(buffer);
}

char*
pyam_cpu_get_governor(
        struct pyam_cpu_t* const cpu) {
    return pyam_cpu_get_file_contents_str(cpu, FILE_CPU_GOVERNOR);
}

void
pyam_cpu_set_governor(
        struct pyam_cpu_t* const cpu,
        const int32_t governor) {
    // if is valid governor
        pyam_cpu_internal_set(cpu, FILE_CPU_GOVERNOR, governor);
}

static double
pyam_cpu_internal_get(
        struct pyam_cpu_t* const cpu,
        const char* const file_name) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Error: internal_get opening file: %s\n", file_name);
        pyam_cpu_destroy(cpu);
        exit(9);
    }
    char* line = NULL;
    size_t n = 0;
    getline(&line, &n, file);
    const double value = strtol(line, NULL, 10);
    fclose(file);
    free(line);
    return value;
}
