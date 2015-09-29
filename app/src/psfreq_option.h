/**
 * @file psfreq_option.h
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

#ifndef PSFREQ_OPTION_H
#define PSFREQ_OPTION_H

#define OPTION_PARSE_EXITCODE_STOP_FAILURE -1
#define OPTION_PARSE_EXITCODE_CONTINUE 0
#define OPTION_PARSE_EXITCODE_STOP_SUCCESS 1

int psfreq_option_parse(const int opt);

#endif

