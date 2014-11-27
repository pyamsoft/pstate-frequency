#define _GNU_SOURCE
#include <stdio.h>
#include "src/plan.h"
#include "src/util.h"
#include "src/color.h"
#include <string.h>
#include <getopt.h>

static void set_powersave(int32_t *const value_min, int32_t *const value_max,
                int32_t *const value_turbo);

static void set_performance(int32_t *const value_min, int32_t *const value_max,
                int32_t *const value_turbo);

static void set_max_performance(int32_t *const value_min, int32_t *const value_max,
                int32_t *const value_turbo);

int32_t set_plan(int32_t *const value_min, int32_t *const value_max,
                int32_t *const value_turbo)
{
        if (str_starts_with("powersave", optarg) 
            || strncmp("1", optarg, strlen(optarg)) == 0) {
                set_powersave(value_min, value_max, value_turbo);
        } else if (str_starts_with("performance", optarg)
            || strncmp("2", optarg, strlen(optarg)) == 0) {
                set_performance(value_min, value_max, value_turbo);
        } else if (str_starts_with("max-performance", optarg)
            || strncmp("3", optarg, strlen(optarg)) == 0) {
                set_max_performance(value_min, value_max, value_turbo);
        } else {
                fprintf(stderr, "%sPlan: Unrecognized plan: %s%s%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_BOLD_RED,
                        optarg, PYAM_COLOR_OFF);
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

static void set_powersave(int32_t *const value_min,
        int32_t *const value_max, int32_t *const value_turbo)
{
    *value_min = 0;
    *value_max = 0;
    *value_turbo = 1;
}

static void set_performance(int32_t *const value_min,
        int32_t *const value_max, int32_t *const value_turbo)
{
    *value_min = 0;
    *value_max = 100;
    *value_turbo = 1;
}

static void set_max_performance(int32_t *const value_min,
        int32_t *const value_max, int32_t *const value_turbo)
{
    *value_min = 99;
    *value_max = 100;
    *value_turbo = 0;
}
