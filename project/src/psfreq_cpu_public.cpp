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

#include "include/psfreq_cpu.h"
#include "include/psfreq_util.h"

namespace psfreq {


cpu::cpu()
{
	number = findNumber();
	minInfoFrequency = findInfoMinFrequency();
	maxInfoFrequency = findInfoMaxFrequency();
	minFrequencyFileVector = std::vector<std::string>();
	maxFrequencyFileVector = std::vector<std::string>();
	initializeVector(minFrequencyFileVector, "min_freq");
	initializeVector(maxFrequencyFileVector, "max_freq");
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

int cpu::getNumber() const
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

int cpu::getMaxPState() const
{
	const std::string line = cpuSysfs.read("intel_pstate/max_perf_pct");
	const int result = stringToNumber(line);
	return result;
}

int cpu::getMinPState() const
{
	const std::string line = cpuSysfs.read("intel_pstate/min_perf_pct");
	const int result = stringToNumber(line);
	return result;
}

int cpu::getTurboBoost() const
{
	const std::string line = cpuSysfs.read("intel_pstate/no_turbo");
	const int result = stringToNumber(line);
	return result;
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
	setScalingMax(100);
	setScalingMin(0);
	setTurboBoost(1);
}

void cpu::setScalingMax(const int max) const
{
	if (number == maxFrequencyFileVector.size()) {
		const int scalingMax = maxInfoFrequency / 100 * max;
		for (unsigned int i = 0; i < number; ++i) {
			cpuSysfs.write(maxFrequencyFileVector[i], scalingMax);
		}
		cpuSysfs.write("intel_pstate/max_perf_pct", max);
	}
}

void cpu::setScalingMin(const int min) const
{
	if (number == minFrequencyFileVector.size()) {
		const int scalingMin = maxInfoFrequency / 100 * min;
		for (unsigned int i = 0; i < number; ++i) {
			cpuSysfs.write(minFrequencyFileVector[i], scalingMin);
		}
		cpuSysfs.write("intel_pstate/min_perf_pct", min);
	}
}

void cpu::setTurboBoost(const int turbo) const
{
	cpuSysfs.write("intel_pstate/no_turbo", turbo);
}


}
