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

#ifndef CPP_PSFREQ_CPU_H
#define CPP_PSFREQ_CPU_H

#include <string>
#include <vector>

#include "include/psfreq_logger.h"
#include "include/psfreq_sysfs.h"

namespace psfreq {

class cpu {
private:
	static bool pstate;

	sysfs cpuSysfs;
	unsigned int number;
	double minInfoFrequency;
	double maxInfoFrequency;
	std::vector<std::string> maxFrequencyFileVector;
	std::vector<std::string> minFrequencyFileVector;
	std::vector<std::string> governorFileVector;

	void initializeVector(std::vector<std::string> &vector, std::string what) const;
	unsigned int findNumber() const;
	double findInfoMinFrequency() const;
	double findInfoMaxFrequency() const;

	static bool findPstate()
	{
		std::ostringstream log;
		if (logger::isDebug()) {
			log << "pstate-frequency [psfreq_cpu_private.cpp]: findPstate"
				<< std::endl;
			logger::d(log);
		}
		if (logger::isDebug()) {
			log << "\tCheck for presence of pstate driver"
				<< std::endl;
			logger::d(log);
		}
		sysfs cpuSysfs;
		const std::string driver = cpuSysfs.read("cpu0/cpufreq/scaling_driver");
		if (logger::isDebug()) {
			log << "Compare found: " << driver << " with driver: intel_pstate"
				<< std::endl;
			logger::d(log);
		}
		return (driver.compare("intel_pstate") == 0);
	}

public:
	cpu();
	~cpu();
	void setSaneDefaults() const;
	void setScalingMax(const int max) const;
	void setScalingMin(const int min) const;
	void setTurboBoost(const int turbo) const;
	void setGovernor(const std::string &governor) const;
	int getTurboBoost() const;
	unsigned int getNumber() const;
	int getInfoMinValue() const;
	int getInfoMaxValue() const;
	int getMinPState() const;
	int getMaxPState() const;
	double getScalingMinFrequency() const;
	double getScalingMaxFrequency() const;
	double getInfoMinFrequency() const;
	double getInfoMaxFrequency() const;
	const std::string getGovernor() const;
	const std::vector<std::string> getRealtimeFrequencies() const;
	const std::vector<std::string> getAvailableGovernors() const;
	const std::string getDriver() const;
	const std::string getIOScheduler() const;

	static bool hasPstate()
	{
		return pstate;
	}
};


}

#endif
