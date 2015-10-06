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

#include <stdlib.h>

#define ACTION_TYPE_UNDEFINED   -1
#define ACTION_TYPE_HELP        0
#define ACTION_TYPE_VERSION     1
#define ACTION_TYPE_CPU_GET     2
#define ACTION_TYPE_CPU_SET     3

#define PLAN_UNDEFINED  NULL

#define CPU_UNDEFINED   NULL

#define TURBO_UNDEFINED NULL

#define GOVERNOR_UNDEFINED      NULL

#define CPU_GET_TYPE_CURRENT    0
#define CPU_GET_TYPE_REAL       1

#define SLEEP           1
#define NO_SLEEP        0

#define NO_COLOR        0
#define COLOR           1

typedef struct psfreq_option_type {
        char action;
        char *cpu_turbo;
        char *cpu_max;
        char *cpu_min;
        char *cpu_governor;
        char *cpu_plan;
        unsigned char cpu_get_type;
        unsigned char cpu_sleep;
        unsigned char color_enabled;
} psfreq_option_type;

void psfreq_option_init(psfreq_option_type *options);

unsigned char psfreq_option_parse(psfreq_option_type *options, const int opt);

#endif

