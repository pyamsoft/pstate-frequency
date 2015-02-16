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

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <dirent.h>
#include <unistd.h>

#include "include/psfreq_cpu.h"
#include "include/psfreq_util.h"

namespace psfreq {


bool cpu::values::isInitialized() const
{
	return hasAction() && (max != -1 || min != -1
			|| turbo != -1 || governor != "");
}

bool cpu::values::hasAction() const
{
	return action != -1;
}

bool cpu::values::isActionNull() const
{
	return action == -1;
}

bool cpu::values::isActionGet() const
{
	return action == 0;
}

bool cpu::values::isActionSet() const
{
	return action == 1;
}

void cpu::values::setGovernor(const std::string& newGovernor)
{
	governor = newGovernor;
}


void cpu::values::setAction(const int newAction)
{
	action = newAction;
}

void cpu::values::setMax(const int newMax)
{
	max = newMax;
}

void cpu::values::setMin(const int newMin)
{
	min = newMin;
}

void cpu::values::setTurbo(const int newTurbo)
{
	turbo = newTurbo;
}

void cpu::values::setRequested(const int newRequest)
{
	requested = newRequest;
}

const std::string cpu::values::getGovernor() const
{
	return governor;
}

int cpu::values::getAction() const
{
	return action;
}

int cpu::values::getMax() const
{
	return max;
}

int cpu::values::getMin() const
{
	return min;
}

int cpu::values::getTurbo() const
{
	return turbo;
}

int cpu::values::getRequested() const
{
	return requested;
}

void cpu::values::setPlan(const int plan)
{
	if (plan == 1) {
		setPlanPowersave();
	} else if (plan == 2) {
		setPlanPerformance();
	} else if (plan == 3) {
		setPlanMaxPerformance();
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (plan == 0) {
		setPlanAuto();
#endif
#endif
	}
}

void cpu::values::setPlanPowersave()
{
	max = 0;
	min = 0;
	turbo = parent.hasPstate() ? 1 : 0;
	governor = "powersave";
}

void cpu::values::setPlanPerformance()
{
	max = 100;
	min = 0;
	turbo = parent.hasPstate() ? 1 : 0;
	governor = parent.hasPstate() ? "powersave" : "ondemand";
}

void cpu::values::setPlanMaxPerformance()
{
	max = 100;
	min = 100;
	turbo = parent.hasPstate() ? 0 : 1;
	governor = "performance";
}

void cpu::values::setPlanAuto()
{
	const char *const dirName = "/sys/class/power_supply/";
	DIR *const directory = opendir(dirName);
	if (!directory) {
		std::exit(EXIT_FAILURE);
	}
	struct dirent *entry =  readdir(directory);
	while(entry) {
		const std::string entryName = entry->d_name;
		if (!hideDirectory(entryName)) {
			std::ostringstream oss;
			oss << dirName << entryName << "/";
			const std::string fullPath = oss.str();
			if (fullPath.length() < fullPath.max_size()) {
				if (discoverPowerSupply(fullPath)) {
					break;
				}
			} else {
				std::exit(EXIT_FAILURE);
			}
		}
		entry = readdir(directory);
	}
	if (closedir(directory)) {
		std::exit(EXIT_FAILURE);
	}
}

bool cpu::values::discoverPowerSupply(const std::string &fullPath)
{
	std::ostringstream oss;
	oss << fullPath << "/type";
	const std::string typePath = oss.str();
	const char *const type = typePath.c_str();
	if (access(type, F_OK) != -1) {
		const std::string powerType = parent.cpuSysfs.read(fullPath, "type");
		if (powerType.compare("Mains") == 0) {
			const int status = stringToNumber(parent.cpuSysfs.read(fullPath, "online"));
			if (status == 1) {
				setPlanPerformance();
			} else {
				setPlanPowersave();
			}
		return true;
		}
	}
	return false;
}

bool cpu::values::hideDirectory(const std::string &entryName)
{
	return (entryName.compare("..") == 0 || entryName.compare(".") == 0);
}

}
