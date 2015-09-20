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

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <getopt.h>
#include <unistd.h>

#include "psfreq_color.h"
#include "psfreq_cpu.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_output.h"
#include "psfreq_util.h"
#include "psfreq_values.h"

static bool setCpuValues(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues);
static void setSaneValues(const psfreq::Cpu &cpu,
                const int cpuInfoMin,
                const int cpuInfoMax);
static bool isSystemInsane(const int cpuInfoMin, const int cpuInfoMax,
                const int cpuMinPstate, const int cpuMaxPstate,
                const std::string &cpuGovernor);
static int sanitizeVal(const int requested, const int cpuPstate,
                const int cpuInfoMin, const int cpuInfoMax);
static void setCpuTurbo(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues);
static void setCpuGovernor(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues,
                const std::string cpuGovernor);
static void setCpuFrequencies(const psfreq::Cpu &cpu, const int newMax,
                const int newMin);
static void sleepCpu(const psfreq::Values &cpuValues);
static unsigned int initializeCpu(psfreq::Cpu &cpu);
static unsigned int nullAction(void);
static unsigned int getAction(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues);
static unsigned int setActionRoot(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues);
static unsigned int setActionOther();

static unsigned int setActionRoot(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues)
{
        if (!cpuValues.isInitialized()) {
                if (!psfreq::Log::isAllQuiet()) {
                        std::cerr << psfreq::Color::boldRed()
                                << "[Error] No Requests."
                                << psfreq::Color::reset()
                                << std::endl;
                }
                return EXIT_FAILURE;
        }
        if (!setCpuValues(cpu, cpuValues)) {
                if (!psfreq::Log::isAllQuiet()) {
                        std::cerr
                                << psfreq::Color::boldRed()
                                << "[Error] Environment was"
                                << " not sane. Could"
                                << " not set any"
                                << " values"
                                << psfreq::Color::reset()
                                << std::endl;
                }
                return EXIT_FAILURE;
        }
        psfreq::printCpuValues(cpu);
        return EXIT_SUCCESS;
}

static unsigned int setActionOther()
{
        if (!psfreq::Log::isAllQuiet()) {
                std::cerr << psfreq::Color::boldRed()
                        << "[Error] Insufficient Permissions."
                        << psfreq::Color::reset() << std::endl;
        }
        return EXIT_FAILURE;
}

static unsigned int getAction(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues)
{
        if (cpuValues.getRequested() == psfreq::Values::REQUESTED_CURRENT) {
                psfreq::printCpuValues(cpu);
        } else {
                psfreq::printRealtimeFrequency(cpu);
        }
        return EXIT_SUCCESS;

}

static unsigned int nullAction()
{
        psfreq::printGPL();
        psfreq::printHelp();
        return EXIT_SUCCESS;
}

/*
 * Initialize the cpu so that it may now act on sysfs values.
 */
static unsigned int initializeCpu(psfreq::Cpu &cpu)
{
        if (!cpu.init()) {
                if (!psfreq::Log::isAllQuiet()) {
                        std::cerr << psfreq::Color::boldRed()
                                  << "[Error] Could not init CPU"
                                  << psfreq::Color::reset() << std::endl;
                }
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}

static void sleepCpu(const psfreq::Values &cpuValues)
{
        if (cpuValues.shouldSleep()) {
                if (psfreq::Log::isDebug()) {
                        std::cout << "[Debug] Sleep for two seconds"
                                << std::endl;
                }
                sleep(2);
        }
}

static void setCpuFrequencies(const psfreq::Cpu &cpu, const int newMax,
                const int newMin)
{
        /*
         * If the new maximum frequency that is requested
         * is less than the current minimum, we must
         * modify the minimum first before we can actually
         * change the max frequency
         */
        if (psfreq::Log::isDebug()) {
                std::cout << "[Debug] Current min is lower "
                        << "than the new max, can safely "
                        << "adjust the new max"
                        << std::endl;
        }
        cpu.setScalingMax(newMax);
        cpu.setScalingMin(newMin);
}


static void setCpuGovernor(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues,
                const std::string cpuGovernor)
{
        /*
         * Set the software CPU governor
         */
        if (psfreq::Log::isDebug()) {
                std::cout << "[Debug] Set the cpu governor"
                                << std::endl;
        }
        const std::string requestedGovernor = cpuValues.getGovernor();
        const std::string newGovernor =
                        (requestedGovernor != psfreq::UNINITIALIZED_STR
                        ? requestedGovernor
                        : cpuGovernor);
        cpu.setGovernor(newGovernor);
}

static void setCpuTurbo(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues)
{
        /*
         * If the system supports a Turbo Boost
         * type functionality, attempt to set it
         * as well.
         */
        const int cpuTurbo = cpu.getTurboBoost();
        if (cpuTurbo != psfreq::Cpu::TURBO_BOOST_INSANE) {
                if (psfreq::Log::isDebug()) {
                        std::cout << "[Debug] Turbo is available"
                                << std::endl;
                }
                const int turbo = cpuValues.getTurbo();
                int newTurbo = (turbo != -1 ? turbo : cpuTurbo);
                newTurbo = psfreq::boundValue(newTurbo, 0, 1);
                cpu.setTurboBoost(newTurbo);
        }
}

static int sanitizeVal(const int requested, const int cpuPstate,
                const int cpuInfoMin, const int cpuInfoMax)
{
        /*
         * Sanitize the CPU frequency so that
         * it can safely be set.
         */
        int newVal = (requested >= 0
                        ? requested
                        : cpuPstate);
        newVal = psfreq::boundValue(newVal, cpuInfoMin, cpuInfoMax);
        return newVal;
}

static bool isSystemInsane(const int cpuInfoMin, const int cpuInfoMax,
                const int cpuMinPstate, const int cpuMaxPstate,
                const std::string &cpuGovernor)
{
        /*
         * Check for sane system, if any of these values are not stable,
         * exit immediately, this system is not supported by
         * pstate-frequency
         */
        return (cpuInfoMin == psfreq::Cpu::INFO_FREQUENCY_INSANE
                        || cpuInfoMax == psfreq::Cpu::INFO_FREQUENCY_INSANE
                        || cpuMinPstate == psfreq::Cpu::PSTATE_VALUE_INSANE
                        || cpuMaxPstate == psfreq::Cpu::PSTATE_VALUE_INSANE
                        || cpuGovernor == psfreq::Cpu::GOVERNOR_INSANE)
                ? false
                : true;

}

static void setSaneValues(const psfreq::Cpu &cpu,
                const int cpuInfoMin,
                const int cpuInfoMax)
{
        /* If intel_pstate only source the max_perf_pct and
                * min_perf_pct files when they change, then we need to force
                * a change somehow. Though ugly, setting the CPU first to a
                * powersave state and then a performance state should force
                * the driver to re-read the CPU in almost all situations
                */
        if (psfreq::Log::isDebug()) {
                std::cout << "[Debug] Setting sane min/max values"
                        << std::endl;
        }
        int saneMin = 0;
        int saneMax = 100;
        int saneTurbo = cpu.getTurboBoost() == psfreq::Values::PSTATE_TURBO
                ? psfreq::Values::PSTATE_NO_TURBO
                : psfreq::Values::PSTATE_TURBO;
        saneMin = psfreq::boundValue(saneMin, cpuInfoMin,
                        cpuInfoMax - 1);
        saneMax = psfreq::boundValue(saneMax, cpuInfoMin + 1,
                        cpuInfoMax);
        saneMin = (saneMin >= saneMax
                        ? saneMax - 1
                        : saneMin);
        if (psfreq::Log::isDebug()) {
                std::cout << "[Debug] Sane Max: " << saneMax
                                << " Sane Min: " << saneMin
                                << " Sane Turbo: " << saneTurbo
                                << std::endl;
        }
        cpu.setScalingMin(saneMin);
        cpu.setScalingMax(saneMax);
        cpu.setTurboBoost(saneTurbo);
}

/*
 * Retrieves the values requested by the user and makes sure that they are
 * sane, or sanitizes them. Then attempts to set the values requested by the
 * user.
 */
static bool setCpuValues(const psfreq::Cpu &cpu,
                const psfreq::Values &cpuValues)
{
        /*
         * Retrieve the system constant values including the
         * full range of available CPU frequencies
         */
        const int cpuInfoMin = cpu.getInfoMinValue();
        const int cpuInfoMax =  cpu.getInfoMaxValue();
        const int cpuMinPstate = cpu.getMinValue();
        const int cpuMaxPstate = cpu.getMaxValue();
        const std::string cpuGovernor = cpu.getGovernor();

        if (!isSystemInsane(cpuInfoMin, cpuInfoMax, cpuMinPstate,
                                cpuMaxPstate, cpuGovernor)) {
                if (!psfreq::Log::isAllQuiet()) {
                        std::cerr << psfreq::Color::boldRed()
                                  << "[Error] System is insane"
                                  << psfreq::Color::reset() << std::endl;
                }
                return false;
        } else {
                if (psfreq::Log::isDebug()) {
                        std::cout << "[Debug] bound the CPU min" << std::endl;
                }
                int newMin = sanitizeVal(cpuValues.getMin(),
                                cpuMinPstate, cpuInfoMin, cpuInfoMax - 1);
                if (psfreq::Log::isDebug()) {
                        std::cout << "[Debug] bound the CPU max" << std::endl;
                }
                int newMax = sanitizeVal(cpuValues.getMax(),
                                cpuMaxPstate, cpuInfoMin + 1, cpuInfoMax);
                /*
                 * Make sure that the minimum frequency is not going
                 * to be greater than or equal to the maximum frequency
                 */
                newMin = (newMin >= newMax
                                ? newMax - 1
                                : newMin);

                setSaneValues(cpu, cpuInfoMin, cpuInfoMax);
                sleepCpu(cpuValues);
                setCpuFrequencies(cpu, newMax, newMin);
                setCpuTurbo(cpu, cpuValues);
                setCpuGovernor(cpu, cpuValues, cpuGovernor);
                return true;
        }
}


/*
 * The main program function.
 */
int main(int argc, char** argv)
{
        /*
         * The cpu and cpuValues are defined here, though by default they
         * do not actually have the functionality to modify or access much.
         * The cpu must be initialized at a later period after option parsing
         * by calling cpu.init()
         */
        psfreq::Cpu cpu;
        psfreq::Values cpuValues = psfreq::Values(cpu);

        const char *const shortOptions = ":SGHVcrdaqp:m:n:t:g:";
        const struct option longOptions[] = {
                {"help",          no_argument,        NULL,           'H'},
                {"version",       no_argument,        NULL,           'V'},
                {"quiet",         no_argument,        NULL,           'q'},
                {"all-quiet",     no_argument,        NULL,           'a'},
                {"no-sleep",      no_argument,        NULL,           '2'},
                {"debug",         no_argument,        NULL,           'd'},
                {"get",           no_argument,        NULL,           'G'},
                {"set",           no_argument,        NULL,           'S'},
                {"current",       no_argument,        NULL,           'c'},
                {"real",          no_argument,        NULL,           'r'},
                {"color",         no_argument,        NULL,           '1'},
                {"plan",          required_argument,  NULL,           'p'},
                {"governor",      required_argument,  NULL,           'g'},
                {"max",           required_argument,  NULL,           'm'},
                {"min",           required_argument,  NULL,           'n'},
                {"turbo",         required_argument,  NULL,           't'},
                {0,               0,                  0,              0}
                };

        if (initializeCpu(cpu) == EXIT_FAILURE) {
                return EXIT_FAILURE;
        }

        const unsigned int opts = psfreq::parseOptions(argc, argv, cpu,
                        cpuValues, shortOptions, longOptions);
        if (opts != psfreq::PARSE_EXIT_NORMAL) {
                return ((opts == psfreq::PARSE_EXIT_GOOD)
                        ? EXIT_SUCCESS
                        : EXIT_FAILURE);
        }

        if (cpuValues.hasPlan() && !cpuValues.runPlan()) {
                return EXIT_FAILURE;
        }

        if (cpuValues.isActionNull()) {
                return nullAction();
        } else if (cpuValues.isActionGet()) {
                return getAction(cpu, cpuValues);
        } else {
                /*
                 * User must have root privilages to set
                 * values here.
                 */
                if (geteuid() == psfreq::UID_ROOT) {
                        return setActionRoot(cpu, cpuValues);
                } else {
                        return setActionOther();
                }
        }

        return EXIT_SUCCESS;
}
