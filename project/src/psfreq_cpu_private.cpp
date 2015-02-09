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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_logger.h"
#include "include/psfreq_util.h"

namespace psfreq {

unsigned int cpu::findNumber() const
{
	const std::string cmd = "grep processor /proc/cpuinfo | wc -l";
	const char *command = cmd.c_str();
	std::FILE *pipe = popen(command, "r");
	char *line = NULL;
        size_t n = 0;
        if (getline(&line, &n, pipe) == -1) {
		std::ostringstream oss;
		oss << PSFREQ_COLOR_BOLD_RED << "Could not get a line from file."
			<< PSFREQ_COLOR_OFF << std::endl;
		psfreq::logger::e(oss.str());
		psfreq::logger::close();
                exit(EXIT_FAILURE);
        }
	pclose(pipe);
	const unsigned int result = stringToNumber(line);
	free(line);
	return result;
}

void cpu::initializeVector(std::vector<std::string> &vector, std::string what) const
{
	for (unsigned int i = 0; i < number; ++i) {
		std::ostringstream oss;
		oss << "cpu" << i << "/cpufreq/scaling_" << what;
		vector.push_back(oss.str());
	}
}

double cpu::findInfoMaxFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_max_freq");
	const double result = stringToNumber(line);
	return result;
}

double cpu::findInfoMinFrequency() const
{
	const std::string line = cpuSysfs.read("cpu0/cpufreq/cpuinfo_min_freq");
	const double result = stringToNumber(line);
	return result;
}

}
