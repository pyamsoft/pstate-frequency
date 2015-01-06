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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "src/plan.h"
#include "src/util.h"
#include "src/color.h"
#include "src/cpuutil.h"

#if INCLUDE_UDEV_RULE >=1

#include <dirent.h>
#include "src/cpuread.h"

static void set_auto(struct cpu_t *const cpu, struct flag_t *const flags, const char *const dir_name);
static int32_t hide_dir(char *const dir_name);
static int32_t is_main_power_type(char *const dir_name);
static int32_t get_path_length(const char *const dir_name, char *const d_name);

#endif

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

#if INCLUDE_UDEV_RULE >=1
/*
 * Determines the current power source and then selects a power plan based
 on that source
 *
 */
static void set_auto(struct cpu_t *const cpu, struct flag_t *const flags, const char *const dir_name)
{
	log_debug("Attempting to open dir: %s\n", dir_name);
	DIR *const directory = opendir(dir_name);
	if (!directory) {
		log_error("Could not open directory: %s\n", dir_name);
		cpu_destroy(cpu);
		exit(1);
	}
	struct dirent *entry =  readdir(directory);
	log_debug("Attempting to open dir entry: %s\n", entry);
	while(entry) {
		if (!hide_dir(entry->d_name)) {
			if (get_path_length(dir_name, entry->d_name) < PATH_MAX) {
				char *dir_entry;
				safe_malloc(cpu, asprintf(&dir_entry, "%s/%s/type", dir_name, entry->d_name),
						"Could not malloc for subdir\n", NULL);
				if (access(dir_entry, F_OK) != -1) {
					char *power_type = get_power_type(cpu, dir_entry);
					if (is_main_power_type(power_type)) {
						char *status;
						safe_malloc(cpu, asprintf(&status, "%s/%s/online",
							dir_name, entry->d_name),
							"Could not malloc for online\n", power_type, dir_entry, NULL);
						int32_t online = get_online_status(cpu, status);
						if (online) {
							log_debug("auto set performance\n");
							set_performance(flags);
						} else {
							log_debug("auto set powersave\n");
							set_powersave(flags);
						}
						free(status);
						if (online) {
							free(power_type);
							free(dir_entry);
							break;
						}
					}
					free(power_type);
				}
				free(dir_entry);
			} else {
				log_error("Path name is too long\n");
				exit(1);
			}
		}
		entry = readdir(directory);
	}
	if (closedir(directory)) {
		log_error("Could not close directory: %s\n", dir_name);
		exit(1);
	}
}

/*
 * Check if the power_supply is type Mains
 */
static int32_t is_main_power_type(char *const dir_name)
{
	return strncmp(dir_name, "Mains", 5) == 0;
}

/*
 * Ignore the directory if it is the parent or itself
 */
static int32_t hide_dir(char *const dir_name)
{
	const int32_t result = (strncmp(dir_name, "..", 2) == 0 || strncmp(dir_name, ".", 1) == 0);
	log_debug("%s %s\n", result ? "Hiding directory:" : "Showing directory", dir_name);
	return result;
}

/*
 * Get the length of the path name
 */
static int32_t get_path_length(const char *const dir_name, char *const d_name)
{
	char path[PATH_MAX];
	const int32_t result = snprintf(path, PATH_MAX, "%s/%s", dir_name, d_name);
	log_debug("Path length: %d\n", result);
	return result;
}
#endif

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
#if INCLUDE_UDEV_RULE >=1
	} else if (str_starts_with("auto", optarg)
		|| strncmp("0", optarg, strlen(optarg)) == 0) {
		set_auto(cpu, flags, DIR_POWER_SUPPLY);
#endif
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
                fprintf(stderr, "%sValid plans are:%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
#if INCLUDE_UDEV_RULE >= 1
                fprintf(stderr, "%s    0: auto%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
#endif
                fprintf(stderr, "%s    1: powersave%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                fprintf(stderr, "%s    2: performance%s\n",
                        PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_OFF);
                fprintf(stderr, "%s    3: max-performance%s\n",
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
