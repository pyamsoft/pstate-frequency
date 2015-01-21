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

#include <getopt.h>
#include <unistd.h>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_cpuvalues.h"
#include "include/psfreq_util.h"

void setCpuValues(const psfreq::cpu &cpu, const psfreq::cpuValues &cpuValues);
void printCpuValues(const psfreq::cpu& cpu);
void printGPL();
void printVersion();
void printHelp();
int handleOptionResult(psfreq::cpuValues &cpuValues, const int result);
int planFromOptArg(char *const arg);

void setCpuValues(const psfreq::cpu &cpu, const psfreq::cpuValues &cpuValues)
{
	const int max = psfreq::boundValue(cpuValues.getMax(), cpu.getInfoMinValue() + 1, cpu.getInfoMaxValue());
	const int min = psfreq::boundValue(cpuValues.getMin(), cpu.getInfoMinValue(), cpu.getInfoMaxValue() - 1);
	const int turbo = psfreq::boundValue(cpuValues.getTurbo(), 0, 1);
	const int newTurbo = (turbo != -1 ? turbo : cpu.getTurboBoost());
	const int newMin = (min >= 0 ? min : cpu.getMinPState());
	int newMax = (max >= 0 ? max : cpu.getMaxPState());
	newMax = (newMax > newMin ? newMax : newMin + 1);

	cpu.setSaneDefaults();
	cpu.setScalingMax(newMax);
	cpu.setScalingMin(newMin);
	cpu.setTurboBoost(newTurbo);
}

int planFromOptArg(char *const arg)
{
	const std::string convertedArg = arg;
	int plan;
	if (convertedArg.compare("1") == 0 || psfreq::stringStartsWith("powersave", convertedArg)) {
		plan = 1;
	} else if (convertedArg.compare("2") == 0 || psfreq::stringStartsWith("performance", convertedArg)) {
		plan = 2;
	} else if (convertedArg.compare("3") == 0 || psfreq::stringStartsWith("max-performance", convertedArg)) {
		plan = 3;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (convertedArg.compare("0") == 0 || psfreq::stringStartsWith("auto", convertedArg)) {
		plan = 0;
#endif
#endif
	} else {
		std::cerr << psfreq::PSFREQ_COLOR_BOLD_RED << "Invalid plan: " << convertedArg << psfreq::PSFREQ_COLOR_OFF << std::endl;
		exit(EXIT_FAILURE);
	}
	return plan;
}

void printGPL()
{
	std::cout << "pstate-frequency comes with ABSOLUTELY NO WARRANTY."
		<< std::endl
		<< "This is free software, and you are welcome to redistribute it"
		<< std::endl << "under certain conditions." << std::endl
		<< "Please see the README for details."
		<< psfreq::PSFREQ_COLOR_OFF << std::endl << std::endl;
}

void printVersion()
{
	std::cout << std::endl;
#ifdef VERSION
	std::cout << psfreq::PSFREQ_COLOR_BOLD_BLUE << "pstate-frequency  "
		<< psfreq::PSFREQ_COLOR_BOLD_MAGENTA << VERSION << psfreq::PSFREQ_COLOR_OFF
		<< std::endl;
#endif
}

void printCpuValues(const psfreq::cpu& cpu)
{
	printVersion();
	std::cout << psfreq::PSFREQ_COLOR_BOLD_WHITE
		<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN << "CPU_DRIVER     -> "
		<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getDriver() << std::endl;
	std::cout << psfreq::PSFREQ_COLOR_BOLD_WHITE
		<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN << "CPU_GOVERNOR   -> "
		<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getGovernor() << std::endl;
	std::cout << psfreq::PSFREQ_COLOR_BOLD_WHITE
		<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN << "NO_TURBO       -> "
		<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getTurboBoost() << " : "
		<< (cpu.getTurboBoost() == 1 ? "OFF" : "ON") << std::endl;
	std::cout << psfreq::PSFREQ_COLOR_BOLD_WHITE
		<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN << "CPU_MIN        -> "
		<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getMinPState() << "% : "
		<< static_cast<int>(cpu.getScalingMinFrequency()) << "KHz" << std::endl;
	std::cout << psfreq::PSFREQ_COLOR_BOLD_WHITE
		<< "    pstate::" << psfreq::PSFREQ_COLOR_BOLD_GREEN << "CPU_MAX        -> "
		<< psfreq::PSFREQ_COLOR_BOLD_CYAN << cpu.getMaxPState() << "% : "
		<< static_cast<int>(cpu.getScalingMaxFrequency()) << "KHz" << std::endl;
	std::cout << psfreq::PSFREQ_COLOR_OFF << std::endl;
}

void printHelp()
{
        std::cout << "usage:"
		<< "pstate-frequency [action] [option]"
		<< std::endl
		<< "    actions:" << std::endl
		<< "        unprivilaged:" << std::endl
		<< "            -h | --help     Display this help and exit" << std::endl
		<< "            -v | --version  Display application version and exit" << std::endl
		<< "            -d | --debug    Print debugging messages to stdout" << std::endl
		<< "            -q | --quiet    Supress all output" << std::endl
		<< "            -g | --get      Access current CPU values" << std::endl
		<< std::endl
		<< "        privilaged:" << std::endl
		<< "            -s | --set      Modify current CPU values" << std::endl
		<< std::endl
		<< "    options:" << std::endl
		<< "        unprivilaged:" << std::endl
		<< "            -i | --io       Get the IO Scheduler for the given device" << std::endl
		<< std::endl
		<< "        privilaged: "<< std::endl
		<< "            -p | --plan     Set a predefined power plan" << std::endl
		<< "            -m | --max      Modify current CPU max frequency" << std::endl
		<< "            -i | --io       Set the IO Scheduler for the given device" << std::endl
		<< "            -o | --gov      Set the cpufreq governor" << std::endl
		<< "            -n | --min      Modify current CPU min frequency" << std::endl
		<< "            -t | --turbo    Modify curent CPU turbo boost state" << std::endl;
}

int handleOptionResult(psfreq::cpuValues &cpuValues, const int result)
{
	switch(result) {
	case 0:
                return 0;
        case 'h':
		printGPL();
		printHelp();
		std::cout << std::endl;
                return -1;
        case 'v':
		printGPL();
		printVersion();
		std::cout << std::endl;
                return -1;
        case 's':
		cpuValues.setAction(1);
                return 0;
        case 'd':
                return 0;
	case 'q':
		return 0;
        case 'g':
		cpuValues.setAction(0);
                return 0;
        case 'p':
		cpuValues.setPlan(planFromOptArg(optarg));
                return 0;
        case 'm':
		cpuValues.setMax(psfreq::stringToNumber(optarg));
                return 0;
	case 'i':
		return 0;
	case 'o':
		return 0;
        case 'n':
		cpuValues.setMin(psfreq::stringToNumber(optarg));
		return 0;
        case 't':
		cpuValues.setTurbo(psfreq::stringToNumber(optarg));
		return 0;
	}
	return EXIT_FAILURE;
}

int main(int argc, char** argv)
{
	psfreq::cpu cpu;
	psfreq::cpuValues cpuValues;

	int finalOptionResult = 0;
	int optionResult = 0;
	const char *const shortOptions = "hvsdgqp:m:n:t:i:o:";
	struct option longOptions[] = {
                {"help",          no_argument,        NULL,           'h'},
                {"version",       no_argument,        NULL,           'v'},
                {"get",           no_argument,        NULL,           'g'},
                {"set",           no_argument,        NULL,           's'},
                {"quiet",         no_argument,        NULL,           'q'},
                {"debug",         no_argument,        NULL,           'd'},
                {"plan",          required_argument,  NULL,           'p'},
                {"io",            required_argument,  NULL,           'i'},
                {"gov",           required_argument,  NULL,           'o'},
		{"max",		  required_argument,  NULL,           'm'},
		{"min",		  required_argument,  NULL,           'n'},
		{"turbo",	  required_argument,  NULL,           't'},
		{0,		  0,                  0,              0}
                };

	while (true) {
                int index = 0;
                optionResult = getopt_long(argc, argv, shortOptions, longOptions, &index);
                if (optionResult == -1) {
                        break;
                } else {
			finalOptionResult = handleOptionResult(cpuValues, optionResult);
                        if (finalOptionResult == -1) {
                                return 0;
                        } else if (finalOptionResult == EXIT_FAILURE) {
                                return EXIT_FAILURE;
                        }
                }
        }


	if (cpuValues.isActionNull()) {
		printGPL();
		printHelp();
		std::cout << std::endl;
		return 0;
	} else if (cpuValues.isActionGet()) {
		printCpuValues(cpu);
	} else {
		if (geteuid() == 0) {
			setCpuValues(cpu, cpuValues);
			printCpuValues(cpu);
		} else {
			std::cerr << psfreq::PSFREQ_COLOR_BOLD_RED << "Root privilages required"
				<< psfreq::PSFREQ_COLOR_OFF << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
