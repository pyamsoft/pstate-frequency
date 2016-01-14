/**
 * @file psfreq_input.c
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
 *
 *  Copyright (C) 2014 - 2016 Peter Kenji Yamanaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * @section DESCRIPTION
 * Helper functions for taking and processing user input from the command line
 */

#include <getopt.h>
#include <stdlib.h>

#include "psfreq_constants.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"

/**
 * Parse the passed in input and pass the processed input to options
 *
 * @param options The psfreq_option_type instance to use
 * @param argc Number of arguments, literally just pass it argc from main
 * @param argv arguments array, literally just pass it argv from main
 * @return Boolean value, true if all options parsed successfully, false if
 * otherwise
 */
unsigned char psfreq_input_parse(psfreq_option_type *const options,
                const int argc, char **const argv)
{
        const char *const shortOptions = ":SGHVcrdqp:m:n:t:g:";
        const struct option longOptions[] = {
                {"help",          no_argument,        NULL,           'H'},
                {"version",       no_argument,        NULL,           'V'},
                {"quiet",         no_argument,        NULL,           'q'},
                {"sleep",         no_argument,        NULL,           '2'},
                {"delay",         no_argument,        NULL,           '3'},
                {"debug",         no_argument,        NULL,           'd'},
                {"get",           no_argument,        NULL,           'G'},
                {"set",           no_argument,        NULL,           'S'},
                {"current",       no_argument,        NULL,           'c'},
                {"real",          no_argument,        NULL,           'r'},
                {"color",         no_argument,        NULL,           '1'},
                {"plan",          required_argument,  NULL,           'p'},
                {"governor",      required_argument,  NULL,           'g'},
                {"max",           required_argument,  NULL,           'm'},
                {"min",           required_argument,  NULL,           'n'},
                {"turbo",         required_argument,  NULL,           't'},
                {NULL,            no_argument,        NULL,           0}
        };

        for (;;) {
                const int opt = getopt_long(argc, argv, shortOptions,
                                longOptions, NULL);
                if (opt < 0) {
                        break;
                } else {
                        if (psfreq_option_parse(options, opt)
                                        == OPTION_PARSE_FAILURE) {
                                return INPUT_PARSE_FAILURE;
                        }
                }
        }
        return INPUT_PARSE_SUCCESS;
}

/**
 * Parse the power plan from the option argument
 *
 * @param p The option argument string to parse
 * @return The power plan value, or undefined if passed in input is invalid
 */
char psfreq_input_plan_from_optarg(const char *const p)
{
        char plan;
        if (psfreq_strings_starts_with(PLAN_NAME_POWERSAVE, p)
                || psfreq_strings_equals(PLAN_STR_POWERSAVE, p)) {
                plan = PLAN_POWERSAVE;
        } else if (psfreq_strings_starts_with(PLAN_NAME_BALANCED, p)
                || psfreq_strings_equals(PLAN_STR_BALANCED, p)) {
                plan = PLAN_BALANCED;
        } else if (psfreq_strings_starts_with(PLAN_NAME_PERFORMANCE, p)
                || psfreq_strings_equals(PLAN_STR_PERFORMANCE, p)) {
                plan = PLAN_PERFORMANCE;
        } else if (psfreq_strings_starts_with(PLAN_NAME_AUTO, p)
                || psfreq_strings_equals(PLAN_STR_AUTO, p)) {
                plan = PLAN_AUTO;
        } else {
                psfreq_log_error("psfreq_input_plan_from_optarg",
                        "User input: '%s' is not valid for cpu_plan", p);
                plan = PLAN_UNDEFINED;
        }
        return plan;
}

/**
 * Parse the turbo from the option argument
 *
 * @param t The option argument string to parse
 * @return The turbo value, or undefined if passed in input is invalid
 */
char psfreq_input_turbo_from_optarg(const char *const t)
{
        char turbo;
        if (psfreq_strings_starts_with(TURBO_NAME_OFF, t)
                || psfreq_strings_equals(TURBO_STR_OFF, t)) {
                turbo = TURBO_OFF;
        } else if (psfreq_strings_starts_with(TURBO_NAME_ON, t)
                || psfreq_strings_equals(TURBO_STR_ON, t)) {
                turbo = TURBO_ON;
        } else {
                psfreq_log_error("psfreq_input_max_from_optarg",
                        "User input: '%s' is not valid for cpu_turbo", t);
                turbo = TURBO_UNDEFINED;
        }
        return turbo;
}

/**
 * Parse the governor from the option argument
 *
 * @param g The option argument string to parse
 * @return The governor value, or undefined if passed in input is invalid
 */
char *psfreq_input_gov_from_optarg(const char *const g)
{
        char *gov;
        if (psfreq_strings_starts_with(GOV_POWERSAVE, g)
                || psfreq_strings_equals(GOV_STR_POWERSAVE, g)) {
                gov = GOV_POWERSAVE;
        } else if (psfreq_strings_starts_with(GOV_PERFORMANCE, g)
                || psfreq_strings_equals(GOV_STR_PERFORMANCE, g)) {
                gov = GOV_PERFORMANCE;
        } else {
                psfreq_log_error("psfreq_input_gov_from_optarg",
                        "User input: '%s' is not valid for scaling_governor", g);
                gov = GOV_UNDEFINED;
        }
        return gov;
}

