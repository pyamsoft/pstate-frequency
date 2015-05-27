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

#include <unistd.h>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_log.h"
#include "include/psfreq_util.h"

namespace psfreq {

const std::vector<std::string> Cpu::BAD_VECTOR = std::vector<std::string>();
const std::string Cpu::BAD_READ = UNINITIALIZED_STR;
const std::string Cpu::GOVERNOR_INSANE = UNINITIALIZED_STR;
const double Cpu::INFO_FREQUENCY_INSANE = 1.0;
const double Cpu::SCALING_FREQUENCY_INSANE = 0.0;
const int Cpu::PSTATE_VALUE_INSANE = 0;
const int Cpu::TURBO_BOOST_INSANE = -2;
const int Cpu::POWER_SOURCE_NONE = 0;
const int Cpu::POWER_SOURCE_BAT = 1;
const int Cpu::POWER_SOURCE_AC = 2;

int Cpu::getInfoMinValue() const
{
	return static_cast<int>(minInfoFrequency / maxInfoFrequency * 100);
}
int Cpu::getInfoMaxValue() const
{
	return 100;
}

/*
 * Initialize the CPU. This function is meant to be called only once, and
 * will allow the CPU to be able to handle the cpufreq sysfs.
 */
bool Cpu::init()
{
	/*
	 * If the init function has not been called before,
	 * initialize the CPU.
	 */
	if (!initialized) {
		if (Log::isDebug()) {
			std::cout << "[Debug] Initialize CPU" << std::endl;
		}
		number = findNumber();
		pstate = findPstate();
		minInfoFrequency = findInfoMinFrequency();
		maxInfoFrequency = findInfoMaxFrequency();
		if (minInfoFrequency == NO_FREQ || maxInfoFrequency == NO_FREQ
				|| number == NO_CPUS) {
			if (!Log::isAllQuiet()) {
				std::cerr << Color::boldRed()
					<< "[Error] Unable to find CPU vars"
					<< Color::reset() << std::endl;
			}
			return false;
		}
		initializeVector(minFrequencyFileVector, "min_freq");
		initializeVector(maxFrequencyFileVector, "max_freq");
		initializeVector(governorFileVector, "governor");
		initialized = true;
		return true;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] CPU already initialized"
				<< Color::reset() << std::endl;
		}
		return false;
	}
}

bool Cpu::hasPstate() const
{
	return pstate;
}

double Cpu::getScalingMinFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_min_freq");
	if (line != BAD_READ) {
		const double result = stringToNumber(line);
		if (Log::isDebug()) {
			std::cout << "[Debug] Check the scaling_min_freq: '"
				  << static_cast<int>(result) << "'"
				  << std::endl;
		}
		return (result == BAD_NUMBER)
			? SCALING_FREQUENCY_INSANE
			: result;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get scaling_min_freq"
				<< Color::reset() << std::endl;
		}
		return SCALING_FREQUENCY_INSANE;
	}
}

double Cpu::getScalingMaxFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_max_freq");
	if (line != BAD_READ) {
		const double result = stringToNumber(line);
		if (Log::isDebug()) {
			std::cout << "[Debug] Check the scaling_max_freq: '"
				  << static_cast<int>(result) << "'"
				  << std::endl;
		}
		return (result == BAD_NUMBER)
			? SCALING_FREQUENCY_INSANE
			: result;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get scaling_max_freq"
				<< Color::reset() << std::endl;
		}
		return SCALING_FREQUENCY_INSANE;
	}
}


const std::string Cpu::getGovernor() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_governor");
	if (line != BAD_READ) {
		if (Log::isDebug()) {
			std::cout << "[Debug] Check the scaling_governor: '"
				  << line << "'" << std::endl;
		}
		return line;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get scaling_governor"
				<< Color::reset() << std::endl;
		}
		return BAD_READ;
	}
}

const std::string Cpu::getDriver() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_driver");
	if (line != BAD_READ) {
		if (Log::isDebug()) {
			std::cout << "[Debug] Check the scaling_driver: '"
				  << line << "'" << std::endl;
		}
		return line;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get scaling_driver"
				<< Color::reset() << std::endl;
		}
		return BAD_READ;
	}
}

const std::vector<std::string> Cpu::getRealtimeFrequencies() const
{
	const char *cmd = "grep MHz /proc/cpuinfo | cut -c12-";
	const std::vector<std::string> result = sysfs.readPipe(cmd, number);
	if (!result.empty()) {
		if (Log::isDebug()) {
			std::cout << "[Debug] Found realtime frequency vector"
				  << std::endl;
		}
		return result;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get realtime frequencies"
				<< Color::reset() << std::endl;
		}
		return BAD_VECTOR;
	}
}

const std::vector<std::string> Cpu::getAvailableGovernors() const
{
	const std::vector<std::string> availableGovernors
		= sysfs.readAll("cpu0/cpufreq/scaling_available_governors");
	if (!availableGovernors.empty()) {
		if (Log::isDebug()) {
			std::cout << "[Debug] Found available governors vector"
				  << std::endl;
		}
		return availableGovernors;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to get a list of available "
				<< "governors" << Color::reset() << std::endl;
		}
		return BAD_VECTOR;
	}
}

int Cpu::getMaxValue() const
{
	return static_cast<int>(getScalingMaxFrequency()
			/ getInfoMaxFrequency() * 100);
}

int Cpu::getMinValue() const
{
	return static_cast<int>(getScalingMinFrequency()
			/ getInfoMaxFrequency() * 100);
}

int Cpu::getTurboBoost() const
{
	const std::string line = sysfs.read(hasPstate()
			? "intel_pstate/no_turbo"
			: "cpufreq/boost");
	if (line != BAD_READ) {
		const int result = stringToNumber(line);
		if (Log::isDebug()) {
			std::cout << "[Debug] Check the no_turbo: '"
				  << result << "'" << std::endl;
		}
		return (result == BAD_NUMBER)
			? TURBO_BOOST_INSANE
			: result;
	} else {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Unable to read turbo boost value"
				<< Color::reset() << std::endl;
		}
		return TURBO_BOOST_INSANE;
	}
}

void Cpu::setScalingMax(const int max) const
{
	if (number == maxFrequencyFileVector.size()) {
		const int scalingMax = maxInfoFrequency / 100 * max;
		for (unsigned int i = 0; i < number; ++i) {
			if (!sysfs.write(maxFrequencyFileVector[i],
					scalingMax)) {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "[Error] Failed to set"
						<< " the max frequency of"
						<< " CPU " << i
						<< Color::reset() << std::endl;
				}
			}
		}

		if (hasPstate()) {
			if (!sysfs.write("intel_pstate/max_perf_pct", max)) {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "[Error] "
						<< "Failed to set the pstate"
						<< " max" << Color::reset()
						<< std::endl;
				}
			}
		}
	}
}

void Cpu::setScalingMin(const int min) const
{
	if (number == minFrequencyFileVector.size()) {
		const int scalingMin = maxInfoFrequency / 100 * min;
		for (unsigned int i = 0; i < number; ++i) {
			if (!sysfs.write(minFrequencyFileVector[i],
					scalingMin)) {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "[Error] "
						<< "Failed to set"
						<< " the min frequency of"
						<< " CPU " << i
						<< Color::reset()
						<< std::endl;
				}
			}
		}

		if (hasPstate()) {
			if (!sysfs.write("intel_pstate/min_perf_pct", min)) {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "[Error] "
						<< "Failed to set the pstate"
						<< " min" << Color::reset()
						<< std::endl;
				}
			}
		}
	}
}

void Cpu::setTurboBoost(const int turbo) const
{
	const std::string file = hasPstate()
		? "intel_pstate/no_turbo"
		: "cpufreq/boost";
	if (!sysfs.write(file, turbo)) {
		if (!Log::isAllQuiet()) {
			std::cerr << Color::boldRed()
				<< "[Error] Failed to set the turbo"
				<< Color::reset()
				<< std::endl;
		}
	}
}

void Cpu::setGovernor(const std::string &governor) const
{
	if (number == governorFileVector.size()) {
		for (unsigned int i = 0; i < number; ++i) {
			if (!sysfs.write(governorFileVector[i], governor)) {
				if (!Log::isAllQuiet()) {
					std::cerr << Color::boldRed()
						<< "[Error] "
						<< "Failed to set"
						<< " the governor of"
						<< " CPU " << i
						<< Color::reset()
						<< std::endl;
				}
			}
		}
	}
}

/*
 * Given a sysfs power_supply path, decide whether the power_supply is
 * the 'Mains' type adapter and if so, return whether or not the adapter
 * is online.
 */
unsigned int Cpu::getPowerSupply(const std::string &fullPath) const
{
	std::ostringstream oss;
	oss << fullPath << "/type";
	const std::string typePath = oss.str();
	const char *const type = typePath.c_str();
	if (Log::isDebug()) {
		std::cout << "[Debug] getting power supply from path: '"
			  << typePath << "'" << std::endl;
	}
	if (access(type, F_OK) != -1) {
		const std::string powerType = sysfs.read(fullPath, "type");
		if (Log::isDebug()) {
			std::cout << "[Debug] power type: '"
				<< powerType << "'" << std::endl;
		}
		if (powerType.compare("Mains") == 0) {
			const int status = stringToNumber(
					sysfs.read(fullPath, "online"));
			if (Log::isDebug()) {
				std::cout << "[Debug] power status: '"
					<< status << "'" << std::endl;
			}
			if (status == BAD_NUMBER) {
				return POWER_SOURCE_NONE;
			} else if (status == 1) {
				return POWER_SOURCE_AC;
			} else {
				return POWER_SOURCE_BAT;
			}
		}
	}
	return POWER_SOURCE_NONE;
}

}

