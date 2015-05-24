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

#include <sstream>

#include <getopt.h>

#include "include/psfreq_color.h"
#include "include/psfreq_input.h"
#include "include/psfreq_log.h"
#include "include/psfreq_output.h"
#include "include/psfreq_util.h"


namespace psfreq {

static Pair handleOptionResult(const psfreq::Cpu &cpu,
		psfreq::Values &cpuValues, const int result);
static int planFromOptArg(char *const arg);
static const std::string governorFromOptArg(char *const arg,
		const std::vector<std::string> &availableGovernors);
static int turboFromOptArg(const psfreq::Cpu &cpu, char *const arg);
static int maxFromOptArg(char *const arg);
static int minFromOptArg(char *const arg);

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
		max = psfreq::stringToNumber(convertedArg);
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
		min = psfreq::stringToNumber(convertedArg);
	}
	return min;
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
 * Given the return value from the getopt_long function as parameter 'result'
 * decide how to handle the option that was entered by the user.
 */
static Pair handleOptionResult(const psfreq::Cpu &cpu,
		psfreq::Values &cpuValues, const int result)
{
	std::ostringstream err;
	err << "[Error] ";
	switch(result) {
	case 0:
                return Pair(PARSE_EXIT_NORMAL);
        case 'H':
		printGPL();
		printHelp();
                return Pair(PARSE_EXIT_GOOD);
        case 'c':
		/*
		 * The --current option is only valid when using
		 * pstate-frequency to get CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionSet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not GET");
		} else {
			cpuValues.setRequested(psfreq::
					Values::REQUESTED_CURRENT);
			return Pair(PARSE_EXIT_NORMAL);
		}
        case 'r':
		/*
		 * The --real option is only valid when using
		 * pstate-frequency to get CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionSet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not GET");
		} else {
			cpuValues.setRequested(psfreq::Values::REQUESTED_REAL);
			return Pair(PARSE_EXIT_NORMAL);
		}
	case 'd':
		psfreq::Log::setDebug();
		return Pair(PARSE_EXIT_NORMAL);
	case 'a':
		psfreq::Log::setAllQuiet();
		return Pair(PARSE_EXIT_NORMAL);
	case 'q':
		psfreq::Log::setQuiet();
		return Pair(PARSE_EXIT_NORMAL);
        case 'V':
		printGPL();
		printVersion();
		return Pair(PARSE_EXIT_GOOD);
        case 'S':
		cpuValues.setAction(psfreq::Values::ACTION_SET);
		return Pair(PARSE_EXIT_NORMAL);
        case 'G':
		cpuValues.setAction(psfreq::Values::ACTION_GET);
		return Pair(PARSE_EXIT_NORMAL);
        case 'p':
		/*
		 * The --plan option is only valid when using pstate-frequency
		 * to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not SET");
		} else {
			if (cpuValues.setPlan(planFromOptArg(optarg))) {
				return Pair(PARSE_EXIT_NORMAL);
			} else {
				err << "Invalid power plan specified: "
				    << optarg;
				return Pair(PARSE_EXIT_BAD, err.str());
			}
		}
        case 'm':
		/*
		 * The --max option is only valid when using pstate-frequency
		 * to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not SET");
		} else {
			if (cpuValues.setMax(maxFromOptArg(optarg))) {
				return Pair(PARSE_EXIT_NORMAL);
			} else {
				err << "Invalid max specified: "
				    << optarg;
				return Pair(PARSE_EXIT_BAD, err.str());
			}
		}
	case 'g':
		/*
		 * The --governor option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not SET");
		} else {
			if (cpuValues.setGovernor(governorFromOptArg(optarg,
					cpu.getAvailableGovernors()))) {
				return Pair(PARSE_EXIT_NORMAL);
			} else {
				err << "Invalid governor specified: "
				    << optarg;
				return Pair(PARSE_EXIT_BAD, err.str());
			}
		}
        case 'n':
		/*
		 * The --min option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not SET");
		} else {
			if (cpuValues.setMin(minFromOptArg(optarg))) {
				return Pair(PARSE_EXIT_NORMAL);
			} else {
				err << "Invalid min specified: "
				    << optarg;
				return Pair(PARSE_EXIT_BAD, err.str());
			}
		}
        case 't':
		/*
		 * The --turbo option is only valid when using
		 * pstate-frequency to set CPU values
		 */
		if (cpuValues.isActionNull() || cpuValues.isActionGet()) {
			return Pair(PARSE_EXIT_BAD, "Action is not SET");
		} else {
			if (cpuValues.setTurbo(turboFromOptArg(cpu, optarg))) {
				return Pair(PARSE_EXIT_NORMAL);
			} else {
				err << "Invalid turbo specified: "
				    << optarg;
				return Pair(PARSE_EXIT_BAD, err.str());
			}
		}
        case '1':
		psfreq::Color::setEnabled();
		return Pair(PARSE_EXIT_NORMAL);
	case ':':
		return Pair(PARSE_EXIT_BAD, "Missing argument");
	case '?':
		return Pair(PARSE_EXIT_BAD, "Unknown option");
	}
	return Pair(PARSE_EXIT_BAD, "End of options reached");
}

/*
 * As long as command line options exist, loop over the input and
 * run the getopt_long function to figure out the option requested.
 */
Pair parseOptions(const int argc, char **const argv,
		const psfreq::Cpu &cpu, psfreq::Values &cpuValues,
		const char *const shortOptions,
		const struct option longOptions[]) {
	while (true) {
                const int opt = getopt_long(argc, argv, shortOptions,
				longOptions, NULL);
                if (opt == -1) {
                        break;
                } else {
			const Pair result = handleOptionResult(cpu, cpuValues,
					opt);
			if (result.code != PARSE_EXIT_NORMAL) {
				return result;
			}
                }
        }
	return Pair(PARSE_EXIT_NORMAL);
}
}
