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
#include <iostream>
#include <sstream>

#include <getopt.h>
#include <unistd.h>

#include "include/psfreq_color.h"
#include "include/psfreq_cpu.h"
#include "include/psfreq_log.h"
#include "include/psfreq_util.h"
#include "include/psfreq_values.h"

static bool setCpuValues(const psfreq::Cpu &cpu, const psfreq::Values &cpuValues);
static void printCpuValues(const psfreq::Cpu &cpu);
static void printRealtimeFrequency(const psfreq::Cpu &cpu);
static void printGPL();
static void printVersion();
static void printHelp();
static void printPlanHelp();
static void printGovernorHelp(const std::vector<std::string> &availableGovernors);
static int handleOptionResult(const psfreq::Cpu &cpu, psfreq::Values &cpuValues,
 		const int result);
static int parseOptions(const int argc, char **const argv,
 		const psfreq::Cpu &cpu, psfreq::Values &cpuValues,
 		const char *const shortOptions,
 		const struct option longOptions[]);
static int planFromOptArg(char *const arg);
static const std::string governorFromOptArg(char *const arg,
		const std::vector<std::string> &availableGovernors);
static int turboFromOptArg(const psfreq::Cpu &cpu, char *const arg);
static int maxFromOptArg(char *const arg);
static int minFromOptArg(char *const arg);


const int PARSE_EXIT_GOOD = -1;
const int PARSE_EXIT_BAD = 1;
const int PARSE_EXIT_BAD_HANDLED = 2;
const int PARSE_EXIT_NORMAL = 0;
const int UID_ROOT = 0;
const std::string UNINITIALIZED_STR = std::string();

/*
 * Print the available power plans
 */
static void printPlanHelp()
{
	std::ostringstream oss;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	oss << psfreq::Color::boldCyan()
	    << "** Compiled with support for automatic plan switching"
	    << "via udev rules **" << psfreq::Color::reset() << std::endl;
#endif
#endif
	oss << psfreq::Color::boldWhite() << "Available Power Plans:"
	    << std::endl
	    << "(1) " << psfreq::Color::boldRed() << "powersave"
	    << psfreq::Color::boldWhite() << std::endl
	    << "(2) " << psfreq::Color::boldRed() << "powersave"
	    << psfreq::Color::boldWhite() << std::endl
	    << "(3) " << psfreq::Color::boldRed() << "powersave"
	    << psfreq::Color::boldWhite() << std::endl
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	    << "(0) " << psfreq::Color::boldRed() << "auto"
	    << psfreq::Color::boldWhite() << std::endl
#endif
#endif
	    << psfreq::Color::reset() << std::endl;
	std::cerr << oss.str();
}

/*
 * Print the available cpu governors
 */
static void printGovernorHelp(const std::vector<std::string> &availableGovernors)
{
	std::ostringstream oss;
	oss << psfreq::Color::boldWhite() << "Available CPU Governors:"
	    << std::endl;
	const unsigned int size = availableGovernors.size();
	for (unsigned int i = 0; i < size; ++i) {
		oss << psfreq::Color::boldWhite() << "(" << i << ") "
		    << psfreq::Color::boldRed() << availableGovernors.at(i)
		    << std::endl;
	}
	oss << psfreq::Color::reset() << std::endl;
	std::cerr << oss.str();
}

/*
 * Retrieves the values requested by the user and makes sure that they are
 * sane, or sanitizes them. Then attempts to set the values requested by the
 * user.
 */
static bool setCpuValues(const psfreq::Cpu &cpu, const psfreq::Values &cpuValues)
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

	/*
	 * Check for sane system, if any of these values are not stable,
	 * exit immediately, this system is not supported by
	 * pstate-frequency
	 */
	if (cpuInfoMin == psfreq::Cpu::INFO_FREQUENCY_INSANE
			|| cpuInfoMax == psfreq::Cpu::INFO_FREQUENCY_INSANE
			|| cpuMinPstate == psfreq::Cpu::PSTATE_VALUE_INSANE
			|| cpuMaxPstate == psfreq::Cpu::PSTATE_VALUE_INSANE
			|| cpuGovernor == psfreq::Cpu::GOVERNOR_INSANE) {
		return false;
	} else {
		/*
		 * Sanitize the minimum CPU frequency so that
		 * it can safely be set.
		 */
		const int requestedMin = cpuValues.getMin();
		int newMin = (requestedMin >= 0
				? requestedMin
				: cpuMinPstate);
		newMin = psfreq::boundValue(newMin, cpuInfoMin,
				cpuInfoMax - 1);

		/*
		 * Sanitize the maximum CPU frequency, including
		 * the condition that it be greater than the
		 * minimum, so that it can safely be set.
		 */
		const int requestedMax = cpuValues.getMax();
		int newMax = (requestedMax >= 0
				? requestedMax
				: cpuMaxPstate);
		newMax = psfreq::boundValue(newMax, cpuInfoMin + 1,
				cpuInfoMax);
		/*
		 * Make sure that the minimum frequency is not going
		 * to be greater than or equal to the maximum frequency
		 */
		newMin = (newMin >= newMax
				? newMax - 1
				: newMin);


		/*
		 * If the new maximum frequency that is requested
		 * is less than the current minimum, we must
		 * modify the minimum first before we can actually
		 * change the max frequency
		 */
		if (cpuMinPstate > newMax) {
			cpu.setScalingMin(newMin);
			cpu.setScalingMax(newMax);
		} else {
			cpu.setScalingMax(newMax);
			cpu.setScalingMin(newMin);
		}
		/*
		 * If the system supports a Turbo Boost
		 * type functionality, attempt to set it
		 * as well.
		 */
		const int cpuTurbo = cpu.getTurboBoost();
		if (cpuTurbo != psfreq::Cpu::TURBO_BOOST_INSANE) {
			const int turbo = cpuValues.getTurbo();
			int newTurbo = (turbo != -1 ? turbo : cpuTurbo);
			newTurbo = psfreq::boundValue(newTurbo, 0, 1);
			cpu.setTurboBoost(newTurbo);
		}

		/*
		 * Set the software CPU governor
		 */
		const std::string requestedGovernor = cpuValues.getGovernor();
		const std::string newGovernor =
				(requestedGovernor != UNINITIALIZED_STR
				? requestedGovernor
				: cpuGovernor);
		cpu.setGovernor(newGovernor);
		return true;
	}
}

/*
 * Given the user command line input of either a number or a plan name,
 * decide what the proper power plan to run should be.
 */
static int planFromOptArg(char *const arg)
{
	const std::string convertedArg(arg);
	int plan;
	if (convertedArg.compare("1") == 0
			|| psfreq::stringStartsWith("powersave",
				convertedArg)) {
		plan = psfreq::Values::POWER_PLAN_POWERSAVE;
	} else if (convertedArg.compare("2") == 0
			|| psfreq::stringStartsWith("performance",
			convertedArg)) {
		plan = psfreq::Values::POWER_PLAN_PERFORMANCE;
	} else if (convertedArg.compare("3") == 0
			|| psfreq::stringStartsWith("max-performance",
				convertedArg)) {
		plan = psfreq::Values::POWER_PLAN_MAX_PERFORMANCE;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	} else if (convertedArg.compare("0") == 0
		|| psfreq::stringStartsWith("auto", convertedArg)) {
		plan = psfreq::Values::POWER_PLAN_AUTO;
#endif
#endif
	} else {
		if (!psfreq::Log::isAllQuiet()) {
			printPlanHelp();
		}
		return psfreq::Values::POWER_PLAN_NONE;
	}
	return plan;
}

static int turboFromOptArg(const psfreq::Cpu &cpu, char *const arg)
{
	const std::string convertedArg(arg);
	int turbo;
	if (cpu.hasPstate()) {
		if (convertedArg.compare("0") == 0
				|| psfreq::stringStartsWith("on",
					convertedArg)) {
			turbo = psfreq::Values::PSTATE_TURBO;
		} else if (convertedArg.compare("1") == 0
				|| psfreq::stringStartsWith("off",
				convertedArg)) {
			turbo = psfreq::Values::PSTATE_NO_TURBO;
		} else {
			turbo = psfreq::Values::TURBO_INSANE;
		}
	} else {
		if (convertedArg.compare("0") == 0
				|| psfreq::stringStartsWith("off",
					convertedArg)) {
			turbo = psfreq::Values::CPUFREQ_NO_TURBO;
		} else if (convertedArg.compare("1") == 0
				|| psfreq::stringStartsWith("on",
				convertedArg)) {
			turbo = psfreq::Values::CPUFREQ_TURBO;
		} else {
			turbo = psfreq::Values::TURBO_INSANE;
		}
	}
	return turbo;
}

static int maxFromOptArg(char *const arg)
{
	const std::string convertedArg(arg);
	int max;
	if (convertedArg.compare("min") == 0) {
		max = 0;
	} else if (convertedArg.compare("max") == 0) {
		max = 100;
	} else {
		const int n = psfreq::stringToNumber(convertedArg);
		if (n == BAD_NUMBER) {
			max = BAD_NUMBER;
		} else {
			max = n;
		}
	}
	return max;
}

static int minFromOptArg(char *const arg)
{
	const std::string convertedArg(arg);
	int min;
	if (convertedArg.compare("min") == 0) {
		min = 0;
	} else if (convertedArg.compare("max") == 0) {
		min = 99;
	} else {
		const int n = psfreq::stringToNumber(convertedArg);
		if (n == BAD_NUMBER) {
			min = BAD_NUMBER;
		} else {
			min = n;
		}
	}
	return min;
}

/*
 * Grab the current CPU frequencies from /proc/cpuinfo
 * and pretty print them to the stdout
 */
void printRealtimeFrequency(const psfreq::Cpu &cpu)
{
	if (psfreq::Log::isOutputCapable()) {
		printVersion();
		const std::vector<std::string> frequencies =
				cpu.getRealtimeFrequencies();
		if (!frequencies.empty()) {
			std::ostringstream oss;
			for (unsigned int i = 0; i < cpu.getNumber(); ++i) {
				std::string freq = frequencies[i];
				oss << psfreq::Color::boldWhite()
					<< "    pstate::"
					<< psfreq::Color::boldGreen()
					<< "CPU["
					<< psfreq::Color::boldMagenta()
					<< i << psfreq::Color::boldGreen()
					<< "]  -> "
					<< psfreq::Color::boldCyan()
					<< freq.substr(0, freq.size() - 1)
					<< "MHz" << psfreq::Color::reset()
					<< std::endl;
			}
			std::cout << oss.str();
		}
	}
}

/*
 * Given the user input which is a governor name, and the list of currently
 * available governors, decide on the governor to set or none if the user
 * requested input is a value which does not exist on the system.
 */
static const std::string governorFromOptArg(char *const arg,
		const std::vector<std::string> &availableGovernors)
{
	const std::string convertedArg(arg);
	std::string governor;
	for (unsigned int i = 0; i < availableGovernors.size(); ++i) {
		if (psfreq::stringStartsWith(availableGovernors[i],
				convertedArg)) {
			governor = availableGovernors[i];
			break;
		}
	}
	if (governor == UNINITIALIZED_STR) {
		for (unsigned int i = 0; i < availableGovernors.size(); ++i) {
			if (convertedArg.compare(psfreq::numberToString(i))
						== 0) {
				governor = availableGovernors[i];
				break;
			}
		}
	}
	if (governor == UNINITIALIZED_STR) {
		if (!psfreq::Log::isAllQuiet()) {
			printGovernorHelp(availableGovernors);
		}
		return UNINITIALIZED_STR;
	} else {
		return governor;
	}
}

/*
 * Print the first part of the GPL v2 License.
 */
static void printGPL()
{
	if (psfreq::Log::isOutputCapable()) {
		std::ostringstream oss;
		oss << "pstate-frequency comes with ABSOLUTELY NO WARRANTY."
			<< std::endl
			<< "This is free software, and you are welcome "
			<< "to redistribute it"
			<< std::endl << "under certain conditions."
			<< std::endl
			<< "Please see the README for details."
			<< psfreq::Color::reset()
			<< std::endl << std::endl;
		std::cout << oss.str();
	}
}

/*
 * Display the current program version if it is set.
 */
static void printVersion()
{
	if (psfreq::Log::isOutputCapable()) {
		std::ostringstream oss;
		oss << std::endl;
#ifdef VERSION
		oss << psfreq::Color::boldBlue() << "pstate-frequency  "
			<< psfreq::Color::boldMagenta() << VERSION
			<< psfreq::Color::reset()
			<< std::endl;
#endif
		std::cout << oss.str();
	}
}

/*
 * Print out the current CPU settings as configured either from
 * the cpufreq sysfs files or the intel_pstate sysfs files.
 */
static void printCpuValues(const psfreq::Cpu &cpu)
{
	if (psfreq::Log::isOutputCapable()) {
		printVersion();
		std::ostringstream oss;
		oss << psfreq::Color::boldWhite()
			<< "    pstate::" << psfreq::Color::boldGreen()
			<< "CPU_DRIVER     -> "
			<< psfreq::Color::boldCyan() << cpu.getDriver()
			<< std::endl;
		oss << psfreq::Color::boldWhite()
			<< "    pstate::" << psfreq::Color::boldGreen()
			<< "CPU_GOVERNOR   -> "
			<< psfreq::Color::boldCyan() << cpu.getGovernor()
			<< std::endl;
		const int turbo = cpu.getTurboBoost();
		oss << psfreq::Color::boldWhite()
			<< "    pstate::" << psfreq::Color::boldGreen()
			<< (cpu.hasPstate() ? "NO_TURBO       -> "
					: "TURBO_BOOST    -> ")
			<< psfreq::Color::boldCyan() << turbo << " : "
			<< (cpu.hasPstate() ? (turbo == 1 ? "OFF" : "ON")
					: (turbo == 1 ? "ON" : "OFF"))
			<< std::endl;
		oss << psfreq::Color::boldWhite()
			<< "    pstate::" << psfreq::Color::boldGreen()
			<< "CPU_MIN        -> "
			<< psfreq::Color::boldCyan() << cpu.getMinValue()
			<< "% : "
			<< static_cast<int>(cpu.getScalingMinFrequency())
			<< "KHz" << std::endl;
		oss << psfreq::Color::boldWhite()
			<< "    pstate::" << psfreq::Color::boldGreen()
			<< "CPU_MAX        -> "
			<< psfreq::Color::boldCyan() << cpu.getMaxValue()
			<< "% : "
			<< static_cast<int>(cpu.getScalingMaxFrequency())
			<< "KHz" << std::endl;
		oss << psfreq::Color::reset();
		std::cout << oss.str();
	}
}

/*
 * Print the program's options and the intended methods of use.
 */
static void printHelp()
{
	if (psfreq::Log::isOutputCapable()) {
		std::ostringstream oss;
		oss << "usage:" << std::endl
			<< "pstate-frequency [verbose] [ACTION] [option(s)]"
			<< std::endl
#ifdef INCLUDE_UDEV_RULE
			<< "** Compiled with support for automatic plan "
			<< "switching via udev rules **"
			<< std::endl
#endif
			<< std::endl
			<< "verbose:" << std::endl
			<< "    unprivilaged:" << std::endl
			<< "    -d | --debug     Print debugging messages to "
			<< "stdout" << std::endl
			<< "    -q | --quiet     Supress all non-error output"
			<< std::endl
			<< "    -a | --all-quiet Supress all output"
			<< std::endl
			<< "    --color          Colorize output"
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
}

/*
 * Given the return value from the getopt_long function as parameter 'result'
 * decide how to handle the option that was entered by the user.
 */
static int handleOptionResult(const psfreq::Cpu &cpu, psfreq::Values &cpuValues,
		const int result)
{
	switch(result) {
	case 0:
                return PARSE_EXIT_NORMAL;
        case 'H':
		printGPL();
		printHelp();
                return PARSE_EXIT_GOOD;
        case 'c':
		/*
		 * The --current option is only valid when using
		 * pstate-frequency to get CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionSet()) {
			return PARSE_EXIT_BAD;
		} else {
			cpuValues.setRequested(psfreq::
					Values::REQUESTED_CURRENT);
			return PARSE_EXIT_NORMAL;
		}
        case 'r':
		/*
		 * The --real option is only valid when using
		 * pstate-frequency to get CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionSet()) {
			return PARSE_EXIT_BAD;
		} else {
			cpuValues.setRequested(psfreq::Values::REQUESTED_REAL);
			return PARSE_EXIT_NORMAL;
		}
	case 'd':
		psfreq::Log::setDebug();
		return PARSE_EXIT_NORMAL;
	case 'a':
		psfreq::Log::setAllQuiet();
		return PARSE_EXIT_NORMAL;
	case 'q':
		psfreq::Log::setQuiet();
		return PARSE_EXIT_NORMAL;
        case 'V':
		printGPL();
		printVersion();
                return PARSE_EXIT_GOOD;
        case 'S':
		cpuValues.setAction(psfreq::Values::ACTION_SET);
                return PARSE_EXIT_NORMAL;
        case 'G':
		cpuValues.setAction(psfreq::Values::ACTION_GET);
                return PARSE_EXIT_NORMAL;
        case 'p':
		/*
		 * The --plan option is only valid when using pstate-frequency
		 * to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return PARSE_EXIT_BAD;
		} else {
			return (!cpuValues.setPlan(planFromOptArg(optarg))
					? PARSE_EXIT_BAD_HANDLED
					: PARSE_EXIT_NORMAL);
		}
        case 'm':
		/*
		 * The --max option is only valid when using pstate-frequency
		 * to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return PARSE_EXIT_BAD;
		} else {
			return (cpuValues.setMax(maxFromOptArg(optarg))
					? PARSE_EXIT_NORMAL
					: PARSE_EXIT_BAD);
		}
	case 'g':
		/*
		 * The --governor option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return PARSE_EXIT_BAD;
		} else {
			return (!cpuValues.setGovernor(
					governorFromOptArg(optarg,
					cpu.getAvailableGovernors()))
					? PARSE_EXIT_BAD_HANDLED
					: PARSE_EXIT_NORMAL);
		}
        case 'n':
		/*
		 * The --min option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return PARSE_EXIT_BAD;
		} else {
			return (cpuValues.setMin(minFromOptArg(optarg))
					? PARSE_EXIT_NORMAL
					: PARSE_EXIT_BAD);
		}
        case 't':
		/*
		 * The --turbo option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return PARSE_EXIT_BAD;
		} else {
			return (cpuValues.setTurbo(
					turboFromOptArg(cpu, optarg))
					? PARSE_EXIT_NORMAL
					: PARSE_EXIT_BAD);
		}
        case '1':
		psfreq::Color::setEnabled();
		return PARSE_EXIT_NORMAL;
	case ':':
		if (!psfreq::Log::isAllQuiet()) {
			std::cerr << psfreq::Color::boldRed()
				<< "Missing argument for option. "
				<< psfreq::Color::reset() << std::endl;
		}
		return PARSE_EXIT_BAD;
	case '?':
		if (!psfreq::Log::isAllQuiet()) {
			std::cerr << psfreq::Color::boldRed()
				<< "Unknown option."
				<< psfreq::Color::reset() << std::endl;
		}
		return PARSE_EXIT_BAD;
	}
	return PARSE_EXIT_BAD;
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
                {"debug",         no_argument,        NULL,           'd'},
                {"get",           no_argument,        NULL,           'G'},
                {"set",           no_argument,        NULL,           'S'},
                {"current",       no_argument,        NULL,           'c'},
                {"real",          no_argument,        NULL,           'r'},
		{"color",	  no_argument,	      NULL,           '1'},
                {"plan",          required_argument,  NULL,           'p'},
                {"governor",      required_argument,  NULL,           'g'},
		{"max",		  required_argument,  NULL,           'm'},
		{"min",		  required_argument,  NULL,           'n'},
		{"turbo",	  required_argument,  NULL,           't'},
		{0,		  0,                  0,              0}
                };

	/*
	 * Initialize the cpu so that it may now act on sysfs values.
	 */
	if (!cpu.init()) {
		return EXIT_FAILURE;
	}

	const int parseResult = parseOptions(argc, argv, cpu, cpuValues,
			shortOptions, longOptions);
	if (parseResult != PARSE_EXIT_NORMAL) {
		if (parseResult == PARSE_EXIT_GOOD) {
			return EXIT_SUCCESS;
		} else {
			return EXIT_FAILURE;
		}
	}

	if (!cpuValues.runPlan()) {
		return EXIT_FAILURE;
	}

	if (cpuValues.isActionNull()) {
		printGPL();
		printHelp();
		return EXIT_SUCCESS;
	} else if (cpuValues.isActionGet()) {
		if (cpuValues.getRequested() ==
				psfreq::Values::REQUESTED_CURRENT) {
			printCpuValues(cpu);
		} else {
			printRealtimeFrequency(cpu);
		}
	} else {
		/*
		 * User must have root privilages to set
		 * values here.
		 */
		if (geteuid() == UID_ROOT) {
			if (!cpuValues.isInitialized()) {
				if (!psfreq::Log::isAllQuiet()) {
					std::cerr << psfreq::Color::boldRed()
						<< "No Requests."
						<< psfreq::Color::reset()
						<< std::endl;
				}
				return EXIT_FAILURE;
			}
			if (!setCpuValues(cpu, cpuValues)) {
				if (!psfreq::Log::isAllQuiet()) {
					std::cerr
						<< psfreq::Color::boldRed()
						<< "Environment was"
						<< " not sane. Could"
						<< " not set any"
						<< " values"
						<< psfreq::Color::reset()
						<< std::endl;
				}
				return EXIT_FAILURE;
			}
			printCpuValues(cpu);
		} else {
			if (!psfreq::Log::isAllQuiet()) {
				std::cerr << psfreq::Color::boldRed()
					<< "Permissions Error."
					<< psfreq::Color::reset() << std::endl;
			}
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

/*
 * As long as command line options exist, loop over the input and
 * run the getopt_long function to figure out the option requested.
 */
static int parseOptions(const int argc, char **const argv,
		const psfreq::Cpu &cpu, psfreq::Values &cpuValues,
		const char *const shortOptions,
		const struct option longOptions[]) {
	while (true) {
                const int optionResult = getopt_long(argc, argv, shortOptions,
				longOptions, NULL);
                if (optionResult == -1) {
                        break;
                } else {
			const int finalOptionResult = handleOptionResult(cpu, cpuValues,
					optionResult);
			if (finalOptionResult == PARSE_EXIT_GOOD) {
				return PARSE_EXIT_GOOD;
			} else if (finalOptionResult == PARSE_EXIT_BAD) {
				if (!psfreq::Log::isAllQuiet()) {
					std::cerr << psfreq::Color::boldRed()
						<< "Bad Option."
						<< psfreq::Color::reset()
						<< std::endl;
				}
				return PARSE_EXIT_BAD;
			} else if (finalOptionResult == PARSE_EXIT_BAD_HANDLED) {
				return PARSE_EXIT_BAD;
			}
                }
        }
	return PARSE_EXIT_NORMAL;
}
