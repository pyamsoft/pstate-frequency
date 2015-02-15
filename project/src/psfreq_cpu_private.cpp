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
#include <cstdio>
#include <iostream>
#include <sstream>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_logger.h"
#include "include/psfreq_util.h"

namespace psfreq {

bool cpu::pstate = cpu::findPstate();

unsigned int cpu::findNumber() const
{
	std::ostringstream log;
	if (logger::isDebug()) {
		log << "pstate-frequency [psfreq_cpu_private.cpp]: cpu::findNumber"
			<< std::endl;
		logger::d(log);
	}

	const char *cmd = "grep processor /proc/cpuinfo | wc -l";
	return stringToNumber(cpuSysfs.readPipe(cmd, 1)[0]);
}

void cpu::initializeVector(std::vector<std::string> &vector, std::string what) const
{
	std::ostringstream log;
	if (logger::isDebug()) {
		log << "pstate-frequency [psfreq_cpu_private.cpp]: cpu::initializeVector"
			<< std::endl;
		logger::d(log);
	}
	for (unsigned int i = 0; i < number; ++i) {
		std::ostringstream oss;
		oss << "cpu" << i << "/cpufreq/scaling_" << what;
	if (logger::isDebug()) {
			log << "\tVector entry[" << i << "]: " << oss.str() << std::endl;
			logger::d(log);
	}
		vector.push_back(oss.str());
	}
}

double cpu::findInfoMaxFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_max_freq");
	const double result = stringToNumber(line);
	return result;
}

double cpu::findInfoMinFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_min_freq");
	const double result = stringToNumber(line);
	return result;
}

}
