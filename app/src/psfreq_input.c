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

char psfreq_input_parse(psfreq_option_type *const options,
                const int argc, char **const argv)
{
        const char *const shortOptions = ":SGHVcrdqp:m:n:t:g:";
        const struct option longOptions[] = {
                {"help",          no_argument,        NULL,           'H'},
                {"version",       no_argument,        NULL,           'V'},
                {"quiet",         no_argument,        NULL,           'q'},
                {"no-sleep",      no_argument,        NULL,           '2'},
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
                        psfreq_log_debug("psfreq_input_parse",
                                        "end of options reached.");
                        break;
                } else {
                        int r;
                        psfreq_log_debug("psfreq_input_parse",
                                        "found an option.");
                        r = psfreq_option_parse(options, opt);
                        if (r == OPTION_RETURNCODE_STOP_FAILURE) {
                                psfreq_log_debug("psfreq_input_parse",
                                        "Input was not handled properly");
                                return r;
                        } else if (r == OPTION_RETURNCODE_STOP_SUCCESS) {
                                psfreq_log_debug("psfreq_input_parse",
                                        "Input hit a stop option");
                                return r;
                        }
                }
        }
        return OPTION_RETURNCODE_CONTINUE;
}

