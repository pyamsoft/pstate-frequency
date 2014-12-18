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
 * For questions please contact P.Yam Software at pyam.soft@gmail.com
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "src/plan.h"
#include "src/util.h"
#include "src/color.h"

static void set_powersave(struct flag_t *const flags);
static void set_performance(struct flag_t *const flags);
static void set_max_performance(struct flag_t *const flags);

/*
 * Creates a new flag_t data type for holding the current
 * application settings
 */
struct flag_t flags_create(void)
{
	struct flag_t flags;
	flags.action = ACTION_UNDEFINED;
	flags.min = FLAG_UNINITIALIZED;
	flags.max = FLAG_UNINITIALIZED;
	flags.turbo = FLAG_UNINITIALIZED;
	flags.iosched = NULL;
	flags.governor = NULL;
	return flags;
}

/*
 * Free an allocated flag_t resources 
 */
void flags_destroy(struct flag_t* flags)
{
	log_debug("Freeing flags\n");
	if (flags->iosched != NULL) {
                log_debug("Freeing flags->iosched\n");
		free(flags->iosched);
		flags->iosched = NULL;
	}
	if (flags->governor != NULL) {
                log_debug("Freeing flags->governor\n");
		free(flags->governor);
		flags->governor = NULL;
	}
	flags = NULL;
}

/*
 * Sets the running power plan to one of three presets, powersave, performance,
 * and max-performance. Uses string comparison and pattern matching to decipher
 * the input given in the optarg variable
 */
int32_t set_plan(struct cpu_t *const cpu, struct flag_t *const flags)
{
        if (str_starts_with("powersave", optarg)
            || strncmp("1", optarg, strlen(optarg)) == 0) {
                set_powersave(flags);
        } else if (str_starts_with("performance", optarg)
            || strncmp("2", optarg, strlen(optarg)) == 0) {
                set_performance(flags);
        } else if (str_starts_with("max-performance", optarg)
            || strncmp("3", optarg, strlen(optarg)) == 0) {
                set_max_performance(flags);
        } else {
                fprintf(stderr, "%sPlan: Unrecognized plan: %s%s%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_BOLD_RED,
                        optarg, PYAM_COLOR_OFF);
                printf("%sValid plans are:%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                printf("%s    1: powersave%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                printf("%s    2: performance%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                printf("%s    3: max-performance%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                return -1;
        }
	flags->max = fix_value_range(flags->max, cpu->CPU_INFO_MIN + 1, 100);
	flags->min = fix_value_range(flags->min, cpu->CPU_INFO_MIN, 99);
        return 0;
}

/*
 * Sets the powersave plan
 */
static void set_powersave(struct flag_t *const flags)
{
	flags->min = 0;
	flags->max = 0;
	flags->turbo = 1;
}

/*
 * Sets the performance plan
 */
static void set_performance(struct flag_t *const flags)
{
	flags->min = 0;
	flags->max = 100;
	flags->turbo = 1;
}

/*
 * Sets the max-performance plan
 */
static void set_max_performance(struct flag_t *const flags)
{
	flags->min = 99;
	flags->max = 100;
	flags->turbo = 0;
}
