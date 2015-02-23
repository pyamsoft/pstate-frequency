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

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_log.h"
#include "include/psfreq_util.h"

namespace psfreq {

int cpu::findTurboPercent() const
{
	const std::string line = cpuSysfs.read("intel_pstate/turbo_pct");
	if (line != std::string()) {
		return stringToNumber(line);
	}
	std::cerr << "Unable to find turbo percentage." << std::endl;
	return -1;
}

bool cpu::findPstate() const
{
	const std::string driver = cpuSysfs.read("cpu0/cpufreq/scaling_driver");
	if (driver != std::string()) {
		return (driver.compare("intel_pstate") == 0);
	}
	std::cerr << "Unable to get read driver to check for intel_pstate"
		<< std::endl;
	return false;
}

unsigned int cpu::findNumber() const
{
	const char *cmd = "grep processor /proc/cpuinfo | wc -l";
	const std::vector<std::string> result = cpuSysfs.readPipe(cmd, 1);
	if (!result.empty()) {
		return stringToNumber(result[0]);
	}
	std::cerr << "Unable to find number of CPUs" << std::endl;
	return 0;
}

void cpu::initializeVector(std::vector<std::string> &vector,
		std::string what) const
{
	for (unsigned int i = 0; i < number; ++i) {
		std::ostringstream oss;
		oss << "cpu" << i << "/cpufreq/scaling_" << what;
		vector.push_back(oss.str());
	}
}

double cpu::findInfoMaxFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_max_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	std::cerr << "Unable to find cpuinfo_max_freq" << std::endl;
	return 1.0;
}

double cpu::findInfoMinFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_min_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	std::cerr << "Unable to find cpuinfo_min_freq" << std::endl;
	return 1.0;
}

}
