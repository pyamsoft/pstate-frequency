/*
*   pstate_frequency Easier control of the Intel p-state driver
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
*   For questions please contact P.Yam Software at pyam.soft@gmail.com
*
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "src/plan.h"
#include "src/util.h"
#include "src/color.h"

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
