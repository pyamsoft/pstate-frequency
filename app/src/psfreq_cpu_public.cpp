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

const std::vector<std::string> Cpu::BAD_VECTOR = std::vector<std::string>();
const std::string Cpu::BAD_READ = UNINITIALIZED_STR;
const std::string Cpu::GOVERNOR_INSANE = UNINITIALIZED_STR;
const int Cpu::PSTATE_VALUE_INSANE = 0;
const double Cpu::SCALING_FREQUENCY_INSANE = 0.0;
const int Cpu::BAD_FILE_ACCESS = -1;
const int Cpu::TURBO_BOOST_INSANE = -2;
const unsigned int Cpu::POWER_SOURCE_NONE = 0;
const unsigned int Cpu::POWER_SOURCE_BAT = 1;
const unsigned int Cpu::POWER_SOURCE_AC = 2;

/*
 * Initialize the CPU. This function is meant to be called only once, and
 * will allow the CPU to be able to handle the cpufreq sysfs.
 */
bool Cpu::init()
{
        /*
         * If the init function has not been called before,
         * initialize the CPU.
         */
        if (!initialized) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Initialize CPU" << std::endl;
                }
                number = findNumber();
                pstate = findPstate();
                minInfoFrequency = findInfoMinFrequency();
                maxInfoFrequency = findInfoMaxFrequency();
                if (minInfoFrequency == NO_FREQ || maxInfoFrequency == NO_FREQ
                                || number == NO_CPUS) {
                        if (!Log::isAllQuiet()) {
                                std::cerr << Color::boldRed()
                                        << "[Error] Unable to find CPU vars"
                                        << Color::reset() << std::endl;
                        }
                        return false;
                }
                initializeVector(minFrequencyFileVector, "min_freq");
                initializeVector(maxFrequencyFileVector, "max_freq");
                initializeVector(governorFileVector, "governor");
                initialized = true;
                return true;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] CPU already initialized"
                                << Color::reset() << std::endl;
                }
                return false;
        }
}

bool Cpu::hasPstate() const
{
        return pstate;
}

const std::string Cpu::getDriver() const
{
        const std::string line = sysfs.read("cpu0/cpufreq/scaling_driver");
        if (line != BAD_READ) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Check the scaling_driver: '"
                                  << line << "'" << std::endl;
                }
                return line;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to get scaling_driver"
                                << Color::reset() << std::endl;
                }
                return BAD_READ;
        }
}

const std::vector<std::string> Cpu::getRealtimeFrequencies() const
{
        const char *cmd = "grep MHz /proc/cpuinfo | cut -c12-";
        const std::vector<std::string> result = sysfs.readPipe(cmd, number);
        if (!result.empty()) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Found realtime frequency vector"
                                  << std::endl;
                }
                return result;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to get realtime frequencies"
                                << Color::reset() << std::endl;
                }
                return BAD_VECTOR;
        }
}

/*
 * Given a sysfs power_supply path, decide whether the power_supply is
 * the 'Mains' type adapter and if so, return whether or not the adapter
 * is online.
 */
unsigned int Cpu::getPowerSupply(const std::string &fullPath) const
{
        std::ostringstream oss;
        oss << fullPath << "/type";
        const std::string typePath = oss.str();
        const char *const type = typePath.c_str();
        if (Log::isDebug()) {
                std::cout << "[Debug] getting power supply from path: '"
                          << typePath << "'" << std::endl;
        }
        if (access(type, F_OK) != BAD_FILE_ACCESS) {
                const std::string powerType = sysfs.read(fullPath, "type");
                if (Log::isDebug()) {
                        std::cout << "[Debug] power type: '"
                                << powerType << "'" << std::endl;
                }
                if (powerType.compare("Mains") == 0) {
                        const int status = stringToNumber(
                                        sysfs.read(fullPath, "online"));
                        if (Log::isDebug()) {
                                std::cout << "[Debug] power status: '"
                                        << status << "'" << std::endl;
                        }
                        if (status == BAD_NUMBER) {
                                return POWER_SOURCE_NONE;
                        } else if (status == 1) {
                                return POWER_SOURCE_AC;
                        } else {
                                return POWER_SOURCE_BAT;
                        }
                }
        }
        return POWER_SOURCE_NONE;
}

}

