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

int32_t
handle_result(
        const int32_t result,
        int32_t* flag_silent,
        int32_t* flag_action,
        int32_t* value_max,
        int32_t* value_turbo);

void 
print_version(void);

int32_t 
str_starts_with(
        const char* restrict string,
        const char* restrict prefix);

int32_t
access_cpu(
        struct pyam_cpu_t* cpu,
        int32_t* flag_silent,
        int32_t* flag_action,
        int32_t* value_max,
        int32_t* value_turbo);

void
print_help(void);

void
print_output(void);

int32_t
is_all_digits(
        const char* string);

int32_t
set_plan(
        int32_t* value_max,
        int32_t* value_turbo);

int
main(
        int32_t argc,
        char** argv) {

    struct pyam_cpu_t cpu = pyam_cpu_create();
    static int32_t flag_silent = 0;
    static int32_t flag_action = 0;
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
        {"turbo",   required_argument,  NULL,           't'},
        {0,         0,                  0,              0}
    };
    while (1) {
        int32_t option_index = 0;
        result = getopt_long(argc, argv, "hqvsgp:m:t:", long_options, &option_index);
        if (result == -1) {
            break;
        } else {
            final_result = handle_result(result, &flag_silent,
                    &flag_action, &value_max, &value_turbo);
            if (final_result == -1) {
                pyam_cpu_destroy(&cpu);
                return 1;
            }
        }
    }
    int32_t i = access_cpu(&cpu, &flag_silent, &flag_action, &value_max, &value_turbo);
    pyam_cpu_destroy(&cpu);
    return i;
}

int32_t
handle_result(
        const int32_t result,
        int32_t* flag_silent,
        int32_t* flag_action,
        int32_t* value_max,
        int32_t* value_turbo) {
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
            return set_plan(value_max, value_turbo);
        case 'm':
            if (is_all_digits(optarg)) {
                if (*value_max < 0) {
                    *value_max = strtol(optarg, NULL, 10);
                }
                return 0;
            }
            printf("%sMax Frequency must be positive, non-zero integer value%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return -1;
        case 't':
            if (is_all_digits(optarg)) {
                if (*value_turbo < 0) {
                    *value_turbo = strtol(optarg, NULL, 10);
                }
                return 0;
            }
            printf("%sMax Frequency must be either 0 or 1%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return -1;
        case '?':
            return -1;
    }
    return -1;
}

int32_t
set_plan(
        int32_t* value_max,
        int32_t* value_turbo) {
    if (str_starts_with("powersave", optarg)) {
        const int32_t min = pyam_cpu_get_min();
        *value_max = min;
        *value_turbo = 0;
    } else if (strncmp("1", optarg, strlen(optarg)) == 0) {
        const int32_t min = pyam_cpu_get_min();
        *value_max = min;
        *value_turbo = 0;
    } else if (str_starts_with("performance", optarg)) {
        const int32_t non_turbo_max = pyam_cpu_get_mhz();
        *value_max = non_turbo_max;
        *value_turbo = 0;
    } else if (strncmp("2", optarg, strlen(optarg)) == 0) {
        const int32_t non_turbo_max = pyam_cpu_get_mhz();
        *value_max = non_turbo_max;
        *value_turbo = 0;
    } else if (str_starts_with("max-performance", optarg)) {
        *value_max = 100;
        *value_turbo = 1;
    } else if (strncmp("3", optarg, strlen(optarg)) == 0) {
        *value_max = 100;
        *value_turbo = 1;
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

int32_t
is_all_digits(
        const char* string) {
    const int32_t length = strlen(string);
    for (int32_t i = 0; i < length; ++i) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

int32_t
access_cpu(
        struct pyam_cpu_t* cpu,
        int32_t* flag_silent,
        int32_t* flag_action,
        int32_t* value_max,
        int32_t* value_turbo) {
    if (*flag_action == 2) {
        //set
        if (geteuid() == 0) {
            // is effective root
            int32_t requested = 0;
            if (*value_max >= 0) {
                requested = 1;
                pyam_cpu_set_max(cpu, *value_max);
            }
            if (*value_turbo == 0
                    || *value_turbo == 1) {
                requested = 1;
                pyam_cpu_set_turbo(*value_turbo);
            }
            if (requested == 0) {
                printf("%sSet called with no target or invalid values%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return 2;
            }
            // print out
            if (*flag_silent == 0) {
                print_output();
            }
        } else {
            printf("%sRoot privilages required%s\n",
                    PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
            return 2;
        }
    } else if (*flag_action == 1) {
        // get
        // ignore silent flag
        print_output();
    } else {
        // display help
        print_help();
    }
    return 0;
}

void
print_output() {
    int32_t turbo = pyam_cpu_get_turbo();
    const char* turbo_string = turbo == 1 ? "ON" : "OFF";
    int32_t cpu_max = pyam_cpu_get_max();
    int32_t cpu_freq = pyam_cpu_get_freq();
    int32_t max_mhz = cpu_freq / 1000;
#ifdef VERSION
    printf("%spstate-frequency %s%s%s\n", 
            PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_BOLD_MAGENTA,
            VERSION,
            PYAM_COLOR_OFF);
#else
    printf("%spstate-frequency%s\n", 
            PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_OFF);
#endif
    printf("%s    pstate::%sTURBO     -> %s%d : %s%s\n", 
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            turbo, turbo_string,
            PYAM_COLOR_OFF);
    printf("%s    pstate::%sCPU_MAX   -> %s%d%% : %dMhz%s\n", 
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            cpu_max, max_mhz,
            PYAM_COLOR_OFF);
    printf("%s    pstate::%sFREQUENCY -> %s%dKHz%s\n",
            PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
            cpu_freq,
            PYAM_COLOR_OFF);
    printf("\n");
}

void
print_version() {
#ifdef VERSION
    printf("pstate-frequency version %s\n", VERSION);
#endif
    printf("pstate-frequency comes with ABSOLUTELY NO WARRANTY.\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions;\n");
}

void
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
    printf("        -t | --turbo    Modify curent CPU turbo boost state (root)\n");
    printf("\n");
}

int32_t 
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
