/*
    pstate-frequency Easier control of the Intel p-state driver

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
#include "src/cpu.h"
#include "src/color.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

static int32_t
handle_result(
        struct pyam_cpu_t* const cpu,
        const int32_t result,
        int32_t* const flag_silent,
        int32_t* const flag_action,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

static void 
print_version(void);

static int32_t 
str_starts_with(
        const char* restrict string,
        const char* restrict prefix);

static int32_t
access_cpu(
        struct pyam_cpu_t* const cpu,
        int32_t* const flag_silent,
        int32_t* const flag_action,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

static void
fix_value_range(
        int32_t* const value,
        const int32_t min);

static void
print_help(void);

static void
print_output(
        struct pyam_cpu_t* const cpu);

static int32_t
is_all_digits(
        const char* const string);

static int32_t
set_plan(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

static void
set_powersave(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

static void
set_performance(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

static void
set_max_performance(
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo);

int
main(
        int32_t argc,
        char** argv) {

    struct pyam_cpu_t cpu = pyam_cpu_create();
    static int32_t flag_silent = 0;
    static int32_t flag_action = 0;
    static int32_t value_min = -1;
    static int32_t value_max = -1;
    static int32_t value_turbo = -1;

    int32_t final_result = 0;

    int32_t result = -1;
    static struct option long_options[] = {
        {"help",    no_argument,        NULL,           'h'},
        {"version", no_argument,        NULL,           'v'},
        {"quiet",   no_argument,        &flag_silent,   1},
        {"get",     no_argument,        &flag_action,   1},
        {"set",     no_argument,        &flag_action,   2},
        {"plan",    required_argument,  NULL,           'p'},
        {"max",     required_argument,  NULL,           'm'},
        {"min",     required_argument,  NULL,           'n'},
        {"turbo",   required_argument,  NULL,           't'},
        {0,         0,                  0,              0}
    };
    while (1) {
        int32_t option_index = 0;
        result = getopt_long(argc, argv, "hqvsgp:m:n:t:", long_options, &option_index);
        if (result == -1) {
            break;
        } else {
            final_result = handle_result(&cpu, result, &flag_silent,
                    &flag_action, &value_min, &value_max, &value_turbo);
            if (final_result == -1) {
                pyam_cpu_destroy(&cpu);
                return 1;
            }
        }
    }
    int32_t i = access_cpu(&cpu, &flag_silent, &flag_action, &value_min, &value_max, &value_turbo);
    pyam_cpu_destroy(&cpu);
    return i;
}

static int32_t
handle_result(
        struct pyam_cpu_t* const cpu,
        const int32_t result,
        int32_t* const flag_silent,
        int32_t* const flag_action,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    switch(result) {
        case 0:
            return 0;
        case 'h':
            print_help();
            return -1;
        case 'v':
            print_version();
            return -1;
        case 'q':
            *flag_silent = 1;
            return 0;
        case 's':
            *flag_action = 2;
            return 0;
        case 'g':
            *flag_action = 1;
            return 0;
        case 'p':
            return set_plan(cpu, value_min ,value_max, value_turbo);
        case 'm':
            if (is_all_digits(optarg)) {
                if (*value_max < 0) {
                    *value_max = strtol(optarg, NULL, 10);
                    fix_value_range(value_max, pyam_cpu_get_cpuinfo_min(cpu) + 1);
                }
                return 0;
            }
            printf("%sMax Frequency must be positive, non-zero integer value%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return -1;
        case 'n':
            if (is_all_digits(optarg)) {
                if (*value_min < 0) {
                    *value_min = strtol(optarg, NULL, 10);
                    fix_value_range(value_min, pyam_cpu_get_cpuinfo_min(cpu));
                }
                return 0;
            }
            printf("%sMin Frequency must be positive, non-zero integer value%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return -1;
        case 't':
            if (is_all_digits(optarg)) {
                if (*value_turbo < 0) {
                    *value_turbo = strtol(optarg, NULL, 10);
                }
                return 0;
            }
            printf("%sTurbo Boost must be either 0 or 1%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return -1;
        case '?':
            return -1;
    }
    return -1;
}

static void
fix_value_range(
        int32_t* const value,
        const int32_t min) {
    const int32_t max = pyam_cpu_get_cpuinfo_max();
    if (*value < min) {
        *value = min;
    } else if (*value > max) {
        *value = max;
    }
}

static int32_t
set_plan(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    if (str_starts_with("powersave", optarg) 
            || strncmp("1", optarg, strlen(optarg)) == 0) {
        set_powersave(cpu, value_min, value_max, value_turbo);
    } else if (str_starts_with("performance", optarg)
            || strncmp("2", optarg, strlen(optarg)) == 0) {
        set_performance(cpu, value_min, value_max, value_turbo);
    } else if (str_starts_with("max-performance", optarg) 
            || strncmp("3", optarg, strlen(optarg)) == 0) {
        set_max_performance(value_min, value_max, value_turbo);
    } else {
        printf("%sPlan: Unrecognized plan: %s%s%s\n",
                PYAM_COLOR_NORMAL_RED, PYAM_COLOR_BOLD_RED,
                optarg,
                PYAM_COLOR_OFF);
        printf("%sValid plans are:%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        printf("%s    1: powersave%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        printf("%s    2: performance%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        printf("%s    3: max-performance%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
        return -1;
    }
    return 0;
}

static void
set_powersave(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    const int32_t min = pyam_cpu_get_cpuinfo_min(cpu);
    *value_min = min;
    *value_max = min + 1;
    *value_turbo = 0;
}

static void
set_performance(
        struct pyam_cpu_t* const cpu,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    const int32_t non_turbo_max = pyam_cpu_get_mhz(cpu);
    const int32_t min = pyam_cpu_get_cpuinfo_min(cpu);
    *value_min = min;
    *value_max = non_turbo_max;
    *value_turbo = 0;
}

static void
set_max_performance(
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    *value_min = 99;
    *value_max = 100;
    *value_turbo = 1;
}

static int32_t
is_all_digits(
        const char* const string) {
    const int32_t length = strlen(string);
    for (int32_t i = 0; i < length; ++i) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

static int32_t
access_cpu(
        struct pyam_cpu_t* const cpu,
        int32_t* const flag_silent,
        int32_t* const flag_action,
        int32_t* const value_min,
        int32_t* const value_max,
        int32_t* const value_turbo) {
    if (*flag_action == 2) {
        //set
        if (geteuid() == 0) {
            // is effective root
            int32_t requested = 0;
            if (*value_max >= 0
                    && *value_min >= 0) {
                /*
                   If both the min and max are requested to change,
                   you must change in proper order. Max can not be <= min,
                   and min can not be >= max. You must change max first
                   if it is greater than current min, and min first if
                   it is greater than current max otherwise no changes will take effect
                */
                requested = 1;
                if (*value_max > pyam_cpu_get_min(cpu)) {
                    pyam_cpu_set_max(cpu, *value_max);
                    pyam_cpu_set_min(cpu, *value_min);
                } else { // (*value_min >= pyam_cpu_get_max()) {
                    pyam_cpu_set_min(cpu, *value_min);
                    pyam_cpu_set_max(cpu, *value_max);
                }
            } else {
                if (*value_max >= 0) {
                    requested = 1;
                    pyam_cpu_set_max(cpu, *value_max);
                }
                if (*value_min >= 0) {
                    requested = 1;
                    pyam_cpu_set_min(cpu, *value_min);
                }
            }
            if (*value_turbo == 0
                    || *value_turbo == 1) {
                requested = 1;
                pyam_cpu_set_turbo(cpu, *value_turbo);
            }
            if (requested == 0) {
                printf("%sSet called with no target or invalid values%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return 2;
            }
            // print out
            if (*flag_silent == 0) {
                print_output(cpu);
            }
        } else {
            printf("%sRoot privilages required%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return 2;
        }
    } else if (*flag_action == 1) {
        // get
        // ignore silent flag
        print_output(cpu);
    } else {
        // display help
        print_help();
    }
    return 0;
}

static void
print_output(
        struct pyam_cpu_t* const cpu) {
    const int32_t turbo = pyam_cpu_get_turbo(cpu);
    const char* turbo_string = turbo == 1 ? "ON" : (turbo == 0 ? "OFF" : "INVALID");
    const int32_t cpu_max = pyam_cpu_get_max(cpu);
    const int32_t cpu_min = pyam_cpu_get_min(cpu);
    const int32_t max_mhz = pyam_cpu_get_max_freq(cpu);
    const int32_t min_mhz = pyam_cpu_get_min_freq(cpu);
    char* cpu_driver = pyam_cpu_get_driver(cpu);
#ifdef VERSION
    printf("%spstate-frequency %s%s%s\n",
            PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_BOLD_MAGENTA,
            VERSION,
            PYAM_COLOR_OFF);
#else
    printf("%spstate-frequency%s\n",
            PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_OFF);
#endif
    printf("%s    pstate::%sCPU_DRIVER  -> %s%s%s",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            cpu_driver,
            PYAM_COLOR_OFF);
    printf("%s    pstate::%sCPU_TURBO   -> %s%d : %s%s\n",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            turbo, turbo_string,
            PYAM_COLOR_OFF);
    printf("%s    pstate::%sCPU_MIN     -> %s%d%% : %dKhz%s\n",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            cpu_min, min_mhz,
            PYAM_COLOR_OFF);
    printf("%s    pstate::%sCPU_MAX     -> %s%d%% : %dKhz%s\n",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            cpu_max, max_mhz,
            PYAM_COLOR_OFF);
    printf("\n");
    free(cpu_driver);
}

static void
print_version() {
#ifdef VERSION
    printf("pstate-frequency version %s\n", VERSION);
#endif
    printf("pstate-frequency comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions;\n");
}

static void
print_help() {
    print_version();
    printf("\n");
    printf("usage:\n");
    printf("\n");
    printf("    Options:\n");
    printf("        -h | --help     Display this help and exit\n");
    printf("        -v | --version  Display application version and exit\n");
    printf("        -q | --quiet    Suppress normal application output\n");
    printf("        -s | --set      Modify current CPU values (root)\n");
    printf("        -g | --get      Access current CPU values\n");
    printf("        -p | --plan     Set a predefined power plan (root)\n");
    printf("        -m | --max      Modify current CPU max frequency (root)\n");
    printf("        -n | --min      Modify current CPU min frequency (root)\n");
    printf("        -t | --turbo    Modify curent CPU turbo boost state (root)\n");
    printf("\n");
}

static int32_t
str_starts_with(
        const char* restrict string,
        const char* restrict prefix) {
    while (*prefix) {
        if (*prefix++ != *string++) {
            return 0;
        } 
    }
    return 1;
}
