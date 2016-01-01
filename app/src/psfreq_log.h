/**
 * @file psfreq_log.h
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
 * A convenience class which allows for easy logging of messages that are
 * sorted by levels: debugging, normal messages, quiet messages, and
 * entirely silenced output using the all_quiet mode.
 */

#ifndef PSFREQ_LOG_H
#define PSFREQ_LOG_H

#include <stdarg.h>

void psfreq_log(const char *const fmt, ...);
void psfreq_log_debug(const char *const name, const char *const fmt, ...);
void psfreq_log_error(const char *const name, const char *const fmt, ...);

void psfreq_log_set_debug(void);
void psfreq_log_set_quiet(void);

#endif
