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
/*
 * For strtok_r using C99
 */
#define _POSIX_C_SOURCE 2
#include <stdio.h>

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "psfreq_constants.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_plan.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

static const char *const CURRENT_DIR = ".";
static const char *const PARENT_DIR = "..";
static const unsigned int POWER_PLAN_ITEMS = 4;
static const unsigned char HIDE_DIRECTORY = 1;
static const unsigned char NO_HIDE_DIRECTORY = 0;

/**
 * Internal, tokenizes the passed in plan values and sets the cpu values
 *
 * @param p The plan requested
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_set(const char *const p,
                int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Parse the requested automatic plan and run it
 *
 * @param plan The plan requested
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_auto_exec(const char *const plan,
                int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Run the powersave plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_powersave(int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Run the balanced plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_balanced(int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Run the performance plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_performance(int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Run the auto plan by figuring out the current power supply state
 * of the machine and setting the plan based on user variables
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_auto(int *const max, int *const min,
                int *const turbo, char **const gov);

/**
 * Get the status of the current power source with the type Mains
 *
 * @param dir The directory to look for power supply classes in
 * @param name The name of the directory passed as dir
 * @return An integer value denoting the type of power supply, either
 * undefined, mains, or battery
 */
static unsigned int psfreq_plan_get_power_source(DIR *const dir,
                const char *const name);

/**
 * Check if the full path is the Mains type power supply
 *
 * @param p The full path to the power supply
 * @return Integer value, undefined if the power supply is not accessable
 * or does not specify a curent online status, mains if the power source
 * is Mains, and battery if the power source is battery
 */
static unsigned int psfreq_plan_mains_online(char *const p);


/**
 * Check if the directory name represented by e is the current or
 * parent directory
 *
 * @param e Directory name
 * @return Boolean, true if directory is either current or parent and should
 * be hidden from results, false if otherwise
 */
static unsigned char psfreq_plan_hide_directory(const char *const e);

/**
 * Wrapper for the strtok_r function to be used to parse user power plans
 *
 * @param s String entered by user
 * @param arr_len Hold the length of the returned array
 * @return An array holding each tokenized user input value
 */
static char **psfreq_plan_strtok(char *s, size_t *arr_len);

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
                int *const min, int *const turbo, char **const gov)
{
        unsigned char r;
        if (*plan == PLAN_AUTO) {
                r = psfreq_plan_auto(max, min, turbo, gov);
        } else if (*plan == PLAN_POWERSAVE) {
                r = psfreq_plan_powersave(max, min, turbo, gov);
        } else if (*plan == PLAN_BALANCED) {
                r = psfreq_plan_balanced(max, min, turbo, gov);
        } else if (*plan == PLAN_PERFORMANCE) {
                r = psfreq_plan_performance(max, min, turbo, gov);
        } else {
                r = POWER_PLAN_APPLY_FAILURE;
        }
        return r;
}

/**
 * Internal, tokenizes the passed in plan values and sets the cpu values
 *
 * @param p The plan requested
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_set(const char *const p,
                int *const max, int *const min,
                int *const turbo, char **const gov)
{
        char **arr = NULL;;
        char *pp = NULL;
        size_t arr_len;
        if (psfreq_strings_asprintf(&pp, "%s", p) < 0) {
                return POWER_PLAN_APPLY_FAILURE;
        }
        arr = psfreq_plan_strtok(pp, &arr_len);
        if (arr == STRTOK_ERROR) {
                free(pp);
                return POWER_PLAN_APPLY_FAILURE;
        }

        /* Check array length */
        if (arr_len != POWER_PLAN_ITEMS) {
                psfreq_log_error("psfreq_plan_set",
                                "Incorrect number of plan items. Got %d wanted %d",
                                arr_len, POWER_PLAN_ITEMS);
                free(pp);
                free(arr);
                return POWER_PLAN_APPLY_FAILURE;
        }

        *max = psfreq_strings_to_int(arr[0]);
        *min = psfreq_strings_to_int(arr[1]);
        *turbo = psfreq_strings_to_int(arr[2]);
        if (psfreq_strings_asprintf(gov, "%s", arr[3]) < 0) {
                free(pp);
                free(arr);
                return POWER_PLAN_APPLY_FAILURE;
        }
        free(pp);
        free(arr);
        return POWER_PLAN_APPLY_SUCCESS;
}

/**
 * Parse the requested automatic plan and run it
 *
 * @param plan The plan requested
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_auto_exec(const char *const plan,
                int *const max, int *const min,
                int *const turbo, char **const gov)
{
        if (psfreq_strings_starts_with(plan, PLAN_NAME_POWERSAVE)
            || psfreq_strings_equals(plan, PLAN_STR_POWERSAVE)) {
                return psfreq_plan_powersave(max, min, turbo, gov);
        } else if (psfreq_strings_starts_with(plan, PLAN_NAME_BALANCED)
                   || psfreq_strings_equals(plan, PLAN_STR_BALANCED)) {
                return psfreq_plan_balanced(max, min, turbo, gov);
        } else if (psfreq_strings_starts_with(plan, PLAN_NAME_PERFORMANCE)
                   || psfreq_strings_equals(plan, PLAN_STR_PERFORMANCE)) {
                return psfreq_plan_performance(max, min, turbo, gov);
        } else {
                psfreq_log_error("psfreq_plan_auto_ac",
                                 "Invalid plan specified");
                return POWER_PLAN_APPLY_FAILURE;
        }
}

/**
 * Run the powersave plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_powersave(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_POWERSAVE
        return psfreq_plan_set(PRESET_POWER_PLAN_POWERSAVE, max,
                        min, turbo, gov);
#else
        *max = CPU_FREQUENCY_MINIMUM;
        *min = CPU_FREQUENCY_MINIMUM;
        *turbo = TURBO_OFF;
        *gov = GOV_POWERSAVE;
        return POWER_PLAN_APPLY_SUCCESS;
#endif
}

/**
 * Run the balanced plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_balanced(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_BALANCED
        return psfreq_plan_set(PRESET_POWER_PLAN_BALANCED, max,
                        min, turbo, gov);
#else
        *max = CPU_FREQUENCY_MAXIMUM;
        *min = CPU_FREQUENCY_MINIMUM;
        *turbo = TURBO_OFF;
        *gov = GOV_POWERSAVE;
        return POWER_PLAN_APPLY_SUCCESS;
#endif
}

/**
 * Run the performance plan, either user preset or fallback to a default
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_performance(int *const max, int *const min,
                int *const turbo, char **const gov)
{
#ifdef PRESET_POWER_PLAN_PERFORMANCE
        return psfreq_plan_set(PRESET_POWER_PLAN_PERFORMANCE, max,
                        min, turbo, gov);
#else
        *max = CPU_FREQUENCY_MAXIMUM;
        *min = CPU_FREQUENCY_MAXIMUM;
        *turbo = TURBO_ON;
        *gov = GOV_PERFORMANCE;
        return POWER_PLAN_APPLY_SUCCESS;
#endif
}

/**
 * Run the auto plan by figuring out the current power supply state
 * of the machine and setting the plan based on user variables
 *
 * @param max Value to store the cpu max into
 * @param min Value to store the cpu min into
 * @param turbo Value to store the cpu turbo into
 * @param gov Value to store the cpu governor into
 * @return Boolean value, true if plan was set successfully, false otherwise
 */
static unsigned char psfreq_plan_auto(int *const max, int *const min,
                int *const turbo, char **const gov)
{
        unsigned int r;
        const char *const name = "/sys/class/power_supply/";
        DIR *const dir = opendir(name);
	psfreq_log_debug("psfreq_plan_auto", "Open dir: %s", name);
        if (dir == NULL) {
		psfreq_log_error("psfreq_plan_auto",
			"Couldn't open dir: %s", name);
                return POWER_PLAN_APPLY_FAILURE;
	}
	r = psfreq_plan_get_power_source(dir, name);
	if (closedir(dir)) {
		psfreq_log_error("psfreq_plan_auto",
			"Couldn't close dir: %s", name);
		return POWER_PLAN_APPLY_FAILURE;
	}
	/* Set plan here */
	if (r == POWER_SOURCE_MAINS) {
#ifdef AUTO_POWER_PLAN_AC
		return psfreq_plan_auto_exec(AUTO_POWER_PLAN_AC,
                                max, min, turbo, gov);
#else
		return psfreq_plan_balanced(max, min, turbo, gov);
#endif
	} else if (r == POWER_SOURCE_BATTERY) {
#ifdef AUTO_POWER_PLAN_BAT
		return psfreq_plan_auto_exec(AUTO_POWER_PLAN_BAT,
                                max, min, turbo, gov);
#else
		return psfreq_plan_powersave(max, min, turbo, gov);
#endif
	} else {
                return POWER_PLAN_APPLY_FAILURE;
        }
}

/**
 * Get the status of the current power source with the type Mains
 *
 * @param dir The directory to look for power supply classes in
 * @param name The name of the directory passed as dir
 * @return An integer value denoting the type of power supply, either
 * undefined, mains, or battery
 */
static unsigned int psfreq_plan_get_power_source(DIR *const dir,
                const char *const name)
{
        struct dirent *entry =  readdir(dir);
        unsigned int result = POWER_SOURCE_UNDEFINED;
        while(entry) {
                const char *const e_name = entry->d_name;
                char *path;
                char *full_path;
                if (psfreq_plan_hide_directory(e_name)) {
                        entry = readdir(dir);
			continue;
		}
		path = psfreq_strings_concat(name, e_name);
		if (path == STRING_CONCAT_ERROR) {
			psfreq_log_error("psfreq_plan_get_power_source",
				"Path failed");
			return POWER_SOURCE_UNDEFINED;
		}
		full_path = psfreq_strings_concat(path, "/");
		if (full_path == STRING_CONCAT_ERROR) {
			psfreq_log_error("psfreq_plan_get_power_source",
				"Path failed");
			free(path);
			return POWER_SOURCE_UNDEFINED;
		}
		psfreq_log_debug("psfreq_plan_get_power_source",
			"Power supply path: %s", full_path);
                result = psfreq_plan_mains_online(full_path);
		free(path);
		free(full_path);
		if (result > POWER_SOURCE_UNDEFINED) {
			break;
		}
		psfreq_log_debug("psfreq_plan_get_power_source",
			"Read next dir in directory");
                entry = readdir(dir);
        }
        return result;
}

/**
 * Check if the full path is the Mains type power supply
 *
 * @param p The full path to the power supply
 * @return Integer value, undefined if the power supply is not accessable
 * or does not specify a curent online status, mains if the power source
 * is Mains, and battery if the power source is battery
 */
static unsigned int psfreq_plan_mains_online(char *const p)
{
        unsigned int r;
	char *type;
        char *stat;
        char status;
        char *power;
        if (p == NULL) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "p is NULL");
                return POWER_SOURCE_UNDEFINED;
        }

        type = psfreq_strings_concat(p, "/type");
        if (type == STRING_CONCAT_ERROR) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "type is undefined");
                return POWER_SOURCE_UNDEFINED;
        }
        if (access(type, F_OK) < 0) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "Couldn't open file %s", type);
                free(type);
                return POWER_SOURCE_UNDEFINED;
        }
        power = psfreq_util_read(type);
        free(type);
        if (power == READ_ERROR) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "power is undefined");
                return POWER_SOURCE_UNDEFINED;
        }
        if (!psfreq_strings_equals("Mains", power)) {
                psfreq_log_debug("psfreq_plan_mains_online",
                                "power source not Mains");
                free(power);
                return POWER_SOURCE_UNDEFINED;
        }
        stat = psfreq_util_read2(p, "/online");
        if (stat == READ_ERROR) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "stat is undefined");
                free(power);
                return POWER_SOURCE_UNDEFINED;
        }
        status = psfreq_strings_to_int(stat);
        if (status < 0) {
                psfreq_log_error("psfreq_plan_mains_online",
                                "stat is non zero");
                r = POWER_SOURCE_UNDEFINED;
        } else if (status) {
                psfreq_log_debug("psfreq_plan_mains_online",
                                "stat is AC");
                r = POWER_SOURCE_MAINS;
        } else {
                psfreq_log_debug("psfreq_plan_mains_online",
                                "stat is BAT");
                r = POWER_SOURCE_BATTERY;
        }
        free(stat);
        free(power);
        return r;
}

/**
 * Check if the directory name represented by e is the current or
 * parent directory
 *
 * @param e Directory name
 * @return Boolean, true if directory is either current or parent and should
 * be hidden from results, false if otherwise
 */
static unsigned char psfreq_plan_hide_directory(const char *const e)
{
        unsigned char res = psfreq_strings_equals(CURRENT_DIR, e);
        if (res == STRING_COMPARE_SUCCESS) {
                return HIDE_DIRECTORY;
        }
        res = psfreq_strings_equals(PARENT_DIR, e);
        if (res == STRING_COMPARE_SUCCESS) {
                return HIDE_DIRECTORY;
        }

        return NO_HIDE_DIRECTORY;
}

/**
 * Wrapper for the strtok_r function to be used to parse user power plans
 *
 * @param s String entered by user
 * @param arr_len Hold the length of the returned array
 * @return An array holding each tokenized user input value
 */
static char **psfreq_plan_strtok(char *s, size_t *arr_len)
{
        size_t i = 0;
        const char *const del = " ,.-";
        char *tok;
        char *saveptr;
        char **arr = malloc(POWER_PLAN_ITEMS * sizeof(char *));
        *arr_len = 0;

        if (arr == NULL) {
                psfreq_log_debug("psfreq_plan_strtok",
                                "Failed malloc");
                return STRTOK_ERROR;
        }
        psfreq_log_debug("psfreq_plan_strtok",
                        "Split string '%s' by delims '%s'", s, del);
        tok = strtok_r(s, del, &saveptr);
        psfreq_log_debug("psfreq_plan_strtok", "First tok success");
        while (tok != STRTOK_ERROR) {
                psfreq_log_debug("psfreq_strings_strok",
                                "assign '%s' to arr[%d]", tok, i);
                arr[i++] = tok;
                tok = strtok_r(NULL, del, &saveptr);
        }
        *arr_len = i;
        return arr;
}

