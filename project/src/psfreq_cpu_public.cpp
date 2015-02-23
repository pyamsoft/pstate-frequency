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

void Cpu::init()
{
	number = findNumber();
	pstate = findPstate();
	minInfoFrequency = findInfoMinFrequency();
	maxInfoFrequency = findInfoMaxFrequency();
	initializeVector(minFrequencyFileVector, "min_freq");
	initializeVector(maxFrequencyFileVector, "max_freq");
	initializeVector(governorFileVector, "governor");
}

bool Cpu::hasPstate() const
{
	return pstate;
}

double Cpu::getScalingMinFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_min_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	std::cerr << "Failed to get scaling_min_freq" << std::endl;
	return 0.0;
}

double Cpu::getScalingMaxFrequency() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_max_freq");
	if (line != std::string()) {
		const double result = stringToNumber(line);
		return result;
	}
	std::cerr << "Failed to get scaling_max_freq" << std::endl;
	return 0.0;
}

double Cpu::getInfoMinFrequency() const
{
	return minInfoFrequency;
}

double Cpu::getInfoMaxFrequency() const
{
	return maxInfoFrequency;
}

unsigned int Cpu::getNumber() const
{
	return number;
}

const std::string Cpu::getGovernor() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_governor");
	if (line != std::string()) {
		return line;
	}
	std::cerr << "Failed to get scaling_governor" << std::endl;
	return line;
}

const std::string Cpu::getDriver() const
{
	const std::string line = sysfs.read("cpu0/cpufreq/scaling_driver");
	if (line != std::string()) {
		return line;
	}
	std::cerr << "Failed to get scaling_driver" << std::endl;
	return line;
}

const std::vector<std::string> Cpu::getRealtimeFrequencies() const
{
	const char *cmd = "grep MHz /proc/cpuinfo | cut -c12-";
	const std::vector<std::string> result = sysfs.readPipe(cmd, number);
	if (!result.empty()) {
		return result;
	}
	std::cerr << "Failed to get realtime frequencies" << std::endl;
	return result;
}

const std::vector<std::string> Cpu::getAvailableGovernors() const
{
	const std::vector<std::string> availableGovernors
		= sysfs.readAll("cpu0/cpufreq/scaling_available_governors");
	if (!availableGovernors.empty()) {
		return availableGovernors;
	}
	std::cerr << "Failed to get a list of available governors"
		<< std::endl;
	return availableGovernors;
}

int Cpu::getMaxPState() const
{
	return static_cast<int>(getScalingMaxFrequency()
			/ getInfoMaxFrequency() * 100);
}

int Cpu::getMinPState() const
{
	return static_cast<int>(getScalingMinFrequency()
			/ getInfoMaxFrequency() * 100);
}

int Cpu::getTurboBoost() const
{
	const std::string line = sysfs.read(hasPstate()
			? "intel_pstate/no_turbo"
			: "cpufreq/boost");
	if (line != std::string()) {
		const int result = stringToNumber(line);
		return result;
	}
	std::cerr << "Unable to read turbo boost value" << std::endl;
	return -2;
}

int Cpu::getInfoMinValue() const
{
	return static_cast<int>(minInfoFrequency / maxInfoFrequency * 100);
}
int Cpu::getInfoMaxValue() const
{
	return 100;
}

void Cpu::setScalingMax(const int max) const
{
	if (number == maxFrequencyFileVector.size()) {
		const int scalingMax = maxInfoFrequency / 100 * max;
		for (unsigned int i = 0; i < number; ++i) {
			if (!sysfs.write(maxFrequencyFileVector[i],
					scalingMax)) {
				std::cerr << "Failed to set"
					<< " the max frequency of CPU " << i
					<< std::endl;
			}
		}
		if (hasPstate()) {
			if (!sysfs.write("intel_pstate/max_perf_pct",
					max)) {
				std::cerr << "Failed to set the pstate max"
					<< std::endl;
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
				std::cerr << "Failed to set"
					<< " the min frequency of CPU " << i
					<< std::endl;
			}
		}
		if (hasPstate()) {
			if (!sysfs.write("intel_pstate/min_perf_pct",
					min)) {
				std::cerr << "Failed to set the pstate min"
					<< std::endl;
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
		std::cerr << "Failed to set the turbo"
			<< std::endl;
	}
}

void Cpu::setGovernor(const std::string &governor) const
{
	if (number == governorFileVector.size()) {
		for (unsigned int i = 0; i < number; ++i) {
			if (!sysfs.write(governorFileVector[i], governor)) {
				std::cerr << "Failed to set"
					<< " the governor of CPU " << i
					<< std::endl;
			}
		}
	}
}

unsigned int Cpu::getPowerSupply(const std::string &fullPath) const
{
	std::ostringstream oss;
	oss << fullPath << "/type";
	const std::string typePath = oss.str();
	const char *const type = typePath.c_str();
	if (access(type, F_OK) != -1) {
		const std::string powerType = sysfs.read(fullPath, "type");
		if (powerType.compare("Mains") == 0) {
			const int status = stringToNumber(
					sysfs.read(fullPath, "online"));
			if (status == 1) {
				return 2;
			} else {
				return 1;
			}
		}
	}
	return 0;
}

bool Cpu::hideDirectory(const std::string &entryName) const
{
	return (entryName.compare("..") == 0 || entryName.compare(".") == 0);
}

}
