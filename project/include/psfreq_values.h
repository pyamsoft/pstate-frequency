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

#ifndef CPP_PSFREQ_VALUES_H
#define CPP_PSFREQ_VALUES_H

#include <string>

#include "include/psfreq_cpu.h"

namespace psfreq {

class values {
private:
	const cpu &parent;
	int action;
	int max;
	int min;
	int turbo;
	std::string governor;
	int requested;

	void setPlanPowersave();
	void setPlanPerformance();
	void setPlanMaxPerformance();
	bool setPlanAuto();

	bool hideDirectory(const std::string &entryName);
	bool discoverPowerSupply(const std::string &fullPath);
	values();

public:
	values(const cpu &parentCpu) :
		parent(parentCpu),
		action(-1),
		max(-1),
		min(-1),
		turbo(-1),
		governor(std::string()),
		requested(0)
	{
	}

	~values()
	{
	}

	void setAction(const int newAction);
	void setMax(const int newMax);
	void setMin(const int newMin);
	void setTurbo(const int newTurbo);
	bool setPlan(const int plan);
	void setRequested(const int newRequest);
	bool setGovernor(const std::string& newGovernor);

	int getAction() const;
	int getMax() const;
	int getMin() const;
	int getTurbo() const;
	int getRequested() const;
	const std::string getGovernor() const;

	bool hasAction() const;
	bool isActionNull() const;
	bool isActionGet() const;
	bool isActionSet() const;
	bool isInitialized() const;
};

}

#endif
