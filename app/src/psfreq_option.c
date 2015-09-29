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

#include "psfreq_log.h"
#include "psfreq_option.h"

int psfreq_option_parse(const int opt)
{
        switch(opt) {
        case 0:
                /* end of options */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'H':
                /* Help */
                return OPTION_PARSE_EXITCODE_STOP_SUCCESS;
        case 'c':
                /*
                 * The --current option is only valid when using
                 * pstate-frequency to get CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'r':
                /*
                 * The --real option is only valid when using
                 * pstate-frequency to get CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'd':
                /* enable debugging */
                psfreq_log_set_debug();
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'q':
                psfreq_log_set_quiet();
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'a':
                psfreq_log_set_all_quiet();
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'V':
                /* Version */
                return OPTION_PARSE_EXITCODE_STOP_SUCCESS;
        case 'S':
                /* ACTION: Set */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'G':
                /* ACTION: Get */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'p':
                /*
                 * The --plan option is only valid when using pstate-frequency
                 * to set CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'm':
                /*
                 * The --max option is only valid when using pstate-frequency
                 * to set CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'g':
                /*
                 * The --governor option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 'n':
                /*
                 * The --min option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case 't':
                /*
                 * The --turbo option is only valid when using
                 * pstate-frequency to set CPU values
                 */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case '2':
                /* Do not sleep for 2 seconds */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case '1':
                /* Enable color messages */
                return OPTION_PARSE_EXITCODE_CONTINUE;
        case ':':
                /* Missing arguments */
                psfreq_log_error("psfreq_option_parse",
                                "Missing option arguments");
                return OPTION_PARSE_EXITCODE_STOP_FAILURE;
        case '?':
                /* Invalid option */
                psfreq_log_error("psfreq_option_parse", "Invalid option");
                return OPTION_PARSE_EXITCODE_STOP_FAILURE;
        default:
                psfreq_log_error("psfreq_option_parse",
                                "End of options, no matching option supported");
                return OPTION_PARSE_EXITCODE_STOP_FAILURE;
        }
}

