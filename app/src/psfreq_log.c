/**
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
 */

#include <stdarg.h>
#include <stdio.h>

#include "psfreq_color.h"
#include "psfreq_log.h"

typedef unsigned char psfreq_log_level;

/**
 * Compare current log level to passed in log level
 *
 * @param log_level Log level required
 */
static unsigned char psfreq_log_level_more_verbose(unsigned char log_level);

static const psfreq_log_level PSFREQ_LOG_EXTRA_DEBUG = 1;
static const psfreq_log_level PSFREQ_LOG_DEBUG       = 1 << 1;
static const psfreq_log_level PSFREQ_LOG_NORMAL      = 1 << 2;
static const psfreq_log_level PSFREQ_LOG_QUIET       = 1 << 3;
static const psfreq_log_level PSFREQ_LOG_ALL_QUIET   = 1 << 4;

/*
 * Apperantly can't assign this to PSFREQ_LOG_NORMAL because it
 * is 'not constant'
 */
static psfreq_log_level psfreq_log_state = 1 << 2;

/**
 * Log output with a fancy wrapper
 *
 * @param fmt Format string
 * @param ... format args
 */
void psfreq_log_debug(const char *const name, const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_level_more_verbose(PSFREQ_LOG_DEBUG)) {
                fprintf(stdout, "%s[D] %s: %s",
                                psfreq_color_blue(), name,
                                psfreq_color_reset());
        }
        if (psfreq_log_level_more_verbose(PSFREQ_LOG_NORMAL)) {
                va_start(arg, fmt);
                vfprintf(stdout, fmt, arg);
                va_end(arg);
                fprintf(stdout, "\n");
        }
}

/**
 * Log debug level output with a fancy wrapper
 *
 * @param name Function name the debug is being called from
 * @param fmt Format string
 * @param ... format args
 */
void psfreq_log(const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_level_more_verbose(PSFREQ_LOG_QUIET)) {
                va_start(arg, fmt);
                vfprintf(stdout, fmt, arg);
                va_end(arg);
                fprintf(stdout, "\n");
        }
}

/**
 * Log error level output with a fancy wrapper
 *
 * @param name Function name the error is being called from
 * @param fmt Format string
 * @param ... format args
 */
void psfreq_log_error(const char *const name, const char *const fmt, ...)
{
        va_list arg;
        if (psfreq_log_level_more_verbose(PSFREQ_LOG_DEBUG)) {
                fprintf(stderr, "%s[E] %s: %s",
                                psfreq_color_red(), name,
                                psfreq_color_reset());
        }
        if (psfreq_log_level_more_verbose(PSFREQ_LOG_ALL_QUIET)) {
                va_start(arg, fmt);
                vfprintf(stderr, fmt, arg);
                va_end(arg);
                fprintf(stderr, "\n");
        }
}

/**
 * Set log level to debug
 */
void psfreq_log_set_debug(void)
{
        unsigned char v = PSFREQ_LOG_EXTRA_DEBUG;
        if (psfreq_log_state > v) {
                psfreq_log_state >>= 1;
        }
}

/**
 * Set log level to quiet
 */
void psfreq_log_set_quiet(void)
{
        unsigned char v = PSFREQ_LOG_ALL_QUIET;
        if (psfreq_log_state < v) {
                psfreq_log_state <<= 1;
        }
}

/**
 * Compare current log level to passed in log level
 *
 * @param log_level Log level required
 */
static unsigned char psfreq_log_level_more_verbose(unsigned char log_level)
{
        return (psfreq_log_state < log_level);
}

