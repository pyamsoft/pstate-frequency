#define _GNU_SOURCE
#include <stdio.h>
#include "src/cpu.h"
#include "src/cpuread.h"
#include "src/cpuwrite.h"
#include "src/color.h"
#include "src/plan.h"
#include "src/util.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

static int32_t handle_result(struct cpu_t *const cpu, const int32_t result,
        int32_t *const flag_action, int32_t *const value_min,
        int32_t *const value_max, int32_t *const value_turbo);
static void print_version(void);
static int32_t access_cpu(struct cpu_t *const cpu, int32_t *const flag_action,
        int32_t *const value_min, int32_t *const value_max,
        int32_t *const value_turbo);
static void print_help(void);
static void print_output(struct cpu_t *const cpu);
static void print_possible_set(void);

int main(int32_t argc, char **argv)
{
        struct cpu_t cpu = cpu_create();
        static int32_t flag_action = 0;
        static int32_t value_min = -1;
        static int32_t value_max = -1;
        static int32_t value_turbo = -1;

        int32_t final_result = 0;
        int32_t result = -1;
        static struct option long_options[] = {
                {"help",    no_argument,        NULL,           'h'},
                {"version", no_argument,        NULL,           'v'},
                {"get",     no_argument,        NULL,           'g'},
                {"set",     no_argument,        NULL,           's'},
                {"debug",   no_argument,        NULL,           'd'},
                {"plan",    required_argument,  NULL,           'p'},
                {"max",     required_argument,  NULL,           'm'},
                {"min",     required_argument,  NULL,           'n'},
                {"turbo",   required_argument,  NULL,           't'},
                {0,         0,                  0,              0}
                };
        while (1) {
                int32_t option_index = 0;
                result = getopt_long(argc, argv, "hvsdgp:m:n:t:", long_options, &option_index);
                if (result == -1) {
                        break;
                } else {
                        final_result = handle_result(&cpu, result, &flag_action,
                                &value_min, &value_max, &value_turbo);
                        if (final_result == -1) {
                                cpu_destroy(&cpu);
                                return 1;
                        } else if (final_result == 1) {
                                cpu_destroy(&cpu);
                                return 0;
                        }
                }
        }
        return access_cpu(&cpu, &flag_action, &value_min, &value_max, &value_turbo);
}

static int32_t handle_result(struct cpu_t *const cpu, const int32_t result,
        int32_t *const flag_action, int32_t *const value_min,
        int32_t *const value_max, int32_t *const value_turbo)
{
        int32_t plan_result;
        switch(result) {
        case 0:
                return 0;
        case 'h':
                print_help();
                return 1;
        case 'v':
                print_version();
                return 1;
        case 's':
                *flag_action = 2;
                return 0;
        case 'd':
                debug = 1;
                return 0;
        case 'g':
                *flag_action = 1;
                return 0;
        case 'p':
                plan_result = set_plan(value_min, value_max, value_turbo);
                *value_max = fix_value_range(*value_max, get_info_min(cpu) + 1, 100);
                *value_min = fix_value_range(*value_min, get_info_min(cpu), 99);
                return plan_result;
        case 'm':
                if (string_is_digits(optarg)) {
                        if (*value_max < 0) {
                                *value_max = strtol(optarg, NULL, 10);
                                *value_max = fix_value_range(*value_max, get_info_min(cpu) + 1, 100);
                        }
                return 0;
                }
                fprintf(stderr, "%sMax Frequency must be positive, non-zero integer value%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
        case 'n':
                if (string_is_digits(optarg)) {
                        if (*value_min < 0) {
                                *value_min = strtol(optarg, NULL, 10);
                                *value_min = fix_value_range(*value_min, get_info_min(cpu), 99);
                        }
                return 0;
                }
                fprintf(stderr, "%sMin Frequency must be positive, non-zero integer value%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
        case 't':
                if (string_is_digits(optarg)) {
                        if (*value_turbo < 0)
                                *value_turbo = strtol(optarg, NULL, 10);
                        return 0;
                }
                fprintf(stderr, "%sTurbo Boost must be either 0 or 1%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
        case '?':
                return -1;
        }
        return -1;
}


static int32_t access_cpu(struct cpu_t *const cpu, int32_t *const flag_action,
        int32_t *const value_min, int32_t *const value_max,
        int32_t *const value_turbo)
{
        switch(*flag_action) {
        case 2:
                if (geteuid() == 0) {
                        if (!(*value_max >= 0 || *value_min >= 0
                            || *value_turbo == 0 || *value_turbo == 1)) {
                                fprintf(stderr, "%sSet called with no target or invalid values%s\n",
                                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                                print_possible_set();
                                cpu_destroy(cpu);
                                return 1;
                        }
                log_debug("MAX is %d %d\n", (*value_max >= 0), *value_max);
                log_debug("MIN is %d %d\n", (*value_min >= 0), *value_min);
                log_debug("TURBO is %d %d\n", (*value_turbo >= 0), *value_turbo);
                const int32_t max = *value_max >= 0 ? *value_max : get_scaling_max(cpu);
                const int32_t real_max = (max > *value_min) ? max : *value_min + 1;
                const int32_t real_min = *value_min >= 0 ? *value_min : get_scaling_min(cpu);
                const int32_t real_turbo = (*value_turbo == 0 || *value_turbo == 1)
                        ? *value_turbo : get_turbo(cpu);

                // Reset values to sane defaults
                set_turbo(cpu, 0);
                set_scaling_max(cpu, 100);
                set_scaling_min(cpu, 0);

                log_debug("Set turbo = %d\n", real_turbo);
                log_debug("Set max = %d\n", real_max);
                log_debug("Set min = %d\n", real_min);
                set_turbo(cpu, real_turbo);
                set_scaling_max(cpu, real_max);
                set_scaling_min(cpu, real_min);
                print_output(cpu);
                } else {
                        fprintf(stderr, "%sRoot privilages required%s\n",
                                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                        cpu_destroy(cpu);
                        return 1;
                }
                break;
        case 1:
                print_output(cpu);
                break;
        default:
                print_help();
                break;
        }
        cpu_destroy(cpu);
        return 0;
}

static void print_possible_set()
{
        printf("Possible set values are: \n");
        printf("    -m | --max   Set the max scaling frequency to a number between 0 and 100 inclusive\n");
        printf("    -n | --min   Set the min scaling frequency to a number between 0 and 100 inclusive\n");
        printf("    -t | --turbo Set the state of turbo boost to either 1 (OFF) or 0 (ON)\n");
        printf("    -p | --plan  Set a predefined power plan\n");
}

static void print_output(struct cpu_t *const cpu)
{
        const int32_t turbo = get_turbo(cpu);
        const char* turbo_string = turbo == 0 ? "ON" : (turbo == 1 ? "OFF" : "INVALID");
        const int32_t cpu_max = get_scaling_max(cpu);
        const int32_t cpu_min = get_scaling_min(cpu);
        const int32_t max_mhz = get_scaling_max_freq(cpu);
        const int32_t min_mhz = get_scaling_min_freq(cpu);
        char* cpu_driver = get_driver(cpu);
#ifdef VERSION
        printf("%spstate-frequency %s%s%s\n",
                PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_BOLD_MAGENTA,
                VERSION, PYAM_COLOR_OFF);
#else
        printf("%spstate-frequency%s\n",
                PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_OFF);
#endif
        printf("%s    pstate::%sCPU_DRIVER  -> %s%s%s",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu_driver, PYAM_COLOR_OFF);
        printf("%s    pstate::%sNO_TURBO    -> %s%d : %s%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                turbo, turbo_string, PYAM_COLOR_OFF);
        printf("%s    pstate::%sCPU_MIN     -> %s%d%% : %dKhz%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu_min, min_mhz, PYAM_COLOR_OFF);
        printf("%s    pstate::%sCPU_MAX     -> %s%d%% : %dKhz%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu_max, max_mhz, PYAM_COLOR_OFF);
        printf("\n");
        free(cpu_driver);
}

static void print_version()
{
#ifdef VERSION
        printf("pstate-frequency version %s\n", VERSION);
#endif
        printf("pstate-frequency comes with ABSOLUTELY NO WARRANTY.\n");
        printf("This is free software, and you are welcome to redistribute it\n");
        printf("under certain conditions;\n");
}

static void print_help()
{
        print_version();
        printf("\n");
        printf("usage:\n");
        printf("\n");
        printf("    Options:\n");
        printf("        -h | --help     Display this help and exit\n");
        printf("        -v | --version  Display application version and exit\n");
        printf("        -d | --debug    Print debugging messages to stdout\n");
        printf("        -g | --get      Access current CPU values\n");
        printf("        -s | --set      Modify current CPU values (root)\n");
        printf("        -p | --plan     Set a predefined power plan (root)\n");
        printf("        -m | --max      Modify current CPU max frequency (root)\n");
        printf("        -n | --min      Modify current CPU min frequency (root)\n");
        printf("        -t | --turbo    Modify curent CPU turbo boost state (root)\n");
        printf("\n");
}
