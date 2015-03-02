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

bool Cpu::findPstate() const
{
	const std::string driver = sysfs.read("cpu0/cpufreq/scaling_driver");
	if (driver != std::string()) {
		return (driver.compare("intel_pstate") == 0);
	}
	if (!Log::isAllQuiet()) {
		std::cerr << Color::boldRed()
			<< "Unable to get read driver to check for"
			<< " intel_pstate" << Color::reset()
			<< std::endl;
	}
	return false;
}

unsigned int Cpu::findNumber() const
{
	const char *cmd = "grep processor /proc/cpuinfo | wc -l";
	const std::vector<std::string> result = sysfs.readPipe(cmd, 1);
	if (!result.empty()) {
		return stringToNumber(result[0]);
	}
	if (!Log::isAllQuiet()) {
		std::cerr << Color::boldRed()
			<< "Unable to find number of CPUs"
			<< Color::reset() << std::endl;
	}
	return 0;
}

void Cpu::initializeVector(std::vector<std::string> &vector,
		std::string what) const
{
	for (unsigned int i = 0; i < number; ++i) {
		std::ostringstream oss;
		oss << "cpu" << i << "/cpufreq/scaling_" << what;
		vector.push_back(oss.str());
	}
}

double Cpu::findInfoMaxFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/cpuinfo_max_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	std::cerr << Color::boldRed()
		<< "Unable to find cpuinfo_max_freq"
		<< Color::reset() << std::endl;
	return 1.0;
}

double Cpu::findInfoMinFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/cpuinfo_min_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	if (!Log::isAllQuiet()) {
		std::cerr << Color::boldRed()
			<< "Unable to find cpuinfo_min_freq"
			<< Color::reset()
			<< std::endl;
	}
	return 1.0;
}

}
