/**
 * @file psfreq_input.h
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 * Helper functions for taking and processing user input from the command line
 */

#include <stdbool.h>

#ifndef PSFREQ_INPUT_H
#define PSFREQ_INPUT_H

#define INPUT_PLAN_UNDEFINED            -1
#define INPUT_PLAN_AUTO                 0
#define INPUT_PLAN_POWERSAVE            1
#define INPUT_PLAN_BALANCED             2
#define INPUT_PLAN_PERFORMANCE          3
#define INPUT_PLAN_NAME_AUTO            "auto"
#define INPUT_PLAN_NAME_POWERSAVE       "powersave"
#define INPUT_PLAN_NAME_BALANCED        "balanced"
#define INPUT_PLAN_NAME_PERFORMANCE     "performance"
#define INPUT_PLAN_STR_AUTO             "0"
#define INPUT_PLAN_STR_POWERSAVE        "1"
#define INPUT_PLAN_STR_BALANCED         "2"
#define INPUT_PLAN_STR_PERFORMANCE      "3"

#define INPUT_TURBO_UNDEFINED   -1
#define INPUT_TURBO_ON          0
#define INPUT_TURBO_OFF         1
#define INPUT_TURBO_NAME_ON     "on"
#define INPUT_TURBO_NAME_OFF    "off"
#define INPUT_TURBO_STR_ON      "0"
#define INPUT_TURBO_STR_OFF     "1"

#define INPUT_GOV_UNDEFINED             NULL
#define INPUT_GOV_POWERSAVE             "powersave"
#define INPUT_GOV_PERFORMANCE           "performance"
#define INPUT_GOV_STR_POWERSAVE         "0"
#define INPUT_GOV_STR_PERFORMANCE       "1"

#include "psfreq_option.h"


bool psfreq_input_parse(psfreq_option_type *const options,
                const int argc, char **const argv);

char psfreq_input_plan_from_optarg(const char *const p);

char psfreq_input_turbo_from_optarg(const char *const t);

int psfreq_input_max_from_optarg(const char *const m);

int psfreq_input_min_from_optarg(const char *const m);

char *psfreq_input_gov_from_optarg(const char *const g);

#endif

