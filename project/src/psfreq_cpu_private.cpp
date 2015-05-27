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

const unsigned int Cpu::NO_CPUS = 0;
const unsigned int Cpu::NO_FREQ = 0;

unsigned int Cpu::getNumber() const
{
	return number;
}

double Cpu::getInfoMinFrequency() const
{
	return minInfoFrequency;
}

double Cpu::getInfoMaxFrequency() const
{
	return maxInfoFrequency;
}

/*
 * Return a boolean based on whether or not the system is currently using the
 * intel_pstate driver to handle CPU frequency scaling.
 */
bool Cpu::findPstate() const
{
	const std::string driver = sysfs.read("cpu0/cpufreq/scaling_driver");
	if (driver != BAD_READ) {
		if (Log::isDebug()) {
			std::cout << "[Debug] driver is: '" << driver << "'"
				  << std::endl;
		}
		return (driver.compare("intel_pstate") == 0);
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] "
				<< "Unable to get read driver to check for"
				<< " intel_pstate" << Color::reset()
				<< std::endl;
		}
		return false;
	}
}

/*
 * Find the total number of CPUS (logical and physical) that exist on the
 * system.
 */
unsigned int Cpu::findNumber() const
{
	const char *cmd = "grep processor /proc/cpuinfo | wc -l";
	const std::vector<std::string> result = sysfs.readPipe(cmd, 1);
	if (!result.empty()) {
		/*
		 * Handle stringToNumber errors
		 */
		const int n = stringToNumber(result[0]);
		if (Log::isDebug()) {
			std::cout << "[Debug] cpu number is: '" << n << "'"
				  << std::endl;
		}
		if (n == BAD_NUMBER) {
			return NO_CPUS;
		} else {
			return n;
		}
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] "
				<< "Unable to find number of CPUs"
				<< Color::reset() << std::endl;
		}
		return 0;
	}
}

/*
 * Fill a vector with path values to the cpufreq sysfs
 * of a certain index.
 */
void Cpu::initializeVector(std::vector<std::string> &vector,
		const std::string what) const
{
	if (Log::isDebug()) {
		std::cout << "[Debug] fill frequency vector: '"
			  << "cpu0/cpufreq/scaling_" << what << "'"
			  << std::endl;
	}
	for (unsigned int i = 0; i < number; ++i) {
		std::ostringstream oss;
		oss << "cpu" << i << "/cpufreq/scaling_" << what;
		vector.push_back(oss.str());
	}
}

/*
 * Find what the sytem considers to be its absolute maximum CPU frequency.
 */
double Cpu::findInfoMaxFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/cpuinfo_max_freq");
	if (line != BAD_READ) {
		/*
		 * Handle stringToNumber errors
		 */
		const double result = stringToNumber(line);
		if (Log::isDebug()) {
			std::cout << "[Debug] cpuinfo_max_freq is: '"
				  << result << "'" << std::endl;
		}
		if (result == BAD_NUMBER) {
			return NO_FREQ;
		} else {
			return result;
		}
	} else {
		std::cerr << Color::boldRed()
			<< "[Error] "
			<< "Unable to find cpuinfo_max_freq"
			<< Color::reset() << std::endl;
		return INFO_FREQUENCY_INSANE;
	}
}

/*
 * Find what the sytem considers to be its absolute minimum CPU frequency.
 */
double Cpu::findInfoMinFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/cpuinfo_min_freq");
	if (line != BAD_READ) {
		/*
		 * Handle stringToNumber errors
		 */
		const double result = stringToNumber(line);
		if (Log::isDebug()) {
			std::cout << "[Debug] cpuinfo_min_freq is: '"
				  << result << "'" << std::endl;
		}
		if (result == BAD_NUMBER) {
			return NO_FREQ;
		} else {
			return result;
		}
		return result;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] "
				<< "Unable to find cpuinfo_min_freq"
				<< Color::reset()
				<< std::endl;
		}
		return INFO_FREQUENCY_INSANE;
	}
}

}
