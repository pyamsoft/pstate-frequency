/**
 * @file main.c
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
 * pstate-frequency
 */
#include <stdlib.h>
#include <unistd.h>

#include "psfreq_constants.h"
#include "psfreq_cpu.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_option.h"
#include "psfreq_output.h"
#include "psfreq_plan.h"
#include "psfreq_strings.h"
#include "psfreq_sysfs.h"

static unsigned char has_reqeusted_options(const psfreq_option_type *options);
static unsigned char init_cpu_and_sysfs(psfreq_cpu_type *cpu,
                psfreq_sysfs_type *sysfs);
static int get_cpu_max(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options);
static int get_cpu_min(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options);
static char get_cpu_turbo(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options);
static unsigned char set_cpu_values_raw(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options, int *const max,
                int *const min, int *const turbo, char **const gov);
static char *get_cpu_gov(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options);
static unsigned char set_cpu(const psfreq_cpu_type *cpu,
                      const psfreq_sysfs_type *sysfs,
                      const int max, const int min,
                      const int turbo, const char *const gov,
                      const unsigned char do_sleep);

static const unsigned int EUID_ROOT = 0;
static const unsigned char SUCCESS = 1;
static const unsigned char FAILURE = 0;
static const long DELAY_PERIOD = 5;
static const long SLEEP_PERIOD = 2;

static unsigned char has_reqeusted_options(const psfreq_option_type *options)
{
        if (options->cpu_max == OPT_UNDEFINED
            && options->cpu_min == OPT_UNDEFINED
            && options->cpu_turbo == OPT_UNDEFINED
            && options->cpu_governor == OPT_UNDEFINED
            && options->cpu_plan == OPT_UNDEFINED) {
                return FAILURE;
        }
        return SUCCESS;
}

static unsigned char set_cpu_values_raw(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options, int *const max,
                int *const min, int *const turbo, char **const gov)
{
        *max = get_cpu_max(cpu, options);
        if (*max < CPU_FREQUENCY_MINIMUM) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Max %d", *max);
                return FAILURE;
        }
        *min = get_cpu_min(cpu, options);
        if (*min < CPU_FREQUENCY_MINIMUM) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Min %d", *min);
                return FAILURE;
        }
        *turbo = get_cpu_turbo(cpu, options);
        if (*turbo == TURBO_UNDEFINED) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Turbo %d", *turbo);
                return FAILURE;
        }
        *gov = get_cpu_gov(cpu, options);
        if (*gov == GOV_UNDEFINED) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Gov %s", gov);
                return FAILURE;
        }
        return SUCCESS;
}

static int get_cpu_max(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        int max;
        if (options->cpu_max != OPT_UNDEFINED) {
                max = psfreq_strings_to_int(options->cpu_max);
        } else {
                max = psfreq_cpu_get_scaling_max(cpu);
        }
        return max;
}

static int get_cpu_min(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        int min;
        if (options->cpu_min != OPT_UNDEFINED) {
                min = psfreq_strings_to_int(options->cpu_min);
        } else {
                min = psfreq_cpu_get_scaling_min(cpu);
        }
        return min;
}

static char get_cpu_turbo(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        char turbo;
        if (options->cpu_turbo != OPT_UNDEFINED) {
                turbo = psfreq_input_turbo_from_optarg(options->cpu_turbo);
        } else {
                turbo = cpu->pst_turbo;
        }
        return turbo;
}

static char *get_cpu_gov(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        char *gov;
        if (options->cpu_governor != OPT_UNDEFINED) {
                gov = psfreq_input_gov_from_optarg(options->cpu_governor);
        } else {
                gov = cpu->scaling_governor;
        }
        return gov;
}

static unsigned char init_cpu_and_sysfs(psfreq_cpu_type *cpu,
                psfreq_sysfs_type *sysfs)
{
        psfreq_sysfs_init(sysfs);
        if (psfreq_cpu_init(cpu, sysfs) == INIT_FAILURE) {
                psfreq_log_error("init_cpu_and_sysfs",
                        "System has failed to initialize");
                return FAILURE;
        }
        return SUCCESS;
}

static unsigned char set_cpu(const psfreq_cpu_type *cpu,
                      const psfreq_sysfs_type *sysfs,
                      const int max, const int min,
                      const int turbo, const char *const gov,
                      const unsigned char do_sleep)
{
        const int sane_max = CPU_FREQUENCY_MAXIMUM;
        const int sane_min = CPU_FREQUENCY_MINIMUM;
        const int sane_turbo = TURBO_OFF;
        const char *const sane_gov = "powersave";
        if (cpu == CPU_UNDEFINED) {
                psfreq_log_error("set_cpu",
                                "cpu is undefined");
                return FAILURE;
        }
        if (sysfs == SYSFS_UNDEFINED) {
                psfreq_log_error("set_cpu",
                                "sysfs is undefined");
                return FAILURE;
        }

        /* Set sane cpu values first */
        psfreq_cpu_set_max(cpu, sysfs, &sane_max);
        psfreq_cpu_set_min(cpu, sysfs, &sane_min);
        psfreq_cpu_set_gov(cpu, sysfs, sane_gov);
        psfreq_cpu_set_turbo(sysfs, &sane_turbo);

        if (do_sleep) {
                sleep(SLEEP_PERIOD);
        }

        psfreq_log_debug("set_cpu", "Set cpu values: %d, %d, %d, %s",
                        max, min, turbo, gov);
        psfreq_cpu_set_max(cpu, sysfs, &max);
        psfreq_cpu_set_min(cpu, sysfs, &min);
        psfreq_cpu_set_gov(cpu, sysfs, gov);
        psfreq_cpu_set_turbo(sysfs, &turbo);
        return SUCCESS;
}

int main(int argc, char **argv)
{
	psfreq_cpu_type cpu;
	psfreq_sysfs_type sysfs;
	psfreq_option_type options;

	psfreq_option_init(&options);
	if (psfreq_input_parse(&options, argc, argv) == INPUT_PARSE_FAILURE) {
                return EXIT_FAILURE;
        }

        if (options.delay
                        && options.action != ACTION_TYPE_HELP
                        && options.action != ACTION_TYPE_VERSION
                        && options.action != ACTION_TYPE_UNDEFINED) {
                /*
                 * Delay start up for 5 seconds for the
                 * file system to initialize
                 */
                sleep(DELAY_PERIOD);
        }

        if (options.action == ACTION_TYPE_HELP ||
                        options.action == ACTION_TYPE_UNDEFINED) {
                psfreq_output_usage();
                return EXIT_SUCCESS;
        } else if (options.action == ACTION_TYPE_VERSION) {
                psfreq_output_version();
                return EXIT_SUCCESS;
        }

        if (options.action == ACTION_TYPE_CPU_SET) {
                int max;
                int min;
                int turbo;
                char *gov = GOV_UNDEFINED;
                if (geteuid() != EUID_ROOT) {
                        psfreq_log_error("main", "You must be root.");
                        return EXIT_FAILURE;
                }
                if (has_reqeusted_options(&options) == FAILURE) {
                        psfreq_log_error("main",
                                        "An operation must be specified");
                        return EXIT_FAILURE;
                }
                if (init_cpu_and_sysfs(&cpu, &sysfs) == FAILURE) {
                        return EXIT_FAILURE;
                }
                /* Set cpu */
                const char *const p = options.cpu_plan;
                if (p == OPT_UNDEFINED) {
                        if (set_cpu_values_raw(&cpu, &options, &max, &min,
                                                &turbo, &gov) == FAILURE) {
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }

                        /* Actual set */
                        set_cpu(&cpu, &sysfs, max, min, turbo, gov,
                                        options.cpu_sleep);
                } else {
                        /* Pass max,min,turbo,gov and set by plan */
                        unsigned char r;
                        const char plan = psfreq_input_plan_from_optarg(p);
                        if (plan == PLAN_UNDEFINED) {
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }
                        r = psfreq_plan_set_cpu(&plan, &max, &min, &turbo,
                                        &gov);
                        if (r == POWER_PLAN_APPLY_FAILURE) {
                                psfreq_log_error("main",
                                                "Failed to set plan.");
                                if (gov != GOV_UNDEFINED) {
                                        free(gov);
                                }
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }

                        /* Actual set */
                        set_cpu(&cpu, &sysfs, max, min, turbo, gov,
                                        options.cpu_sleep);
                        if (gov != GOV_UNDEFINED) {
                                free(gov);
                        }
                }
                /* After setting the cpu, re-init the changed areas */
                psfreq_cpu_reinit(&cpu, &sysfs);
        } else if (options.action == ACTION_TYPE_CPU_GET) {
                if (init_cpu_and_sysfs(&cpu, &sysfs) == FAILURE) {
                        return EXIT_FAILURE;
                }
        }
        psfreq_output_get_cpu(&cpu, &options);
	psfreq_cpu_destroy(&cpu);
	return EXIT_SUCCESS;
}
