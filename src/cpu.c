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

#include <src/cpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "src/mhz.h"

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        const size_t max);

static int32_t
pyam_cpu_internal_get(
        const char* file_name);

static void
pyam_cpu_internal_set(
        const char* file_name,
        const int32_t value);

static int32_t
pyam_cpu_get_cpuinfo_freq(void);

static void
pyam_cpu_internal_freq(
        struct pyam_cpu_t* cpu, const char* scaling);

int32_t
pyam_cpu_get_mhz() {
    const double mhz = estimate_MHz();
    const double max_mhz = pyam_cpu_get_cpuinfo_freq() / 1000;
    return (mhz / max_mhz) * 100;
}

int32_t
pyam_cpu_get_number() {
    const char* file_name = "/tmp/cpunumber.txt";
    if (access(file_name, F_OK) == -1) {
        // Create the file
        char* command;
        if (asprintf(&command, "cat /proc/cpuinfo | grep processor | wc -l > %s", file_name) == -1) {
            printf("Unable to alloc system command: %s\n", command);
            exit(6);
        }
        // Make it world readable
        char* readable;
        if (asprintf(&readable, "chmod a+r %s", file_name) == -1) {
            printf("Unable to alloc system command: %s\n", readable);
            exit(6);
        }
        system(command);
        system(readable);
        free(command);
        free(readable);
    }
    return pyam_cpu_internal_get(file_name);
}

struct pyam_cpu_t
pyam_cpu_create(void) {
    struct pyam_cpu_t cpu;
    const int32_t cpu_number = pyam_cpu_get_number();
    cpu.CPU_FREQ_FILES = malloc(sizeof(char*) * cpu_number);
    if (cpu.CPU_FREQ_FILES == NULL) {
        printf("Unable to malloc CPU files array\n"); 
        exit(4);
    }
    for (int32_t i = 0; i < cpu_number; ++i) {
        if (asprintf(&(cpu.CPU_FREQ_FILES[i]), 
                "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq", i) == -1) {
            printf("Failed to allocate memory for CPU files array[%d]\n", i);
            exit(4);
        }
    }
    return cpu;
}

void
pyam_cpu_destroy(
        struct pyam_cpu_t* cpu) {
    const int32_t cpu_number = pyam_cpu_get_number();
    for (int32_t i = 0; i < cpu_number; ++i) {
        free(cpu->CPU_FREQ_FILES[i]);
        cpu->CPU_FREQ_FILES[i] = NULL;
    }
    free(cpu->CPU_FREQ_FILES);
    cpu->CPU_FREQ_FILES = NULL;
}

void
pyam_cpu_set_turbo(
        const int32_t turbo) {
    pyam_cpu_internal_set(FILE_PSTATE_TURBO, turbo);
}

void
pyam_cpu_set_max(
        struct pyam_cpu_t* const cpu,
        const int32_t max) {
    size_t real_max = 0;
    // Get the minimum cpu value
    const int32_t min = pyam_cpu_get_min();
    if (max < min) {
        real_max = min;
    } else if (max > 100) {
        real_max = 100;
    } else {
        real_max = max;
    }
    pyam_cpu_set_freq(cpu, real_max);

    /* It appears that this is not needed, the p-state driver will */
    /* adjust itself when the frequency changes */
    /* pyam_cpu_internal_set(FILE_PSTATE_MAX_PERCENT, real_max); */
}

static void
pyam_cpu_set_freq(
        struct pyam_cpu_t* const cpu,
        const size_t max) {
    const int32_t scaling_value = pyam_cpu_get_cpuinfo_freq();
    const size_t scaling_max = scaling_value / 100 * max;
    char* buffer;
    if (asprintf(&buffer, "%zu\n", scaling_max) == -1) {
        printf("Failed to allocate memory for set_freq\n");
        exit(5);
    }
    pyam_cpu_internal_freq(cpu, buffer);
    free(buffer);
}

static void
pyam_cpu_internal_freq(
        struct pyam_cpu_t* cpu, 
        const char* scaling) {
    const int32_t cpu_number = pyam_cpu_get_number();
    for (int32_t i = 0; i < cpu_number; ++i) {
        FILE* file = fopen(cpu->CPU_FREQ_FILES[i], "w");
        fprintf(file, "%s\n", scaling);
        fclose(file);
    }
}

int32_t
pyam_cpu_get_min() {
    return pyam_cpu_internal_get(FILE_PSTATE_MIN_PERCENT) + 1;
}

int32_t
pyam_cpu_get_turbo() {
    return pyam_cpu_internal_get(FILE_PSTATE_TURBO);
}

int32_t
pyam_cpu_get_max() {
    return pyam_cpu_internal_get(FILE_PSTATE_MAX_PERCENT);
}

int32_t
pyam_cpu_get_freq() {
    return pyam_cpu_internal_get(FILE_CPU_FREQ);
}

static int32_t
pyam_cpu_get_cpuinfo_freq() {
    return pyam_cpu_internal_get(FILE_CPU_MAX);
}

static void
pyam_cpu_internal_set(
        const char* file_name,
        const int32_t value) {
    char* buffer;
    if (asprintf(&buffer, "%d", value) == -1) {
        printf("Failed to write bytes into buffer\n");
        exit(6);
    }
    FILE* file = fopen(file_name, "w");    
    fprintf(file, "%s", buffer);
    fclose(file);
    free(buffer);
}

static int32_t
pyam_cpu_internal_get(
        const char* file_name) {
    FILE* file = fopen(file_name, "r");
    char* line = NULL;
    size_t n = 0;
    getline(&line, &n, file);
    const int32_t value = strtol(line, NULL, 10);
    fclose(file);
    free(line);
    return value;
}
