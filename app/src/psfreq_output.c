/**
 * @file psfreq_output.c
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 * Helper functions for presenting formatted output
 */

#include "psfreq_color.h"
#include"psfreq_log.h"
#include"psfreq_option.h"
#include"psfreq_output.h"

bool psfreq_output_get_cpu(const psfreq_cpu_type *const cpu,
                           const psfreq_option_type *const options)
{
        if (cpu == NULL) {
                psfreq_log_error("psfreq_output_get_cpu",
                                "cpu is NULL");
                return false;
        }

        if (options == NULL) {
                psfreq_log_error("psfreq_output_get_cpu",
                                "options is NULL");
                return false;
        }
#ifdef VERSION
	psfreq_log("%spstate-frequency %s%s", psfreq_color_blue(), VERSION,
                        psfreq_color_reset());
#else
	psfreq_log("%spstate-frequency %s", psfreq_color_blue(),
                        psfreq_color_reset());
#endif
        if (options->cpu_get_type == CPU_GET_TYPE_CURRENT) {
                psfreq_log("    %spstate::%s%s%s-> %s%s%s",
                                psfreq_color_green(),
                                psfreq_color_blue(),
                                "CPU_DRIVER   ",
                                psfreq_color_reset(),
                                psfreq_color_cyan(),
                                cpu->scaling_driver,
                                psfreq_color_reset());
                psfreq_log("    %spstate::%s%s%s-> %s%s%s",
                                psfreq_color_green(),
                                psfreq_color_blue(),
                                "CPU_GOVERNOR ",
                                psfreq_color_reset(),
                                psfreq_color_cyan(),
                                cpu->scaling_governor,
                                psfreq_color_reset());
                psfreq_log("    %spstate::%s%s%s-> %s%d [%s]%s",
                                psfreq_color_green(),
                                psfreq_color_blue(),
                                "NO_TURBO     ",
                                psfreq_color_reset(),
                                psfreq_color_cyan(),
                                cpu->pst_turbo,
                                (cpu->pst_turbo ?"OFF" :"ON"),
                                psfreq_color_reset());
                psfreq_log("    %spstate::%s%s%s-> %s%d%% [%uKHz]%s",
                                psfreq_color_green(),
                                psfreq_color_blue(),
                                "CPU_MIN      ",
                                psfreq_color_reset(),
                                psfreq_color_cyan(),
                                psfreq_cpu_get_scaling_min(cpu),
                                cpu->scaling_min_freq,
                                psfreq_color_reset());
                psfreq_log("    %spstate::%s%s%s-> %s%d%% [%uKHz]%s",
                                psfreq_color_green(),
                                psfreq_color_blue(),
                                "CPU_MAX      ",
                                psfreq_color_reset(),
                                psfreq_color_cyan(),
                                psfreq_cpu_get_scaling_max(cpu),
                                cpu->scaling_max_freq,
                                psfreq_color_reset());
        } else if (options->cpu_get_type == CPU_GET_TYPE_REAL) {
                unsigned int i;
                char **frequencies = psfreq_cpu_get_real_freqs(cpu);
                for (i = 0; i < cpu->cpu_num; ++i) {
                        psfreq_log("    pstate::CPU[%d]   -> %s",
                                        i, frequencies[i]);
                        free(frequencies[i]);
                }

                free(frequencies);
        } else {
                return false;
        }
        return true;
}

void psfreq_output_usage(void)
{
	psfreq_log(
"pstate-frequency comes with ABSOLUTELY NO WARRANTY. \n"
"This is free software, and you are welcome to redistribute it \n"
"under certain conditions. \n"
"Please see the README for details. \n");
	psfreq_log(
"\n"
"usage: \n"
"pstate-frequency [verbose] [ACTION] [option(s)] \n"
" \n"
"verbose: \n"
"    unprivilaged: \n"
"    -d | --debug     Print debugging messages to stdout (multiple) \n"
"    -q | --quiet     Supress all non-error output (multiple) \n"
"    --color          Colorize output \n");
	psfreq_log(
" \n"
"actions: \n"
"    unprivilaged: \n"
"    -H | --help      Display this help and exit \n"
"    -V | --version   Display application version and exit \n"
"    -G | --get       Access current CPU values \n"
"    privilaged: \n"
"    -S | --set       Modify current CPU values \n");
	psfreq_log(
" \n"
"options: \n"
"    unprivilaged: \n"
"    -c | --current   Display the current user set CPU values \n"
"    -r | --real      Display the real time CPU frequencies \n"
"    privilaged: \n"
"    -p | --plan      Set a predefined power plan \n"
"    -m | --max       Modify current CPU max frequency \n"
"    -g | --governor  Set the cpufreq governor \n"
"    -n | --min       Modify current CPU min frequency \n"
"    -t | --turbo     Modify curent CPU turbo boost state \n"
"    --no-sleep       Do not sleep before setting CPU frequencies \n");
}

void psfreq_output_version(void)
{
	psfreq_log(
"pstate-frequency comes with ABSOLUTELY NO WARRANTY. \n"
"This is free software, and you are welcome to redistribute it \n"
"under certain conditions. \n"
"Please see the README for details.\n");
#ifdef VERSION
	psfreq_log("pstate-frequency %s", VERSION);
#else
	psfreq_log_error("psfreq_output_version",
		"pstate-frequency version is UNDEFINED");
#endif
}

