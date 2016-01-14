/**
 * @file psfreq_input.h
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

#ifndef PSFREQ_INPUT_H
#define PSFREQ_INPUT_H

#include "psfreq_option.h"

#define INPUT_PARSE_SUCCESS 1
#define INPUT_PARSE_FAILURE 0

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
                const int argc, char **const argv);

/**
 * Parse the power plan from the option argument
 *
 * @param p The option argument string to parse
 * @return The power plan value, or undefined if passed in input is invalid
 */
char psfreq_input_plan_from_optarg(const char *const p);

/**
 * Parse the turbo from the option argument
 *
 * @param t The option argument string to parse
 * @return The turbo value, or undefined if passed in input is invalid
 */
char psfreq_input_turbo_from_optarg(const char *const t);

/**
 * Parse the governor from the option argument
 *
 * @param g The option argument string to parse
 * @return The governor value, or undefined if passed in input is invalid
 */
char *psfreq_input_gov_from_optarg(const char *const g);

#endif

