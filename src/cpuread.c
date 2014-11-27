#define _GNU_SOURCE
#include <stdio.h>
#include "src/color.h"
#include "src/cpuutil.h"
#include "src/cpuread.h"
#include "src/util.h"
#include <string.h>
#include <stdlib.h>

static char *internal_get(struct cpu_t *const cpu,
        FILE *const file, const char *const file_name);
static double internal_get_cpu_value(struct cpu_t *const cpu,
        const char *const file_name);

char *get_driver(struct cpu_t *const cpu)
{
        log_debug("Open file %s for driver\n", FILE_SCALING_DRIVER);
        FILE *file = fopen(FILE_SCALING_DRIVER, "r");
        check_file(cpu, file, FILE_SCALING_DRIVER);
        char *result = internal_get(cpu, file, FILE_SCALING_DRIVER);
        log_debug("driver is: %s", result);
        fclose(file);
        return result;
}


static char *internal_get(struct cpu_t *const cpu,
        FILE *const file, const char *const file_name)
{
        check_file(cpu, file, file_name);
        char *line = NULL;
        size_t n = 0;
        log_debug("Getting a line from file %s\n", file_name);
        if (getline(&line, &n, file) == -1) {
                fprintf(stderr, "%sCould not malloc for getline in internal_get%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                cpu_destroy(cpu);
                exit(1);
        }
        log_debug("Line is: %s", line);
        return line;
}

int32_t has_pstate(struct cpu_t *const cpu)
{
        char *driver = get_driver(cpu);
        const int32_t result = strncmp(driver, "intel_pstate\n", strlen(driver));
        free(driver);
        return (result == 0);
}

int32_t get_cpu_number(struct cpu_t *const cpu)
{
        log_debug("Finding cat, grep, and wc on path\n");
        char *cat = resolve_path_to_file(cpu, "cat");
        char *grep = resolve_path_to_file(cpu, "grep");
        char *wc = resolve_path_to_file(cpu, "wc");
        char *cmd;
        safe_malloc(cpu, asprintf(&cmd, "%s /proc/cpuinfo | %s processor | %s -l", cat, grep, wc),
                "Can't alloc for get_number command",
                cat, grep, wc, NULL);
        log_debug("Opening a pipe for cmd: %s\n", cmd);
        FILE *pf = popen(cmd, "r");
        const int32_t value = str_to_num(internal_get(cpu, pf, cmd));
        log_debug("Freeing pipe resources\n");
        pclose(pf);
        free(cmd);
        free(cat);
        free(grep);
        free(wc);
        log_debug("Cpu_number: %d\n", value);
        return value;
}

static double internal_get_cpu_value(struct cpu_t *const cpu,
        const char *const file_name)
{
        log_debug("Getting a cpu value from file %s\n", file_name);
        FILE *file = fopen(file_name, "r");
        check_file(cpu, file, file_name);
        const double value = str_to_num(internal_get(cpu, file, file_name));
        fclose(file);
        return value;
}

double get_scaling_max_freq(struct cpu_t *const cpu)
{
        return internal_get_cpu_value(cpu, FILE_SCALING_MAX_FREQ);
}

double get_scaling_min_freq(struct cpu_t *const cpu)
{
        return internal_get_cpu_value(cpu, FILE_SCALING_MIN_FREQ);
}

double get_info_max_freq(struct cpu_t *const cpu)
{
        return internal_get_cpu_value(cpu, FILE_INFO_MAX_FREQ);
}

double get_info_min_freq(struct cpu_t *const cpu)
{
        return internal_get_cpu_value(cpu, FILE_INFO_MIN_FREQ);
}

int32_t get_turbo(struct cpu_t *const cpu)
{
        if (has_pstate(cpu)) {
                log_debug("Opening pstate file %s\n", FILE_PSTATE_TURBO);
                FILE *file = fopen(FILE_PSTATE_TURBO, "r");
                check_file(cpu, file, FILE_PSTATE_TURBO);
                const int32_t result = str_to_num(internal_get(cpu, file, FILE_PSTATE_TURBO));
                fclose(file);
                return result;
        }
        log_error("%sError: Not able to get turbo, p-state driver not found%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        return -1;
}

double get_info_min(struct cpu_t *const cpu)
{
        const double max_freq = get_info_max_freq(cpu);
        log_debug("max_freq: %f\n", max_freq);
        const double result = (get_info_min_freq(cpu) / max_freq) * 100;
        log_debug("info_min result: %f\n", result);
        return result;
}

double get_scaling_max(struct cpu_t *const cpu)
{
        const double max_freq = get_info_max_freq(cpu);
        log_debug("max_freq: %f\n", max_freq);
        const double result = (get_scaling_max_freq(cpu) / max_freq) * 100;
        log_debug("scaling_max result: %f\n", result);
        return result;
}

double get_scaling_min(struct cpu_t *const cpu)
{
        const double max_freq = get_info_max_freq(cpu);
        log_debug("max_freq: %f\n", max_freq);
        const double result = (get_scaling_min_freq(cpu) / max_freq) * 100;
        log_debug("scaling_min result: %f\n", result);
        return result;

}
