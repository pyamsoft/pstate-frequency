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

#ifndef C_PYAM_CPUWRITE_H_
#define C_PYAM_CPUWRITE_H_

#include "src/cpu.h"

void set_turbo(struct cpu_t *const cpu, const int32_t turbo);
void set_scaling_max(struct cpu_t *const cpu, const int32_t max);
void set_scaling_min(struct cpu_t *const cpu, const int32_t min);

#endif
