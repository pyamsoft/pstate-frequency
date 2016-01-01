/**
 * @file psfreq_constants.h
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
 * Constants for use in pstate-frequency
 */

#ifndef PSFREQ_CONSTANTS_H
#define PSFREQ_CONSTANTS_H

#define PLAN_UNDEFINED          -1
#define PLAN_AUTO               0
#define PLAN_POWERSAVE          1
#define PLAN_BALANCED           2
#define PLAN_PERFORMANCE        3
#define PLAN_NAME_AUTO          "auto"
#define PLAN_NAME_POWERSAVE     "powersave"
#define PLAN_NAME_BALANCED      "balanced"
#define PLAN_NAME_PERFORMANCE   "performance"
#define PLAN_STR_AUTO           "0"
#define PLAN_STR_POWERSAVE      "1"
#define PLAN_STR_BALANCED       "2"
#define PLAN_STR_PERFORMANCE    "3"

#define TURBO_UNDEFINED         -1
#define TURBO_ON                0
#define TURBO_OFF               1
#define TURBO_NAME_ON           "on"
#define TURBO_NAME_OFF          "off"
#define TURBO_STR_ON            "0"
#define TURBO_STR_OFF           "1"

#define GOV_UNDEFINED           NULL
#define GOV_POWERSAVE           "powersave"
#define GOV_PERFORMANCE         "performance"
#define GOV_STR_POWERSAVE       "0"
#define GOV_STR_PERFORMANCE     "1"

#endif
