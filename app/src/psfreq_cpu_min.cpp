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

int Cpu::getInfoMinValue() const
{
        return static_cast<int>(minInfoFrequency / maxInfoFrequency * 100);
}

double Cpu::getScalingMinFrequency() const
{
        const std::string line = sysfs.read("cpu0/cpufreq/scaling_min_freq");
        if (line != BAD_READ) {
                const double result = stringToNumber(line);
                if (Log::isDebug()) {
                        std::cout << "[Debug] Check the scaling_min_freq: '"
                                  << static_cast<int>(result) << "'"
                                  << std::endl;
                }
                return (result == BAD_NUMBER)
                        ? SCALING_FREQUENCY_INSANE
                        : result;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to get scaling_min_freq"
                                << Color::reset() << std::endl;
                }
                return SCALING_FREQUENCY_INSANE;
        }
}

int Cpu::getMinValue() const
{
        return static_cast<int>(getScalingMinFrequency()
                        / getInfoMaxFrequency() * 100);
}

void Cpu::setScalingMin(const int min) const
{
        if (number == minFrequencyFileVector.size()) {
                const int scalingMin = maxInfoFrequency / 100 * min;
                for (unsigned int i = 0; i < number; ++i) {
                        if (!sysfs.write(minFrequencyFileVector[i],
                                        scalingMin)) {
                                if (!Log::isAllQuiet()) {
                                        std::cerr << Color::boldRed()
                                                << "[Error] "
                                                << "Failed to set"
                                                << " the min frequency of"
                                                << " CPU " << i
                                                << Color::reset()
                                                << std::endl;
                                }
                        }
                }

                if (hasPstate()) {
                        if (!sysfs.write("intel_pstate/min_perf_pct", min)) {
                                if (!Log::isAllQuiet()) {
                                        std::cerr << Color::boldRed()
                                                << "[Error] "
                                                << "Failed to set the pstate"
                                                << " min" << Color::reset()
                                                << std::endl;
                                }
                        }
                }
        }
}

}

