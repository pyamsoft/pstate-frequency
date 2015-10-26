/**
 * @file psfreq_plan.h
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
 * Helper functions for managing power plans on the system
 */

#ifndef PSFREQ_PLAN_H
#define PSFREQ_PLAN_H

#include <stdbool.h>
#include <dirent.h>

bool psfreq_plan_set_cpu(const char *const plan, int *const max,
                int *const min, int *const turbo, char **const gov);

bool psfreq_plan_powersave(int *const max, int *const min,
                int *const turbo, char **const gov);

bool psfreq_plan_performance(int *const max, int *const min,
                int *const turbo, char **const gov);

bool psfreq_plan_max_performance(int *const max, int *const min,
                int *const turbo, char **const gov);

bool psfreq_plan_auto(int *const max, int *const min,
                int *const turbo, char **const gov);

#endif

