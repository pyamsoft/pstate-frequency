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


bool Values::isInitialized() const
{
	return hasAction() && (max != -1 || min != -1
			|| turbo != -1 || governor != std::string());
}

bool Values::hasAction() const
{
	return action != -1;
}

bool Values::isActionNull() const
{
	return action == -1;
}

bool Values::isActionGet() const
{
	return action == 0;
}

bool Values::isActionSet() const
{
	return action == 1;
}

bool Values::setGovernor(const std::string& newGovernor)
{
	if (newGovernor != std::string()) {
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

bool Values::setPlan(const int powerPlan) {
	if (powerPlan != -1) {
		plan = powerPlan;
		return true;
	}
	return false;
}

bool Values::runPlan()
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
			if (!Log::isAllQuiet()) {
				std::cerr << Color::boldRed()
					<< "Failed to decide an automatic"
					<< " plan."
					<< Color::reset()
					<< std::endl;
			}
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

void Values::setPlanPowersave()
{
	max = 0;
	min = 0;
	turbo = cpu.hasPstate() ? 1 : 0;
	governor = "powersave";
}

void Values::setPlanPerformance()
{
	max =  100;
	min = 0;
	turbo = cpu.hasPstate() ? 1 : 0;
	governor = cpu.hasPstate() ? "powersave" : "ondemand";
}

void Values::setPlanMaxPerformance()
{
	max = 100;
	min = 100;
	turbo = cpu.hasPstate() ? 0 : 1;
	governor = "performance";
}

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
		return 0;
	}
	struct dirent *entry =  readdir(directory);
	unsigned int result = 0;
	while(entry) {
		const std::string entryName = entry->d_name;
		if (!cpu.hideDirectory(entryName)) {
			std::ostringstream oss;
			oss << dirName << entryName << "/";
			const std::string fullPath = oss.str();
			if (fullPath.length() < fullPath.max_size()) {
				result = cpu.getPowerSupply(fullPath);
				if (result > 0) {
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
				return 0;
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
		return 0;
	}
	return result;
}

}
