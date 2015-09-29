/**
 * @file psfreq_log.c
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

#include <stdio.h>

#include "psfreq_color.h"
#include "psfreq_log.h"

typedef unsigned char psfreq_log_level;

static psfreq_log_level psfreq_log_state = PSFREQ_LOG_NORMAL;

static unsigned char psfreq_log_should_display(unsigned char log_level);

static unsigned char psfreq_log_should_display(unsigned char log_level)
{
        return (psfreq_log_state < log_level);
}

void psfreq_log_debug(const char *const name, const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_should_display(PSFREQ_LOG_NORMAL)) {
                fprintf(stdout, "%s[D] %s %s",
                                PSFREQ_COLOR_BOLD_BLUE, name,
                                PSFREQ_COLOR_RESET);
                va_start(arg, fmt);
                vfprintf(stdout, fmt, arg);
                va_end(arg);
                fprintf(stdout, "\n");
        }
}

void psfreq_log_normal(const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_should_display(PSFREQ_LOG_QUIET)) {
                va_start(arg, fmt);
                vfprintf(stdout, fmt, arg);
                va_end(arg);
                fprintf(stdout, "\n");
        }
}

void psfreq_log_error(const char *const name, const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_should_display(PSFREQ_LOG_ALL_QUIET)) {
                fprintf(stderr, "%s[E] %s %s",
                                PSFREQ_COLOR_BOLD_RED, name,
                                PSFREQ_COLOR_RESET);
                va_start(arg, fmt);
                vfprintf(stderr, fmt, arg);
                va_end(arg);
                fprintf(stderr, "\n");
        }
}

void psfreq_log_set_debug(void)
{
        psfreq_log_state = PSFREQ_LOG_DEBUG;
}

void psfreq_log_set_normal(void)
{
        psfreq_log_state = PSFREQ_LOG_NORMAL;
}

void psfreq_log_set_quiet(void)
{
        psfreq_log_state = PSFREQ_LOG_QUIET;
}

void psfreq_log_set_all_quiet(void)
{
        psfreq_log_state = PSFREQ_LOG_ALL_QUIET;
}
