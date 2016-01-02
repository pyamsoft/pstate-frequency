/**
 * @file psfreq_option.c
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
 * Helper functions for processing and handling getopt output
 */

#include <getopt.h>

#include "psfreq_color.h"
#include "psfreq_log.h"
#include "psfreq_option.h"

static const unsigned char DELAY = 1;
static const unsigned char NO_DELAY = 0;
static const unsigned char SLEEP = 1;
static const unsigned char NO_SLEEP = 0;

/**
 * Initialize a new psfreq_option_type instance
 *
 * @param options the option instance to initialize
 */
void psfreq_option_init(psfreq_option_type *options)
{
        options->action = ACTION_TYPE_UNDEFINED;
        options->cpu_turbo = OPT_UNDEFINED;
        options->cpu_max = OPT_UNDEFINED;
        options->cpu_min = OPT_UNDEFINED;
        options->cpu_governor = OPT_UNDEFINED;
        options->cpu_plan = OPT_UNDEFINED;
        options->cpu_get_type = CPU_GET_TYPE_CURRENT;
        options->cpu_sleep = NO_SLEEP;
        options->delay = NO_DELAY;
}

/**
 * Parse options returned from the GNU getopt_long function
 *
 * @param options the option instance to use
 * @param opt The option code from getopt_long
 */
unsigned char psfreq_option_parse(psfreq_option_type *options, const int opt)
{
        switch(opt) {
        case 0:
                /* end of options */
                return 1;
        case 'H':
                /* Help */
                if (options->action == ACTION_TYPE_UNDEFINED) {
			options->action = ACTION_TYPE_HELP;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"Cannot define multiple actions");
			return 0;
		}
        case 'V':
                /* Version */
                if (options->action == ACTION_TYPE_UNDEFINED) {
			options->action = ACTION_TYPE_VERSION;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"Cannot define multiple actions");
			return 0;
		}
        case 'G':
                /* ACTION: Get */
                if (options->action == ACTION_TYPE_UNDEFINED) {
			options->action = ACTION_TYPE_CPU_GET;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"Cannot define multiple actions");
			return 0;
		}
        case 'c':
                /*
                 * The --current option is only valid when using
                 * pstate-frequency to get CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_GET) {
                        options->cpu_get_type = CPU_GET_TYPE_CURRENT;
                        return 1;
                } else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -c|--current is only available when acting with -G|--get");
                        return 0;
                }
        case 'r':
                /*
                 * The --real option is only valid when using
                 * pstate-frequency to get CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_GET) {
                        options->cpu_get_type = CPU_GET_TYPE_REAL;
                        return 1;
                } else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -r|--real is only available when acting with -G|--get");
                        return 0;
                }
        case 'S':
                /* ACTION: Set */
                if (options->action == ACTION_TYPE_UNDEFINED) {
			options->action = ACTION_TYPE_CPU_SET;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"Cannot define multiple actions");
			return 0;
		}
        case 'p':
                /*
                 * The --plan option is only valid when using pstate-frequency
                 * to set CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_SET) {
			options->cpu_plan = optarg;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -p|--plan is only available when acting with -S|--set");
                        return 0;
		}
        case 'm':
                /*
                 * The --max option is only valid when using pstate-frequency
                 * to set CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_SET) {
			options->cpu_max = optarg;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -m|--max is only available when acting with -S|--set");
                        return 0;
		}
        case 'g':
                /*
                 * The --governor option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_SET) {
			options->cpu_governor = optarg;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -g|--governor is only available when acting with -S|--set");
                        return 0;
		}
        case 'n':
                /*
                 * The --min option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_SET) {
			options->cpu_min = optarg;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -n|--min is only available when acting with -S|--set");
                        return 0;
		}
        case 't':
                /*
                 * The --turbo option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                if (options->action == ACTION_TYPE_CPU_SET) {
			options->cpu_turbo = optarg;
			return 1;
		} else {
			psfreq_log_error("psfreq_option_parse",
				"The switch -t|--turbo is only available when acting with -S|--set");
                        return 0;
		}
        case 'd':
                /* enable debugging */
                psfreq_log_set_debug();
                return 1;
        case 'q':
                psfreq_log_set_quiet();
                return 1;
        case '2':
                /* Sleep for 2 seconds */
                options->cpu_sleep = SLEEP;
                return 1;
        case '3':
                /* Delay starting for 5 seconds */
                options->delay = DELAY;
                return 1;
        case '1':
                /* Enable color messages */
                psfreq_color_enable();
                return 1;
        case ':':
                /* Missing arguments */
                psfreq_log_error("psfreq_option_parse",
                                "Missing option arguments");
                return 0;
        case '?':
                /* Invalid option */
                psfreq_log_error("psfreq_option_parse", "Invalid option");
                return 0;
        default:
                psfreq_log_error("psfreq_option_parse",
                                "End of options, no matching option supported");
                return 0;
        }
}

