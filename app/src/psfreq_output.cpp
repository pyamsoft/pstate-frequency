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

#include "psfreq_color.h"
#include "psfreq_log.h"
#include "psfreq_output.h"


namespace psfreq {

/*
 * Print the available power plans
 */
void printPlanHelp()
{
	std::ostringstream oss;
#ifdef INCLUDE_UDEV_RULE
#if INCLUDE_UDEV_RULE == 1
	oss << Color::boldCyan()
	    << "** Compiled with support for automatic plan switching"
	    << " via udev rules **" << Color::reset() << std::endl;
#endif
#endif
	oss << Color::boldRed() << "Error "
	    << Color::boldWhite() << "Available Power Plans:" << std::endl
	    << "(0) " << Color::boldRed() << "auto"
	    << Color::boldWhite() << std::endl
	    << "(1) " << Color::boldRed() << "powersave"
	    << Color::boldWhite() << std::endl
	    << "(2) " << Color::boldRed() << "performance"
	    << Color::boldWhite() << std::endl
	    << "(3) " << Color::boldRed() << "max-performance"
	    << Color::boldWhite() << std::endl
	    << Color::reset() << std::endl;
	std::cerr << oss.str();
}

/*
 * Print the available cpu governors
 */
void printGovernorHelp(const std::vector<std::string> &availableGovernors)
{
	std::ostringstream oss;
	oss << Color::boldRed() << "Error "
	    << Color::boldWhite() << "Available CPU Governors:"
	    << std::endl;
	const unsigned int size = availableGovernors.size();
	if (Log::isDebug()) {
		std::cout << "[Debug] Governors available: " << size
			<< std::endl;
	}
	for (unsigned int i = 0; i < size; ++i) {
		oss << Color::boldWhite() << "(" << i << ") "
		    << Color::boldRed() << availableGovernors.at(i)
		    << std::endl;
	}
	oss << Color::reset() << std::endl;
	std::cerr << oss.str();
}

/*
 * Grab the current CPU frequencies from /proc/cpuinfo
 * and pretty print them to the stdout
 */
void printRealtimeFrequency(const Cpu &cpu)
{
	if (Log::isOutputCapable()) {
		printVersion();
		const std::vector<std::string> frequencies =
			cpu.getRealtimeFrequencies();
		if (!frequencies.empty()) {
			std::ostringstream oss;
			const unsigned int size = frequencies.size();
			for (unsigned int i = 0; i < size; ++i) {
				std::string freq = frequencies[i];
				oss << Color::boldWhite()
				    << "    pstate::" << Color::boldGreen()
				    << "CPU[" << Color::boldMagenta()
				    << i << Color::boldGreen() << "]  -> "
				    << Color::boldCyan()
				    << freq.substr(0, freq.size() - 1)
				    << "MHz" << Color::reset() << std::endl;
			}
			std::cout << oss.str();
		}
	}
}

/*
 * Print the first part of the GPL v2 License.
 */
void printGPL()
{
	if (Log::isOutputCapable()) {
		std::ostringstream oss;
		oss << "pstate-frequency comes with ABSOLUTELY NO WARRANTY."
			<< std::endl
			<< "This is free software, and you are welcome "
			<< "to redistribute it"
			<< std::endl << "under certain conditions."
			<< std::endl
			<< "Please see the README for details."
			<< Color::reset()
			<< std::endl << std::endl;
		std::cout << oss.str();
	}
}

/*
 * Display the current program version if it is set.
 */
void printVersion()
{
	if (Log::isOutputCapable()) {
		std::ostringstream oss;
		oss << std::endl;
#ifdef VERSION
		oss << Color::boldBlue() << "pstate-frequency  "
			<< Color::boldMagenta() << VERSION << Color::reset()
			<< std::endl;
#endif
		std::cout << oss.str();
	}
}

/*
 * Print out the current CPU settings as configured either from
 * the cpufreq sysfs files or the intel_pstate sysfs files.
 */
void printCpuValues(const Cpu &cpu)
{
	if (Log::isOutputCapable()) {
		printVersion();
		std::ostringstream oss;
		oss << Color::boldWhite()
			<< "    pstate::" << Color::boldGreen()
			<< "CPU_DRIVER     -> "
			<< Color::boldCyan() << cpu.getDriver()
			<< std::endl;
		oss << Color::boldWhite()
			<< "    pstate::" << Color::boldGreen()
			<< "CPU_GOVERNOR   -> "
			<< Color::boldCyan() << cpu.getGovernor()
			<< std::endl;
		const int turbo = cpu.getTurboBoost();
		oss << Color::boldWhite()
			<< "    pstate::" << Color::boldGreen()
			<< (cpu.hasPstate()
				? "NO_TURBO       -> "
				: "TURBO_BOOST    -> ")
			<< Color::boldCyan() << turbo << " : "
			<< (cpu.hasPstate()
				? (turbo == 1
					? "OFF"
					: "ON")
				: (turbo == 1
					? "ON"
					: "OFF"))
			<< std::endl;
		oss << Color::boldWhite()
			<< "    pstate::" << Color::boldGreen()
			<< "CPU_MIN        -> "
			<< Color::boldCyan() << cpu.getMinValue() << "% : "
			<< static_cast<int>(cpu.getScalingMinFrequency())
			<< "KHz" << std::endl;
		oss << Color::boldWhite()
			<< "    pstate::" << Color::boldGreen()
			<< "CPU_MAX        -> "
			<< Color::boldCyan() << cpu.getMaxValue() << "% : "
			<< static_cast<int>(cpu.getScalingMaxFrequency())
			<< "KHz" << std::endl;
		oss << Color::reset();
		std::cout << oss.str();
	}
}

/*
 * Print the program's options and the intended methods of use.
 */
void printHelp()
{
	if (Log::isOutputCapable()) {
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
			<< "boost state" << std::endl
			<< "    --sleep          Sleep before setting "
			<< "CPU frequencies" << std::endl;
		std::cout << oss.str();
	}
}

}
