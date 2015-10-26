/**
 * @file psfreq_plan.c
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
#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_plan.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

static const unsigned int POWER_PLAN_ITEMS = 4;
static unsigned int psfreq_plan_get_power_source(DIR *const dir,
                const char *const name);
static bool psfreq_plan_hide_directory(const char *const e);
static unsigned int psfreq_plan_check_power_is_mains(char *const p);
static char **psfreq_plan_strtok(char *s, const size_t num);

static bool psfreq_plan_set(const char *const p,
                int *const max, int *const min,
                int *const turbo, char **const gov);

static bool psfreq_plan_auto_exec(const char *const plan,
                int *const max, int *const min,
                int *const turbo, char **const gov);

static bool psfreq_plan_set(const char *const p,
                int *const max, int *const min,
                int *const turbo, char **const gov)
{
        char **arr;
        char *pp;
        if (psfreq_strings_asprintf(&pp, "%s", p) < 0) {
                psfreq_log_error("psfreq_plan_set",
                        "asprintf returned a -1, indicating a failure during\n"
                        "either memory allocation or some other error.");
                return false;
        }
        arr = psfreq_plan_strtok(pp, POWER_PLAN_ITEMS);

        *max = psfreq_strings_to_int(arr[0]);
        *min = psfreq_strings_to_int(arr[1]);
        *turbo = psfreq_strings_to_int(arr[2]);
        if (psfreq_strings_asprintf(gov, "%s", arr[3]) < 0) {
                psfreq_log_error("psfreq_plan_set",
                        "asprintf returned a -1, indicating a failure during\n"
                        "either memory allocation or some other error.");
                free(pp);
                free(arr);
                return false;
        }
        free(pp);
        free(arr);
        return true;
}

static bool psfreq_plan_auto_exec(const char *const plan,
                int *const max, int *const min,
                int *const turbo, char **const gov)
{
        if (psfreq_strings_starts_with(plan, "powersave")
            || psfreq_strings_equals(plan, "1")) {
                psfreq_plan_powersave(max, min, turbo, gov);
        } else if (psfreq_strings_starts_with(plan, "performance")
                   || psfreq_strings_equals(plan, "2")) {
                psfreq_plan_performance(max, min, turbo, gov);
        } else if (psfreq_strings_starts_with(plan, "max-performance")
                   || psfreq_strings_equals(plan, "3")) {
                psfreq_plan_max_performance(max, min, turbo, gov);
        } else {
                psfreq_log_error("psfreq_plan_auto_ac",
                                 "Invalid plan specified");
                return false;
        }
        return true;
}

bool psfreq_plan_set_cpu(const char *const plan, int *const max,
                int *const min, int *const turbo, char **const gov)
{
        bool r;
        if (*plan == INPUT_PLAN_AUTO) {
                r = psfreq_plan_auto(max, min, turbo, gov);
        } else if (*plan == INPUT_PLAN_POWERSAVE) {
                r = psfreq_plan_powersave(max, min, turbo, gov);
        } else if (*plan == INPUT_PLAN_PERFORMANCE) {
                r = psfreq_plan_performance(max, min, turbo, gov);
        } else if (*plan == INPUT_PLAN_MAX_PERFORMANCE) {
                r = psfreq_plan_max_performance(max, min, turbo, gov);
        } else {
                r = false;
        }
        return r;
}

bool psfreq_plan_powersave(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_POWERSAVE
        return psfreq_plan_set(PRESET_POWER_PLAN_POWERSAVE, max,
                        min, turbo, gov);
#else
        *max = 0;
        *min = 0;
        *turbo = 1;
        *gov = "powersave";
        return true;
#endif
}

bool psfreq_plan_performance(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_PERFORMANCE
        return psfreq_plan_set(PRESET_POWER_PLAN_PERFORMANCE, max,
                        min, turbo, gov);
#else
        *max = 100;
        *min = 0;
        *turbo = 1;
        *gov = "powersave";
        return true;
#endif
}

bool psfreq_plan_max_performance(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_MAX_PERFORMANCE
        return psfreq_plan_set(PRESET_POWER_PLAN_MAX_PERFORMANCE, max,
                        min, turbo, gov);
#else
        *max = 100;
        *min = 99;
        *turbo = 0;
        *gov = "performance";
        return true;
#endif
}

bool psfreq_plan_auto(int *const max, int *const min,
                int *const turbo, char **const gov)
{
        unsigned int r;
        const char *const name = "/sys/class/power_supply/";
        DIR *const dir = opendir(name);
	psfreq_log_debug("psfreq_plan_auto", "Open dir: %s", name);
        if (!dir) {
		psfreq_log_error("psfreq_plan_auto",
			"Couldn't open dir: %s", name);
                return false;
	}
	r = psfreq_plan_get_power_source(dir, name);
	if (closedir(dir)) {
		psfreq_log_error("psfreq_plan_auto",
			"Couldn't close dir: %s", name);
		return false;
	}
	/* Set plan here */
	if (r == 1) {
#ifdef AUTO_POWER_PLAN_AC
		return psfreq_plan_auto_exec(AUTO_POWER_PLAN_AC,
                                max, min, turbo, gov);
#else
		return psfreq_plan_performance(max, min, turbo, gov);
#endif
	} else if (r == 2) {
#ifdef AUTO_POWER_PLAN_BAT
		return psfreq_plan_auto_exec(AUTO_POWER_PLAN_BAT,
                                max, min, turbo, gov);
#else
		return psfreq_plan_powersave(max, min, turbo, gov);
#endif
	} else {
                return false;
        }
}

static unsigned int psfreq_plan_get_power_source(DIR *const dir,
                const char *const name)
{
        struct dirent *entry =  readdir(dir);
        unsigned int result = 0;
        while(entry) {
                const char *const e_name = entry->d_name;
                char *path;
                char *full_path;
                if (psfreq_plan_hide_directory(e_name)) {
                        entry = readdir(dir);
			continue;
		}
		path = psfreq_strings_concat(name, e_name);
		if (path == NULL) {
			psfreq_log_error("psfreq_plan_get_power_source",
				"Path failed");
			return 0;
		}
		full_path = psfreq_strings_concat(path, "/");
		if (full_path == NULL) {
			psfreq_log_error("psfreq_plan_get_power_source",
				"Path failed");
			free(path);
			return 0;
		}
		psfreq_log_debug("psfreq_plan_get_power_source",
			"Power supply path: %s", full_path);
                result = psfreq_plan_check_power_is_mains(full_path);
		free(path);
		free(full_path);
		if (result > 0) {
			break;
		}
		psfreq_log_debug("psfreq_plan_get_power_source",
			"Read next dir in directory");
                entry = readdir(dir);
        }
        return result;
}

static unsigned int psfreq_plan_check_power_is_mains(char *const p)
{
        unsigned int r;
	char *type;
        char *stat;
        char status;
        char *power;
        if (p == NULL) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "p is NULL");
                return 0;
        }

        type = psfreq_strings_concat(p, "/type");
        if (type == NULL) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "type is NULL");
                return 0;
        }
        if (access(type, F_OK) < 0) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "Couldn't open file %s", type);
                free(type);
                return 0;
        }
        power = psfreq_util_read(type);
        free(type);
        if (power == NULL) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "power is NULL");
                return 0;
        }
        if (!psfreq_strings_equals("Mains", power)) {
                psfreq_log_debug("psfreq_plan_check_power_is_mains",
                                "power source not Mains");
                free(power);
                return 0;
        }
        stat = psfreq_util_read2(p, "/online");
        if (stat == NULL) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "stat is NULL");
                free(power);
                return 0;
        }
        status = psfreq_strings_to_int(stat);
        if (status < 0) {
                psfreq_log_error("psfreq_plan_check_power_is_mains",
                                "stat is 0");
                r = 0;
        } else if (status) {
                psfreq_log_debug("psfreq_plan_check_power_is_mains",
                                "stat is AC");
                r = 1;
        } else {
                psfreq_log_debug("psfreq_plan_check_power_is_mains",
                                "stat is BAT");
                r = 2;
        }
        free(stat);
        free(power);
        return r;
}

static bool psfreq_plan_hide_directory(const char *const e)
{
        return (psfreq_strings_equals(".", e) || psfreq_strings_equals("..", e));
}

static char **psfreq_plan_strtok(char *s, const size_t num)
{
        char** arr = malloc(num * sizeof(char *));
        size_t i = 0;
        const char *const del = " ,.-";
        char *tok;
        char *saveptr;
        psfreq_log_debug("psfreq_plan_strtok",
                        "Split string '%s' by delims '%s'", s, del);
        tok = strtok_r(s, del, &saveptr);
        psfreq_log_debug("psfreq_plan_strtok", "First tok success");
        while (tok != NULL) {
                psfreq_log_debug("psfreq_strings_strok",
                                "assign '%s' to arr[%d]", tok, i);
                arr[i++] = tok;
                tok = strtok_r(NULL, del, &saveptr);
        }
        return arr;
}
