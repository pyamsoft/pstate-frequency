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

int Cpu::getTurboBoost() const
{
        const std::string line = sysfs.read(hasPstate()
                        ? "intel_pstate/no_turbo"
                        : "cpufreq/boost");
        if (line != BAD_READ) {
                const int result = stringToNumber(line);
                if (Log::isDebug()) {
                        std::cout << "[Debug] Check the no_turbo: '"
                                  << result << "'" << std::endl;
                }
                return (result == BAD_NUMBER)
                        ? TURBO_BOOST_INSANE
                        : result;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Unable to read turbo boost value"
                                << Color::reset() << std::endl;
                }
                return TURBO_BOOST_INSANE;
        }
}

void Cpu::setTurboBoost(const int turbo) const
{
        const std::string file = hasPstate()
                ? "intel_pstate/no_turbo"
                : "cpufreq/boost";
        if (!sysfs.write(file, turbo)) {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to set the turbo"
                                << Color::reset()
                                << std::endl;
                }
        }
}

}

