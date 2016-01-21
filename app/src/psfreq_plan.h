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

#ifndef PSFREQ_PLAN_H
#define PSFREQ_PLAN_H

#define STRTOK_ERROR            0
#define POWER_SOURCE_UNDEFINED  0
#define POWER_SOURCE_MAINS      1
#define POWER_SOURCE_BATTERY    2

#define POWER_PLAN_APPLY_SUCCESS 1
#define POWER_PLAN_APPLY_FAILURE 0

/**
 * Sets the cpu values based on the passed in plan
 *
 * @param plan The plan requested
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
unsigned char psfreq_plan_set_cpu(const char *const plan, int *const max,
                int *const min, int *const turbo, char **const gov);

#endif

