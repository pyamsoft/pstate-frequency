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

#include <iostream>
#include <sstream>

#include <unistd.h>

#include "psfreq_color.h"
#include "psfreq_cpu.h"
#include "psfreq_log.h"
#include "psfreq_util.h"

namespace psfreq {

const std::string Cpu::getGovernor() const
{
        const std::string line = sysfs.read("cpu0/cpufreq/scaling_governor");
        if (line != BAD_READ) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Check the scaling_governor: '"
                                  << line << "'" << std::endl;
                }
                return line;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to get scaling_governor"
                                << Color::reset() << std::endl;
                }
                return BAD_READ;
        }
}

const std::vector<std::string> Cpu::getAvailableGovernors() const
{
        const std::vector<std::string> availableGovernors
                = sysfs.readAll("cpu0/cpufreq/scaling_available_governors");
        if (!availableGovernors.empty()) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Found available governors vector"
                                  << std::endl;
                }
                return availableGovernors;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to get a list of available "
                                << "governors" << Color::reset() << std::endl;
                }
                return BAD_VECTOR;
        }
}

void Cpu::setGovernor(const std::string &governor) const
{
        if (number == governorFileVector.size()) {
                for (unsigned int i = 0; i < number; ++i) {
                        if (!sysfs.write(governorFileVector[i], governor)) {
                                if (!Log::isAllQuiet()) {
                                        std::cerr << Color::boldRed()
                                                << "[Error] "
                                                << "Failed to set"
                                                << " the governor of"
                                                << " CPU " << i
                                                << Color::reset()
                                                << std::endl;
                                }
                        }
                }
        }
}

}

