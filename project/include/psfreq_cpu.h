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

class Cpu {
private:
	class Sysfs {
	private:
		const Cpu &cpu;
		const std::string basePath;
		Sysfs();
	public:
		Sysfs(const Cpu& cpu) :
			cpu(cpu),
			basePath("/sys/devices/system/cpu/")
		{
		}

		~Sysfs()
		{
		}

		bool write(const std::string &path, const std::string &file,
				const std::string &buffer) const;
		bool write(const std::string &path,const std::string &file,
				const int number) const;
		bool write(const std::string &file,
				const std::string &buffer) const;
		bool write(const std::string &file, const int number) const;
		const std::string read(const std::string &file) const;
		const std::string read(const std::string &path,
				const std::string &file) const;
		const std::vector<std::string> readAll(
				const std::string &file) const;
		const std::vector<std::string> readAll(const std::string &path,
				const std::string &file) const;
		const std::vector<std::string> readPipe(const char* command,
				const unsigned int number) const;
	};

	const Sysfs sysfs;
	bool pstate;
	unsigned int number;
	double minInfoFrequency;
	double maxInfoFrequency;
	std::vector<std::string> maxFrequencyFileVector;
	std::vector<std::string> minFrequencyFileVector;
	std::vector<std::string> governorFileVector;

	void initializeVector(std::vector<std::string> &vector,
			std::string what) const;
	unsigned int findNumber() const;
	bool findPstate() const;
	double findInfoMinFrequency() const;
	double findInfoMaxFrequency() const;

public:
	Cpu() :
		sysfs(*this),
		pstate(false),
		number(0),
		minInfoFrequency(0),
		maxInfoFrequency(0),
		maxFrequencyFileVector(std::vector<std::string>()),
		minFrequencyFileVector(std::vector<std::string>()),
		governorFileVector(std::vector<std::string>())
	{
	}

	~Cpu()
	{
	}

	void init();
	void setScalingMax(const int max) const;
	void setScalingMin(const int min) const;
	void setTurboBoost(const int turbo) const;
	void setGovernor(const std::string &governor) const;
	bool hasPstate() const;
	bool hideDirectory(const std::string &entryName) const;
	int getTurboBoost() const;
	int getInfoMinValue() const;
	int getInfoMaxValue() const;
	int getMinValue() const;
	int getMaxValue() const;
	unsigned int getNumber() const;
	unsigned int getPowerSupply(const std::string &fullPath) const;
	double getScalingMinFrequency() const;
	double getScalingMaxFrequency() const;
	double getInfoMinFrequency() const;
	double getInfoMaxFrequency() const;
	const std::string getGovernor() const;
	const std::vector<std::string> getRealtimeFrequencies() const;
	const std::vector<std::string> getAvailableGovernors() const;
	const std::string getDriver() const;

};


}

#endif
