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

#include "include/psfreq_log.h"
#include "include/psfreq_util.h"
#include "include/psfreq_values.h"

namespace psfreq {


bool values::isInitialized() const
{
	return hasAction() && (max != -1 || min != -1
			|| turbo != -1 || governor != std::string());
}

bool values::hasAction() const
{
	return action != -1;
}

bool values::isActionNull() const
{
	return action == -1;
}

bool values::isActionGet() const
{
	return action == 0;
}

bool values::isActionSet() const
{
	return action == 1;
}

bool values::setGovernor(const std::string& newGovernor)
{
	if (newGovernor != std::string()) {
		governor = newGovernor;
		return true;
	}
	return false;
}


void values::setAction(const int newAction)
{
	action = newAction;
}

void values::setMax(const int newMax)
{
	max = newMax;
}

void values::setMin(const int newMin)
{
	min = newMin;
}

void values::setTurbo(const int newTurbo)
{
	turbo = newTurbo;
}

void values::setRequested(const int newRequest)
{
	requested = newRequest;
}

const std::string values::getGovernor() const
{
	return governor;
}

int values::getAction() const
{
	return action;
}

int values::getMax() const
{
	return max;
}

int values::getMin() const
{
	return min;
}

int values::getTurbo() const
{
	return turbo;
}

int values::getRequested() const
{
	return requested;
}

bool values::setPlan(const int powerPlan) {
	if (powerPlan != -1) {
		plan = powerPlan;
		return true;
	}
	return false;
}

bool values::runPlan()
{
	if (plan == -1) {
		return true;
	} else if (plan == 1) {
		setPlanPowersave();
		return true;
	} else if (plan == 2) {
		setPlanPerformance();
		return true;
	} else if (plan == 3) {
		setPlanMaxPerformance();
		return true;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (plan == 0) {
		const unsigned int result = setPlanAuto();
		if (result == 0) {
			std::cerr << "Failed to decide an automatic plan."
				<< std::endl;
			return false;
		}
		if (result == 1) {
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

void values::setPlanPowersave()
{
	max = 0;
	min = 0;
	turbo = parent.hasPstate() ? 1 : 0;
	governor = "powersave";
}

void values::setPlanPerformance()
{
	const int turboPercent = parent.getTurboPercent();
	max =  turboPercent != -1 ? turboPercent : 100;
	min = 0;
	turbo = parent.hasPstate() ? 1 : 0;
	governor = parent.hasPstate() ? "powersave" : "ondemand";
}

void values::setPlanMaxPerformance()
{
	max = 100;
	min = 100;
	turbo = parent.hasPstate() ? 0 : 1;
	governor = "performance";
}

unsigned int values::setPlanAuto()
{
	const char *const dirName = "/sys/class/power_supply/";
	DIR *const directory = opendir(dirName);
	if (!directory) {
		std::cerr << "Could not open directory: "
			<< dirName << std::endl;
		return 0;
	}
	struct dirent *entry =  readdir(directory);
	unsigned int result = 0;
	while(entry) {
		const std::string entryName = entry->d_name;
		if (!parent.hideDirectory(entryName)) {
			std::ostringstream oss;
			oss << dirName << entryName << "/";
			const std::string fullPath = oss.str();
			if (fullPath.length() < fullPath.max_size()) {
				result = parent.getPowerSupply(fullPath);
				if (result > 0) {
					break;
				}
			} else {
				std::cerr << "Path is larger than max allowed."
					<< std::endl;
				return 0;
			}
		}
		entry = readdir(directory);
	}
	if (closedir(directory)) {
		std::cerr << "Failed to close directory: "
			<< dirName << std::endl;
		return 0;
	}
	return result;
}

}
