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

#include "include/psfreq_sysfs.h"

namespace psfreq {

class cpu {
private:
	sysfs cpuSysfs;
	unsigned int number;
	double minInfoFrequency;
	double maxInfoFrequency;
	std::vector<std::string> maxFrequencyFileVector;
	std::vector<std::string> minFrequencyFileVector;

	void initializeVector(std::vector<std::string> &vector, std::string minOrMax) const;
	unsigned int findNumber() const;
	double findInfoMinFrequency() const;
	double findInfoMaxFrequency() const;

public:
	cpu();
	~cpu();
	void setSaneDefaults() const;
	void setScalingMax(const int max) const;
	void setScalingMin(const int min) const;
	void setTurboBoost(const int turbo) const;
	int getTurboBoost() const;
	int getNumber() const;
	int getInfoMinValue() const;
	int getInfoMaxValue() const;
	int getMinPState() const;
	int getMaxPState() const;
	double getScalingMinFrequency() const;
	double getScalingMaxFrequency() const;
	double getInfoMinFrequency() const;
	double getInfoMaxFrequency() const;
	const std::string getGovernor() const;
	const std::string getDriver() const;
	const std::string getIOScheduler() const;
};


}

#endif
