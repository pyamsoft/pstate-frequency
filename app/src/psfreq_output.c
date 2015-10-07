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

#include "psfreq_log.h"
#include "psfreq_option.h"
#include "psfreq_output.h"

unsigned char psfreq_output_get_cpu(const psfreq_cpu_type *const cpu,
                           const psfreq_option_type *const options)
{
#ifdef VERSION
	psfreq_log("pstate-frequency %s", VERSION);
#else
	psfreq_log("pstate-frequency");
#endif
        if (options->cpu_get_type == CPU_GET_TYPE_CURRENT) {
        psfreq_log("    pstate::CPU_DRIVER      -> %s", cpu->scaling_driver);
        psfreq_log("    pstate::CPU_GOVERNOR    -> %s", cpu->scaling_governor);
        psfreq_log("    pstate::NO_TURBO        -> %d : %s", cpu->pst_turbo,
                        (cpu->pst_turbo ? "OFF" : "ON"));
        psfreq_log("    pstate::CPU_MIN         -> %u%% : %uKHz",
                        psfreq_cpu_get_scaling_min(cpu), cpu->scaling_min_freq);
        psfreq_log("    pstate::CPU_MAX         -> %u%% : %uKHz",
                        psfreq_cpu_get_scaling_max(cpu), cpu->scaling_max_freq);
        } else if (options->cpu_get_type == CPU_GET_TYPE_REAL) {
                unsigned char i;
                char **frequencies = psfreq_cpu_get_real_freqs(cpu);
                for (i = 0; i < cpu->cpu_num; ++i) {
                        psfreq_log("    pstate::CPU[%d]   -> %s",
                                        i, frequencies[i]);
                        free(frequencies[i]);
                }

                free(frequencies);
        } else {
                return 0;
        }
        return 1;
}

