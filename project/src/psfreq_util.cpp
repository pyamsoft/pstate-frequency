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

#include "include/psfreq_logger.h"
#include "include/psfreq_sysfs.h"
#include "include/psfreq_util.h"

namespace psfreq {

double stringToNumber(const std::string &line)
{
	std::ostringstream log;
	log << "pstate-frequency [psfreq_util.cpp]: stringToNumber" << std::endl;
	psfreq::logger::d(log);

	log << "\tLine is: " << line << std::endl;
	psfreq::logger::d(log);
        std::istringstream iss(line);

	int result;
	iss >> result;

	log << "\tResult is: " << result << std::endl;
	psfreq::logger::d(log);
	return result;
}

int boundValue(const int value, const int minBound,
		const int maxBound)
{
	std::ostringstream log;
	log << "pstate-frequency [psfreq_util.cpp]: boundValue" << std::endl;
	psfreq::logger::d(log);

	if (value < minBound) {
		log << "\tBound value to minBound: " << minBound << std::endl;
		psfreq::logger::d(log);
		return minBound;
	} else if (value > maxBound) {
		log << "\tBound value to maxBound: " << maxBound << std::endl;
		psfreq::logger::d(log);
		return maxBound;
	} else {
		log << "\tNo bounding needed." << std::endl;
		psfreq::logger::d(log);
		return value;
	}
}

bool stringStartsWith(const std::string &control, const std::string &value)
{
	std::ostringstream log;
	log << "pstate-frequency [psfreq_util.cpp]: stringStartsWith"
		<< std::endl;
	psfreq::logger::d(log);

	log << "\tCheck if " << control
		<< " starts with string: " << value << std::endl;
	psfreq::logger::d(log);
	return control.compare(0, value.length(), value) == 0;
}

bool hasPstate()
{
	std::ostringstream log;
	log << "pstate-frequency [psfreq_util.cpp]: hasPstate"
		<< std::endl;
	psfreq::logger::d(log);

	log << "\tCheck for presence of pstate driver"
		<< std::endl;
	psfreq::logger::d(log);

	sysfs cpuSysfs;
	const std::string driver = cpuSysfs.read("cpu0/cpufreq/scaling_driver");
	log << "Compare found: " << driver << " with driver: intel_pstate"
		<< std::endl;
	psfreq::logger::d(log);

	return (driver.compare("intel_pstate") == 0);
}

}
