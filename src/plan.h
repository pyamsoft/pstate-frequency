/*
 * pstate_frequency Easier control of the Intel p-state driver
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
 * For questions please contact pyamsoft at pyam.soft@gmail.com
 */

#ifndef C_PYAM_PLAN_H_
#define C_PYAM_PLAN_H_

#include <stdint.h>

#include "src/cpu.h"

#define ACTION_SET 2
#define ACTION_GET 1
#define ACTION_UNDEFINED 0
#define FLAG_UNINITIALIZED -1

struct flag_t {
	int32_t action;
	int32_t min;
	int32_t max;
	int32_t turbo;
	char* iosched;
	char* governor;
};

struct flag_t flags_create(void);
void flags_destroy(struct flag_t* flags);
int32_t set_plan(struct cpu_t *const cpu, struct flag_t *const flags);
                

#endif
