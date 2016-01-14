/**
 * @file psfreq_output.h
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
 * Helper functions for presenting formatted output
 */

#ifndef PSFREQ_OUTPUT_H
#define PSFREQ_OUTPUT_H

#include "psfreq_cpu.h"
#include "psfreq_option.h"

#define OUTPUT_FAILURE 0
#define OUTPUT_SUCCESS 1

/**
 * Output the current pstate-frequency frequency CPU configuration
 * based on user specified option flags
 *
 * @param cpu The psfreq_cpu_type instance to use
 * @param options The psfreq_option_type instance to use
 * @return Boolean value, true if output was successful, false if otherwise
 */
unsigned char psfreq_output_get_cpu(const psfreq_cpu_type *const cpu,
                           const psfreq_option_type *const options);

/**
 * Output the usage and help
 */
void psfreq_output_usage(void);

/**
 * Output the program version
 */
void psfreq_output_version(void);

#endif

