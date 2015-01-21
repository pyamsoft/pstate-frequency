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

#ifndef CPP_PSFREQ_CPUVALUES_H
#define CPP_PSFREQ_CPUVALUES_H

#include <string>

namespace psfreq {

class cpuValues {

private:
	int action;
	int max;
	int min;
	int turbo;

	void setPlanPowersave();
	void setPlanPerformance();
	void setPlanMaxPerformance();
	void setPlanAuto();

	bool hideDirectory(const std::string &entryName);
	bool discoverPowerSupply(const std::string &fullPath);

public:

	cpuValues();
	~cpuValues();

	void setAction(const int newAction);
	void setMax(const int newMax);
	void setMin(const int newMin);
	void setTurbo(const int newTurbo);
	void setPlan(const int plan);

	int getAction() const;
	int getMax() const;
	int getMin() const;
	int getTurbo() const;

	bool hasAction() const;
	bool isActionNull() const;
	bool isActionGet() const;
	bool isActionSet() const;
};

}

#endif
