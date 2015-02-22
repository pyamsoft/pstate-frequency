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

#include <iostream>
#include <sstream>

#include <getopt.h>
#include <unistd.h>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_log.h"
#include "include/psfreq_util.h"
#include "include/psfreq_values.h"

bool setCpuValues(const psfreq::cpu &cpu, const psfreq::values &cpuValues);
void printCpuValues(const psfreq::cpu& cpu);
void printRealtimeFrequency(const psfreq::cpu& cpu);
void printGPL();
void printVersion();
void printHelp();
int handleOptionResult(const psfreq::cpu &cpu, psfreq::values &cpuValues,
		const int result);
int planFromOptArg(char *const arg);
const std::string governorFromOptArg(char *const arg,
		const std::vector<std::string> &availableGovernors);

bool setCpuValues(const psfreq::cpu &cpu, const psfreq::values &cpuValues)
{
	const int cpuInfoMin = cpu.getInfoMinValue();
	const int cpuInfoMax =  cpu.getInfoMaxValue();
	const int cpuMinPstate = cpu.getMinPState();
	const int cpuMaxPstate = cpu.getMaxPState();
	const std::string cpuGovernor = cpu.getGovernor();
	if (cpuInfoMin == 1 || cpuInfoMax == 1
			|| cpuMinPstate == 0 || cpuMaxPstate == 0
			|| cpuGovernor == std::string()) {
		return false;
	} else {
		const int requestedMin = cpuValues.getMin();
		int newMin = (requestedMin >= 0
				? requestedMin
				: cpuMinPstate);
		newMin = psfreq::boundValue(newMin, cpuInfoMin,
				cpuInfoMax - 1);

		const int requestedMax = cpuValues.getMax();
		int newMax = (requestedMax >= 0
				? requestedMax
				: cpuMaxPstate);
		newMax = psfreq::boundValue(newMax, cpuInfoMin + 1,
				cpuInfoMax);
		newMin = (newMin > newMax
				? newMax - 1
				: newMin);
		newMax = (newMax > newMin
				? newMax
				: newMin + 1);


		if (cpuMinPstate > newMax) {
			cpu.setScalingMin(newMin);
			cpu.setScalingMax(newMax);
		} else {
			cpu.setScalingMax(newMax);
			cpu.setScalingMin(newMin);
		}
		const int cpuTurbo = cpu.getTurboBoost();
		if (cpuTurbo != -2) {
			const int turbo = cpuValues.getTurbo();
			int newTurbo = (turbo != -1 ? turbo : cpuTurbo);
			newTurbo = psfreq::boundValue(newTurbo, 0, 1);
			cpu.setTurboBoost(newTurbo);
		}

		const std::string requestedGovernor = cpuValues.getGovernor();
		const std::string newGovernor =
				(requestedGovernor != std::string()
				? requestedGovernor
				: cpuGovernor);
		cpu.setGovernor(newGovernor);
		return true;
	}
}

int planFromOptArg(char *const arg)
{
	const std::string convertedArg(arg);
	int plan;
	if (convertedArg.compare("1") == 0
			|| psfreq::stringStartsWith("powersave",
				convertedArg)) {
		plan = 1;
	} else if (convertedArg.compare("2") == 0
			|| psfreq::stringStartsWith("performance",
			convertedArg)) {
		plan = 2;
	} else if (convertedArg.compare("3") == 0
			|| psfreq::stringStartsWith("max-performance",
				convertedArg)) {
		plan = 3;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (convertedArg.compare("0") == 0
		|| psfreq::stringStartsWith("auto", convertedArg)) {
		plan = 0;
#endif
#endif
	} else {
		std::cerr << "Bad Plan." << std::endl;
		return 4;
	}
	return plan;
}

void printRealtimeFrequency(const psfreq::cpu& cpu)
{
		printVersion();
		const std::vector<std::string> frequencies =
				cpu.getRealtimeFrequencies();
		if (!frequencies.empty()) {
			std::ostringstream oss;
			for (unsigned int i = 0; i < cpu.getNumber(); ++i) {
				std::string freq = frequencies[i];
				oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
					<< "    pstate::"
					<< psfreq::PSFREQ_COLOR_BOLD_GREEN
					<< "CPU["
					<< psfreq::PSFREQ_COLOR_BOLD_MAGENTA
					<< i << psfreq::PSFREQ_COLOR_BOLD_GREEN
					<< "]  -> "
					<< psfreq::PSFREQ_COLOR_BOLD_CYAN
					<< freq.substr(0, freq.size() - 1)
					<< "MHz" << psfreq::PSFREQ_COLOR_OFF
					<< std::endl;
			}
			std::cout << oss.str();
		}
}

const std::string governorFromOptArg(char *const arg,
		const std::vector<std::string> &availableGovernors)
{
	const std::string convertedArg(arg);
	std::ostringstream gov;
	std::string governor;
	for (unsigned int i = 0; i < availableGovernors.size(); ++i) {
		if (psfreq::stringStartsWith(availableGovernors[i],
				convertedArg)) {
			governor = availableGovernors[i];
			break;
		}
	}
	if (governor == std::string()) {
		std::cerr << "Bad Governor." << std::endl;
		return std::string();
	}
	return governor;
}

void printGPL()
{
		std::ostringstream oss;
		oss << "pstate-frequency comes with ABSOLUTELY NO WARRANTY."
			<< std::endl
			<< "This is free software, and you are welcome "
			<< "to redistribute it"
			<< std::endl << "under certain conditions."
			<< std::endl
			<< "Please see the README for details."
			<< psfreq::PSFREQ_COLOR_OFF << std::endl << std::endl;
		std::cout << oss.str();
}

void printVersion()
{
		std::ostringstream oss;
		oss << std::endl;
#ifdef VERSION
		oss << psfreq::PSFREQ_COLOR_BOLD_BLUE << "pstate-frequency  "
			<< psfreq::PSFREQ_COLOR_BOLD_MAGENTA << VERSION
			<< psfreq::PSFREQ_COLOR_OFF
			<< std::endl;
#endif
		std::cout << oss.str();
}

void printCpuValues(const psfreq::cpu& cpu)
{
		printVersion();
		std::ostringstream oss;
		oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
			<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN
			<< "CPU_DRIVER     -> "
			<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getDriver()
			<< std::endl;
		oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
			<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN
			<< "CPU_GOVERNOR   -> "
			<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getGovernor()
			<< std::endl;
		const int turbo = cpu.getTurboBoost();
		oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
			<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN
			<< (cpu.hasPstate() ? "NO_TURBO       -> "
					: "TURBO_BOOST    -> ")
			<< psfreq::PSFREQ_COLOR_BOLD_CYAN << turbo << " : "
			<< (cpu.hasPstate() ? (turbo == 1 ? "OFF" : "ON")
					: (turbo == 1 ? "ON" : "OFF"))
			<< std::endl;
		oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
			<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN
			<< "CPU_MIN        -> "
			<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getMinPState()
			<< "% : "
			<< static_cast<int>(cpu.getScalingMinFrequency())
			<< "KHz" << std::endl;
		oss << psfreq::PSFREQ_COLOR_BOLD_WHITE
			<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN
			<< "CPU_MAX        -> "
			<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getMaxPState()
			<< "% : "
			<< static_cast<int>(cpu.getScalingMaxFrequency())
			<< "KHz" << std::endl;
		oss << psfreq::PSFREQ_COLOR_OFF;
		std::cout << oss.str();
}

void printHelp()
{
		std::ostringstream oss;
		oss << "usage:" << std::endl
			<< "pstate-frequency [verbose] [ACTION] [option(s)]"
			<< std::endl
			<< "verbose:" << std::endl
			<< "    unprivilaged:" << std::endl
			<< "    -d | --debug     Print debugging messages to "
			<< "stdout" << std::endl
			<< "    -q | --quiet     Supress all non-error output"
			<< std::endl
			<< "    -a | --all-quiet Supress all output"
			<< std::endl
			<< std::endl
			<< "actions:" << std::endl
			<< "    unprivilaged:" << std::endl
			<< "    -H | --help      Display this help and exit"
			<< std::endl
			<< "    -V | --version   Display application version "
			<< "and exit" << std::endl
			<< "    -G | --get       Access current CPU values"
			<< std::endl
			<< "    privilaged:" << std::endl
			<< "    -S | --set       Modify current CPU values"
			<< std::endl
			<< std::endl
			<< "options:" << std::endl
			<< "    unprivilaged:" << std::endl
			<< "    -c | --current   Display the current user set "
			<< "CPU values" << std::endl
			<< "    -r | --real      Display the real time CPU "
			<< "frequencies" << std::endl
			<< "    privilaged: "<< std::endl
			<< "    -p | --plan      Set a predefined power plan"
			<< std::endl
			<< "    -m | --max       Modify current CPU max "
			<< "frequency" << std::endl
			<< "    -g | --governor  Set the cpufreq governor"
			<< std::endl
			<< "    -n | --min       Modify current CPU min "
			<< "frequency" << std::endl
			<< "    -t | --turbo     Modify curent CPU turbo "
			<< "boost state" << std::endl;
		std::cout << oss.str();
}
int handleOptionResult(const psfreq::cpu &cpu, psfreq::values &cpuValues,
		const int result)
{
	switch(result) {
	case 0:
                return 0;
        case 'H':
		printGPL();
		printHelp();
                return -1;
        case 'c':
		cpuValues.setRequested(0);
		return 0;
        case 'r':
		cpuValues.setRequested(1);
		return 0;
        case 'V':
		printGPL();
		printVersion();
                return -1;
        case 'S':
		cpuValues.setAction(1);
                return 0;
        case 'd':
                return 0;
	case 'a':
		return 0;
	case 'q':
		return 0;
        case 'G':
		cpuValues.setAction(0);
                return 0;
        case 'p':
		if (!cpuValues.setPlan(planFromOptArg(optarg))) {
			std::cerr << "Failed to set a power plan."
			<< std::endl;
			return 1;
		}
                return 0;
        case 'm':
		cpuValues.setMax(psfreq::stringToNumber(optarg));
                return 0;
	case 'g':
		if (!cpuValues.setGovernor(governorFromOptArg(optarg,
					cpu.getAvailableGovernors()))) {
			std::cerr << "Failed to set governor." << std::endl;
			return 1;
		}
		return 0;
        case 'n':
		cpuValues.setMin(psfreq::stringToNumber(optarg));
		return 0;
        case 't':
		cpuValues.setTurbo(psfreq::stringToNumber(optarg));
		return 0;
	case ':':
		std::cerr << "Missing argument for option. "<< std::endl;
		return 1;
	case '?':
		std::cerr << "Unknown option." << std::endl;
		return 1;
	}
	return 1;
}

int main(int argc, char** argv)
{
	psfreq::cpu cpu;
	psfreq::values cpuValues = psfreq::values(cpu);
	int finalOptionResult = 0;
	int optionResult = 0;
	const char *const shortOptions = ":SGHVcrdaqp:m:n:t:g:";
	struct option longOptions[] = {
                {"help",          no_argument,        NULL,           'H'},
                {"version",       no_argument,        NULL,           'V'},
                {"get",           no_argument,        NULL,           'G'},
                {"set",           no_argument,        NULL,           'S'},
                {"current",       no_argument,        NULL,           'c'},
                {"real",          no_argument,        NULL,           'r'},
                {"quiet",         no_argument,        NULL,           'q'},
                {"all-quiet",     no_argument,        NULL,           'a'},
                {"debug",         no_argument,        NULL,           'd'},
                {"plan",          required_argument,  NULL,           'p'},
                {"governor",      required_argument,  NULL,           'g'},
		{"max",		  required_argument,  NULL,           'm'},
		{"min",		  required_argument,  NULL,           'n'},
		{"turbo",	  required_argument,  NULL,           't'},
		{0,		  0,                  0,              0}
                };
	while (true) {
                optionResult = getopt_long(argc, argv, shortOptions,
                		longOptions, NULL);
                if (optionResult == -1) {
                        break;
                } else {
			finalOptionResult = handleOptionResult(cpu, cpuValues,
					optionResult);
                        if (finalOptionResult == -1) {
                                return 0;
                        } else if (finalOptionResult == 1) {
				std::cerr << "Bad Option." << std::endl;
                                return 1;
                        }
                }
        }
	cpu.init();
	if (cpuValues.isActionNull()) {
		printGPL();
		printHelp();
		return 0;
	} else if (cpuValues.isActionGet()) {
		if (cpuValues.getRequested() == 0) {
			printCpuValues(cpu);
		} else {
			printRealtimeFrequency(cpu);
		}
	} else {
		if (geteuid() == 0) {
			if (cpuValues.isInitialized()) {
				if (!setCpuValues(cpu, cpuValues)) {
					std::cerr << "Environment was not sane."
						<< " Could not set any values"
						<< std::endl;
					return 1;
				}
				printCpuValues(cpu);
			} else {
				std::cerr << "No Requests." << std::endl;
				return 1;
			}
		} else {
			std::cerr << "Permissions Error." << std::endl;
			return 1;
		}
	}
	return 0;
}
