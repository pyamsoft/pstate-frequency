/**
 * @file psfreq_input.c
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

#include <getopt.h>
#include <stdlib.h>

#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"

static bool psfreq_input_string_isdigits(const char *s);

bool psfreq_input_parse(psfreq_option_type *const options,
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
                {0,               0,                  0,              0}
        };

        for (;;) {
                const int opt = getopt_long(argc, argv, shortOptions,
                                longOptions, NULL);
                if (opt < 0) {
                        break;
                } else {
                        if (!psfreq_option_parse(options, opt)) {
                                return false;
                        }
                }
        }
        return true;
}

char psfreq_input_plan_from_optarg(const char *const p)
{
        char plan;
        const char *const automatic = "auto";
        const char *const powersave = "powersave";
        const char *const balanced = "balanced";
        const char *const performance = "performance";

        if (psfreq_strings_starts_with(powersave, p)
                || psfreq_strings_equals(INPUT_PLAN_STR_POWERSAVE, p)) {
                plan = INPUT_PLAN_POWERSAVE;
        } else if (psfreq_strings_starts_with(balanced, p)
                || psfreq_strings_equals(INPUT_PLAN_STR_BALANCED, p)) {
                plan = INPUT_PLAN_BALANCED;
        } else if (psfreq_strings_starts_with(performance, p)
                || psfreq_strings_equals(INPUT_PLAN_STR_PERFORMANCE, p)) {
                plan = INPUT_PLAN_PERFORMANCE;
        } else if (psfreq_strings_starts_with(automatic, p)
                || psfreq_strings_equals(INPUT_PLAN_STR_AUTO, p)) {
                plan = INPUT_PLAN_AUTO;
        } else {
                psfreq_log_error("psfreq_input_plan_from_optarg",
                        "User input: '%s' is not valid for cpu_plan", p);
                plan = INPUT_PLAN_UNDEFINED;
        }
        return plan;
}

char psfreq_input_turbo_from_optarg(const char *const t)
{
        char turbo;
        const char *const turbo_off = "off";
        const char *const turbo_on = "on";
        if (psfreq_strings_starts_with(turbo_off, t)
                || psfreq_strings_equals(INPUT_TURBO_STR_OFF, t)) {
                turbo = INPUT_TURBO_OFF;
        } else if (psfreq_strings_starts_with(turbo_on, t)
                || psfreq_strings_equals(INPUT_TURBO_STR_ON, t)) {
                turbo = INPUT_TURBO_ON;
        } else {
                psfreq_log_error("psfreq_input_max_from_optarg",
                        "User input: '%s' is not valid for cpu_turbo", t);
                turbo = INPUT_TURBO_UNDEFINED;
        }
        return turbo;
}

int psfreq_input_max_from_optarg(const char *const m)
{
        if (!psfreq_input_string_isdigits(m)) {
                psfreq_log_error("psfreq_input_max_from_optarg",
                        "User input: '%s' is not valid for cpu_max", m);
                return -1;
        } else {
                return psfreq_strings_to_int(m);
        }
}

int psfreq_input_min_from_optarg(const char *const m)
{
        if (!psfreq_input_string_isdigits(m)) {
                psfreq_log_error("psfreq_input_min_from_optarg",
                        "User input: '%s' is not valid for cpu_min", m);
                return -1;
        } else {
                return psfreq_strings_to_int(m);
        }
}

char *psfreq_input_gov_from_optarg(const char *const g)
{
        char *gov;
        const char *const powersave = "powersave";
        const char *const performance = "performance";
        if (psfreq_strings_starts_with(powersave, g)
                || psfreq_strings_equals(INPUT_GOV_STR_POWERSAVE, g)) {
                gov = INPUT_GOV_POWERSAVE;
        } else if (psfreq_strings_starts_with(performance, g)
                || psfreq_strings_equals(INPUT_GOV_STR_PERFORMANCE, g)) {
                gov = INPUT_GOV_PERFORMANCE;
        } else {
                psfreq_log_error("psfreq_input_gov_from_optarg",
                        "User input: '%s' is not valid for scaling_governor", g);
                gov = INPUT_GOV_UNDEFINED;
        }
        return gov;
}

static bool psfreq_input_string_isdigits(const char *s)
{
        while (*s) {
                if (isdigit(*s++) == 0) {
                        return false;
                }
        }
        return true;
}

