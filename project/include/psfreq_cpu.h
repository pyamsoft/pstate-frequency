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

namespace psfreq {

class cpu {
public:

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
		void setPlanAuto();

		bool hideDirectory(const std::string &entryName);
		bool discoverPowerSupply(const std::string &fullPath);
		values();

	public:

		values(cpu &parentCpu) :
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
		void setPlan(const int plan);
		void setRequested(const int newRequest);
		void setGovernor(const std::string& newGovernor);

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

private:

	class sysfs {
	private:
		const cpu &parent;
		const std::string basePath;
		sysfs();
	public:
		sysfs(cpu& parentCpu) :
			parent(parentCpu),
			basePath("/sys/devices/system/cpu/")
		{
		}

		~sysfs()
		{
		}

		bool exists(const std::string &path, const std::string &file) const;
		bool exists(const std::string &file) const;
		void write(const std::string &path, const std::string &file,
				const std::string &buffer) const;
		void write(const std::string &path,const std::string &file,
				const int number) const;
		void write(const std::string &file, const std::string &buffer) const;
		void write(const std::string &file, const int number) const;
		const std::string read(const std::string &file) const;
		const std::string read(const std::string &path,
				const std::string &file) const;
		const std::vector<std::string> readAll(const std::string &file) const;
		const std::vector<std::string> readAll(const std::string &path,
				const std::string &file) const;
		const std::vector<std::string> readPipe(const char* command,
				const unsigned int number) const;
	};

	const sysfs cpuSysfs;

	bool pstate;
	unsigned int number;
	double minInfoFrequency;
	double maxInfoFrequency;
	std::vector<std::string> maxFrequencyFileVector;
	std::vector<std::string> minFrequencyFileVector;
	std::vector<std::string> governorFileVector;

	void initializeVector(std::vector<std::string> &vector, std::string what) const;
	unsigned int findNumber() const;
	bool findPstate() const;
	double findInfoMinFrequency() const;
	double findInfoMaxFrequency() const;


public:
	values cpuValues;

	cpu() :
		cpuSysfs(*this),
		pstate(false),
		number(0),
		minInfoFrequency(0),
		maxInfoFrequency(0),
		maxFrequencyFileVector(std::vector<std::string>()),
		minFrequencyFileVector(std::vector<std::string>()),
		governorFileVector(std::vector<std::string>()),
		cpuValues(*this)
	{
	}

	~cpu()
	{
	}

	void init();
	void setSaneDefaults() const;
	void setScalingMax(const int max) const;
	void setScalingMin(const int min) const;
	void setTurboBoost(const int turbo) const;
	void setGovernor(const std::string &governor) const;
	values* getCpuValues() const;
	bool hasPstate() const;
	int getTurboBoost() const;
	int getInfoMinValue() const;
	int getInfoMaxValue() const;
	int getMinPState() const;
	int getMaxPState() const;
	unsigned int getNumber() const;
	double getScalingMinFrequency() const;
	double getScalingMaxFrequency() const;
	double getInfoMinFrequency() const;
	double getInfoMaxFrequency() const;
	const std::string getGovernor() const;
	const std::vector<std::string> getRealtimeFrequencies() const;
	const std::vector<std::string> getAvailableGovernors() const;
	const std::string getDriver() const;
	const std::string getIOScheduler() const;

};


}

#endif
