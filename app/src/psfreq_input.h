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

#ifndef PSFREQ_INPUT_H
#define PSFREQ_INPUT_H

#include "psfreq_option.h"

#define INPUT_PARSE_SUCCESS 1
#define INPUT_PARSE_FAILURE 0

unsigned char psfreq_input_parse(psfreq_option_type *const options,
                const int argc, char **const argv);
char psfreq_input_plan_from_optarg(const char *const p);
char psfreq_input_turbo_from_optarg(const char *const t);
char *psfreq_input_gov_from_optarg(const char *const g);

#endif

