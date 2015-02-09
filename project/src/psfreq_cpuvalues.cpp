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
#include <cstdlib>

#include <dirent.h>
#include <unistd.h>

#include "include/psfreq_cpuvalues.h"
#include "include/psfreq_logger.h"
#include "include/psfreq_sysfs.h"
#include "include/psfreq_util.h"

namespace psfreq {

cpuValues::cpuValues()
{
	action = -1;
	max = -1;
	min = -1;
	turbo = -1;
	governor = "";
}

cpuValues::~cpuValues()
{
}

bool cpuValues::isInitialized() const
{
	return hasAction() && (max != -1 || min != -1 || turbo != -1 || governor != "");
}

bool cpuValues::hasAction() const
{
	return action != -1;
}

bool cpuValues::isActionNull() const
{
	return action == -1;
}

bool cpuValues::isActionGet() const
{
	return action == 0;
}

bool cpuValues::isActionSet() const
{
	return action == 1;
}

void cpuValues::setGovernor(const std::string& newGovernor)
{
	governor = newGovernor;
}


void cpuValues::setAction(const int newAction)
{
	action = newAction;
}

void cpuValues::setMax(const int newMax)
{
	max = newMax;
}

void cpuValues::setMin(const int newMin)
{
	min = newMin;
}

void cpuValues::setTurbo(const int newTurbo)
{
	turbo = newTurbo;
}

const std::string cpuValues::getGovernor() const
{
	return governor;
}

int cpuValues::getAction() const
{
	return action;
}

int cpuValues::getMax() const
{
	return max;
}

int cpuValues::getMin() const
{
	return min;
}

int cpuValues::getTurbo() const
{
	return turbo;
}

void cpuValues::setPlan(const int plan)
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

void cpuValues::setPlanPowersave()
{
	max = 0;
	min = 0;
	turbo = 1;
	governor = "powersave";
}

void cpuValues::setPlanPerformance()
{
	max = 100;
	min = 0;
	turbo = 1;
	governor = hasPstate() ? "powersave" : "ondemand";
}

void cpuValues::setPlanMaxPerformance()
{
	max = 100;
	min = 100;
	turbo = 1;
	governor = "performance";
}

void cpuValues::setPlanAuto()
{
	const char *const dirName = "/sys/class/power_supply/";
	DIR *const directory = opendir(dirName);
	if (!directory) {
		psfreq::logger::close();
		exit(EXIT_FAILURE);
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
				psfreq::logger::close();
				exit(EXIT_FAILURE);
			}
		}
		entry = readdir(directory);
	}
	if (closedir(directory)) {
		psfreq::logger::close();
		exit(EXIT_FAILURE);
	}
}

bool cpuValues::discoverPowerSupply(const std::string &fullPath)
{
	std::ostringstream oss;
	oss << fullPath << "/type";
	const std::string typePath = oss.str();
	const char *const type = typePath.c_str();
	if (access(type, F_OK) != -1) {
		sysfs sysfs;
		const std::string powerType = sysfs.read(fullPath, "type");
		if (powerType.compare("Mains") == 0) {
			const int status = stringToNumber(sysfs.read(fullPath, "online"));
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

bool cpuValues::hideDirectory(const std::string &entryName)
{
	return (entryName.compare("..") == 0 || entryName.compare(".") == 0);
}

}
