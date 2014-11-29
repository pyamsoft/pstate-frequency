/*
*   pstate_frequency Easier control of the Intel p-state driver
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
*   For questions please contact P.Yam Software at pyam.soft@gmail.com
*
*/

#ifndef C_PYAM_CPUUTIL_H_
#define C_PYAM_CPUUTIL_H_

#include <stdio.h>
#include <stdarg.h>

#include "src/cpu.h"

char* resolve_path_to_file(struct cpu_t *const cpu, const char *const file_name);
void safe_malloc(struct cpu_t *const cpu, const int32_t result,
        const char *const error_message, ...);
void check_file(struct cpu_t *const cpu, FILE *const file,
        const char *const file_name);
#endif

