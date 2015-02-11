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
#include "include/psfreq_logger.h"
#include "include/psfreq_util.h"

namespace psfreq {


cpu::cpu()
{
	number = findNumber();
	minInfoFrequency = findInfoMinFrequency();
	maxInfoFrequency = findInfoMaxFrequency();
	minFrequencyFileVector = std::vector<std::string>();
	maxFrequencyFileVector = std::vector<std::string>();
	governorFileVector = std::vector<std::string>();
	initializeVector(minFrequencyFileVector, "min_freq");
	initializeVector(maxFrequencyFileVector, "max_freq");
	initializeVector(governorFileVector, "governor");
}

cpu::~cpu()
{
}


double cpu::getScalingMinFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/scaling_min_freq");
	const double result = stringToNumber(line);
	return result;
}

double cpu::getScalingMaxFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/scaling_max_freq");
	const double result = stringToNumber(line);
	return result;
}

double cpu::getInfoMinFrequency() const
{
	return minInfoFrequency;
}

double cpu::getInfoMaxFrequency() const
{
	return maxInfoFrequency;
}

unsigned int cpu::getNumber() const
{
	return number;
}

const std::string cpu::getGovernor() const
{
	return cpuSysfs.read("cpu0/cpufreq/scaling_governor");
}

const std::string cpu::getIOScheduler() const
{
	return cpuSysfs.read("/sys/block/", "sda/queue/scheduler");
}

const std::string cpu::getDriver() const
{
	return cpuSysfs.read("cpu0/cpufreq/scaling_driver");
}

const std::vector<std::string> cpu::getRealtimeFrequencies() const
{
	std::ostringstream log;
	if (logger::isDebug()) {
		log << "pstate-frequency [psfreq_cpu_public.cpp]: cpu::getRealtimeFrequencies"
			<< std::endl;
		logger::d(log);
	}

	const char *cmd = "grep MHz /proc/cpuinfo";
	return cpuSysfs.readPipe(cmd, number);
}

const std::vector<std::string> cpu::getAvailableGovernors() const
{
	std::vector<std::string> availableGovernors = std::vector<std::string>();
	availableGovernors = cpuSysfs.readAll("cpu0/cpufreq/scaling_available_governors");
	return availableGovernors;
}

int cpu::getMaxPState() const
{
	return static_cast<int>(getScalingMaxFrequency() / getInfoMaxFrequency() * 100);
}

int cpu::getMinPState() const
{
	return static_cast<int>(getScalingMinFrequency() / getInfoMaxFrequency() * 100);
}

int cpu::getTurboBoost() const
{
	if (hasPstate()) {
		const std::string line = cpuSysfs.read("intel_pstate/no_turbo");
		const int result = stringToNumber(line);
		return result;
	} else {
		return -1;
	}
}

int cpu::getInfoMinValue() const
{
	return static_cast<int>(minInfoFrequency / maxInfoFrequency * 100);
}
int cpu::getInfoMaxValue() const
{
	return 100;
}

void cpu::setSaneDefaults() const
{
	std::ostringstream log;
	if (logger::isDebug()) {
		log << "pstate-frequency [psfreq_cpu_public.cpp]: cpu::setSaneDefaults"
			<< std::endl;
		logger::d(log);
	}


	if (logger::isDebug()) {
		log << "\tDefaults are set to MAX: 100, MIN: 0, TURBO: 1, GOVERNOR: powersave"
			<< std::endl;
		logger::d(log);
	}

	setScalingMax(100);
	setScalingMin(0);
	setTurboBoost(1);
	setGovernor("powersave");
}

void cpu::setScalingMax(const int max) const
{
	if (number == maxFrequencyFileVector.size()) {
		const int scalingMax = maxInfoFrequency / 100 * max;
		for (unsigned int i = 0; i < number; ++i) {
			cpuSysfs.write(maxFrequencyFileVector[i], scalingMax);
		}
		if (hasPstate()) {
			cpuSysfs.write("intel_pstate/max_perf_pct", max);
		}
	}
}

void cpu::setScalingMin(const int min) const
{
	if (number == minFrequencyFileVector.size()) {
		const int scalingMin = maxInfoFrequency / 100 * min;
		for (unsigned int i = 0; i < number; ++i) {
			cpuSysfs.write(minFrequencyFileVector[i], scalingMin);
		}
		if (hasPstate()) {
			cpuSysfs.write("intel_pstate/min_perf_pct", min);
		}
	}
}

void cpu::setTurboBoost(const int turbo) const
{
	if (hasPstate()) {
		cpuSysfs.write("intel_pstate/no_turbo", turbo);
	}
}

void cpu::setGovernor(const std::string &governor) const
{
	if (number == governorFileVector.size()) {
		for (unsigned int i = 0; i < number; ++i) {
			cpuSysfs.write(governorFileVector[i], governor);
		}
	}
}


}
