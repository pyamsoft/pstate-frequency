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
 * For questions please contact pyamsoft at pyam(dot)soft(at)gmail(dot)com
 */

#ifndef CPP_PSFREQ_VALUES_H
#define CPP_PSFREQ_VALUES_H

#include <string>
#include <dirent.h>

#include "psfreq_cpu.h"
#include "psfreq_util.h"

namespace psfreq {

class Values {
private:
	const Cpu &cpu;
	int action;
	int plan;
	int max;
	int min;
	int turbo;
	bool doSleep;
	std::string governor;
	int requested;

	void setPlanPowersave();
	void setPlanPerformance();
	void setPlanMaxPerformance();
	void setPlanCustom(const std::vector<std::string> &ac);
	unsigned int setPlanAuto();
	unsigned int getPowerSourceDirectory(DIR *const directory,
		const char *const dirName);
	bool hideDirectory(const std::string &entryName);
	void setPlanAutoAC();
	void setPlanAutoBat();

	Values();

public:
	Values(const Cpu &cpu) :
		cpu(cpu),
		action(ACTION_NULL),
		plan(POWER_PLAN_NONE),
		max(UNINITIALIZED),
		min(UNINITIALIZED),
		turbo(TURBO_INSANE),
		doSleep(SLEEP),
		governor(UNINITIALIZED_STR),
		requested(REQUESTED_CURRENT)
	{
	}

	~Values()
	{
	}

	void setAction(const int newAction);
	bool setMax(const int newMax);
	bool setMin(const int newMin);
	bool setTurbo(const int newTurbo);
	void setRequested(const int newRequest);
	bool setPlan(const int powerPlan);
	bool setGovernor(const std::string& newGovernor);
	void noSleep();

	int getAction() const;
	int getMax() const;
	int getMin() const;
	int getTurbo() const;
	int getRequested() const;
	const std::string getGovernor() const;

	bool hasAction() const;
	bool hasPlan() const;
	bool isActionNull() const;
	bool isActionGet() const;
	bool isActionSet() const;
	bool isInitialized() const;
	bool shouldSleep() const;

	bool runPlan();

	static const std::string BAD_READ;
	static const int ACTION_NULL;
	static const int ACTION_SET;
	static const int ACTION_GET;
	static const int REQUESTED_CURRENT;
	static const int REQUESTED_REAL;
	static const int POWER_PLAN_NONE;
	static const int POWER_PLAN_POWERSAVE;
	static const int POWER_PLAN_PERFORMANCE;
	static const int POWER_PLAN_MAX_PERFORMANCE;
	static const int POWER_PLAN_AUTO;
	static const int AUTO_NONE;
	static const int AUTO_POWERSAVE;
	static const int AUTO_PERFORMANCE;
	static const int CPUFREQ_NO_TURBO;
	static const int CPUFREQ_TURBO;
	static const int PSTATE_NO_TURBO;
	static const int PSTATE_TURBO;
	static const int TURBO_INSANE;
	static const int MIN_POSSIBLE_FREQ;
	static const int MAX_POSSIBLE_FREQ;
	static const int UNINITIALIZED;
	static const bool NO_SLEEP;
	static const bool SLEEP;
	static const unsigned int CUSTOM_NUMBER;

};

}

#endif
