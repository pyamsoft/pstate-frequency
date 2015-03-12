/*
 * pstate-frequency Easier control of the Intel p-state driver
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

#include <iostream>
#include <sstream>

#include <dirent.h>

#include "include/psfreq_color.h"
#include "include/psfreq_log.h"
#include "include/psfreq_util.h"
#include "include/psfreq_values.h"

namespace psfreq {

const std::string Values::BAD_READ = std::string();
const int Values::ACTION_NULL = -1;
const int Values::ACTION_SET = 1;
const int Values::ACTION_GET = 0;
const int Values::REQUESTED_CURRENT = 0;
const int Values::REQUESTED_REAL = 1;
const int Values::POWER_PLAN_NONE = -1;
const int Values::POWER_PLAN_POWERSAVE = 1;
const int Values::POWER_PLAN_PERFORMANCE = 2;
const int Values::POWER_PLAN_MAX_PERFORMANCE = 3;
const int Values::POWER_PLAN_AUTO = 0;
const int Values::AUTO_NONE = 0;
const int Values::CPUFREQ_NO_TURBO = 0;
const int Values::CPUFREQ_TURBO = 1;
const int Values::PSTATE_NO_TURBO = 1;
const int Values::PSTATE_TURBO = 0;
const int Values::MIN_POSSIBLE_FREQ = 0;
const int Values::MAX_POSSIBLE_FREQ = 100;

bool Values::isInitialized() const
{
	return hasAction() && (max != ACTION_NULL
			|| min != ACTION_NULL
			|| turbo != ACTION_NULL
			|| governor != BAD_READ);
}

bool Values::hasAction() const
{
	return action != ACTION_NULL;
}

bool Values::isActionNull() const
{
	return action == ACTION_NULL;
}

bool Values::isActionGet() const
{
	return action == ACTION_GET;
}

bool Values::isActionSet() const
{
	return action == ACTION_SET;
}

/*
 * If the governor to set is a valid governor,
 * continue with normal operation, otherwise
 * exit the program from main.
 */
bool Values::setGovernor(const std::string& newGovernor)
{
	if (newGovernor != BAD_READ) {
		governor = newGovernor;
		return true;
	}
	return false;
}


void Values::setAction(const int newAction)
{
	action = newAction;
}

void Values::setMax(const int newMax)
{
	max = newMax;
}

void Values::setMin(const int newMin)
{
	min = newMin;
}

void Values::setTurbo(const int newTurbo)
{
	turbo = newTurbo;
}

void Values::setRequested(const int newRequest)
{
	requested = newRequest;
}

const std::string Values::getGovernor() const
{
	return governor;
}

int Values::getAction() const
{
	return action;
}

int Values::getMax() const
{
	return max;
}

int Values::getMin() const
{
	return min;
}

int Values::getTurbo() const
{
	return turbo;
}

int Values::getRequested() const
{
	return requested;
}

/*
 * If the plan to set is a valid plan,
 * continue with normal operation, otherwise
 * exit the program from main.
 */
bool Values::setPlan(const int powerPlan) {
	if (powerPlan != POWER_PLAN_NONE) {
		plan = powerPlan;
		return true;
	}
	return false;
}

/*
 * If the plan has not been requesed, return true and continue,
 * otherwise make sure that the plan requested does infact exist,
 * if not return false and exit the program from main.
 */
bool Values::runPlan()
{
	if (plan == POWER_PLAN_NONE) {
		return true;
	} else if (plan == POWER_PLAN_POWERSAVE) {
		setPlanPowersave();
		return true;
	} else if (plan == POWER_PLAN_PERFORMANCE) {
		setPlanPerformance();
		return true;
	} else if (plan == POWER_PLAN_MAX_PERFORMANCE) {
		setPlanMaxPerformance();
		return true;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (plan == POWER_PLAN_AUTO) {
		const unsigned int result = setPlanAuto();
		if (result == AUTO_NONE) {
			if (!Log::isAllQuiet()) {
				std::cerr << Color::boldRed()
					<< "Failed to decide an automatic"
					<< " plan."
					<< Color::reset()
					<< std::endl;
			}
			return false;
		}
		if (result == POWER_PLAN_POWERSAVE) {
			setPlanPowersave();
		} else {
			setPlanPerformance();
		}
		return true;
#endif
#endif
	}
	return false;
}

void Values::setPlanPowersave()
{
	max = MIN_POSSIBLE_FREQ;
	min = MIN_POSSIBLE_FREQ;
	turbo = cpu.hasPstate() ? PSTATE_NO_TURBO : CPUFREQ_NO_TURBO;
	governor = "powersave";
}

void Values::setPlanPerformance()
{
	max = MAX_POSSIBLE_FREQ;
	min = MIN_POSSIBLE_FREQ;
	turbo = cpu.hasPstate() ? PSTATE_NO_TURBO : CPUFREQ_NO_TURBO;
	governor = cpu.hasPstate() ? "powersave" : "ondemand";
}

void Values::setPlanMaxPerformance()
{
	max = MAX_POSSIBLE_FREQ;
	min = MAX_POSSIBLE_FREQ;
	turbo = cpu.hasPstate() ? PSTATE_TURBO : CPUFREQ_TURBO;
	governor = "performance";
}

/*
 * When requested, this plan will search the power_supply sysfs to
 * find which interface is considered the 'Mains' type adapter.
 * Once found, it will set a power plan based on the online status
 * on the 'Mains' adapter.
 */
unsigned int Values::setPlanAuto()
{
	const char *const dirName = "/sys/class/power_supply/";
	DIR *const directory = opendir(dirName);
	if (!directory) {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "Could not open directory: "
				<< dirName
				<< Color::reset()
				<< std::endl;
		}
		return AUTO_NONE;
	}
	struct dirent *entry =  readdir(directory);
	unsigned int result = AUTO_NONE;
	while(entry) {
		const std::string entryName = entry->d_name;
		if (!cpu.hideDirectory(entryName)) {
			std::ostringstream oss;
			oss << dirName << entryName << "/";
			const std::string fullPath = oss.str();
			if (fullPath.length() < fullPath.max_size()) {
				result = cpu.getPowerSupply(fullPath);
				if (result > AUTO_NONE) {
					break;
				}
			} else {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "Path is larger than"
						<< " max allowed."
						<< Color::reset()
						<< std::endl;
				}
				return AUTO_NONE;
			}
		}
		entry = readdir(directory);
	}
	if (closedir(directory)) {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "Failed to close directory: "
				<< dirName
				<< Color::reset()
				<< std::endl;
		}
		return AUTO_NONE;
	}
	return result;
}

}
