/*
 * pstate_frequency Easier control of the Intel p-state driver
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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "src/cpu.h"
#include "src/cpuread.h"
#include "src/cpuwrite.h"
#include "src/color.h"
#include "src/plan.h"
#include "src/util.h"

static int32_t handle_result(struct cpu_t *const cpu, struct flag_t *const flags, const int32_t result);
static void print_version(void);
static int32_t access_cpu(struct cpu_t *const cpu, struct flag_t *const flags);
static void print_help(void);
static void print_output(struct cpu_t *const cpu);
static void print_possible_set(void);
static void print_io_scheduler(struct cpu_t *const cpu);
static void quit(struct cpu_t *cpu, struct flag_t *flags);

static void quit(struct cpu_t *cpu, struct flag_t *flags)
{
	cpu_destroy(cpu);
	flags_destroy(flags);
	
}
int main(int32_t argc, char **argv)
{
	struct flag_t flags = flags_create();
        struct cpu_t cpu = cpu_create();

        int32_t final_result = 0;
        int32_t result = -1;
        static struct option long_options[] = {
                {"help",          no_argument,        NULL,           'h'},
                {"version",       no_argument,        NULL,           'v'},
                {"get",           no_argument,        NULL,           'g'},
                {"set",           no_argument,        NULL,           's'},
                {"debug",         no_argument,        NULL,           'd'},
                {"plan",          required_argument,  NULL,           'p'},
                {"io",            required_argument,  NULL,           'i'},
                {"io-scheduler",  required_argument,  NULL,           'i'},
                {"gov",           required_argument,  NULL,           'o'},
                {"governor",	  required_argument,  NULL,           'o'},
		{"max",		  required_argument,  NULL,           'm'},
		{"min",		  required_argument,  NULL,           'n'},
		{"turbo",	  required_argument,  NULL,           't'},
		{0,		  0,                  0,              0}
                };
        while (1) {
                int32_t option_index = 0;
                result = getopt_long(argc, argv, "hvsdgp:m:n:t:i:o:", long_options, &option_index);
                if (result == -1) {
                        break;
                } else {
                        final_result = handle_result(&cpu, &flags, result);
                        if (final_result == -1) {
				quit(&cpu, &flags);
                                return 1;
                        } else if (final_result == 1) {
				quit(&cpu, &flags);
                                return 0;
                        }
                }
        }
        return access_cpu(&cpu, &flags);
}

/*
 * Parses the result of the getopt_long function
 */
static int32_t handle_result(struct cpu_t *const cpu, struct flag_t *const flags, const int32_t result)
{
        switch(result) {
        case 0:
                return 0;
        case 'h':
                print_help();
                return 1;
        case 'v':
                print_version();
                return 1;
        case 's':
                flags->action = ACTION_SET;
                return 0;
        case 'd':
                debug = 1;
                return 0;
        case 'g':
                flags->action = ACTION_GET;
                return 0;
        case 'p':
                return set_plan(cpu, flags);
        case 'm':
                if (string_is_digits(optarg)) {
                        if (flags->max == FLAG_UNINITIALIZED) {
                                flags->max = strtol(optarg, NULL, 10);
                                flags->max = fix_value_range(flags->max, cpu->CPU_INFO_MIN + 1, 100);
                        }
                return 0;
                }
                fprintf(stderr, "%sMax Frequency must be positive, non-zero integer value%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
	case 'i':
		printf("TODO Option: IO_SCHEDULER\n");
		return 0;
	case 'o':
		printf("TODO Option: GOVERNOR\n");
		return 0;
        case 'n':
                if (string_is_digits(optarg)) {
                        if (flags->min == FLAG_UNINITIALIZED) {
                                flags->min = strtol(optarg, NULL, 10);
                                flags->min = fix_value_range(flags->min, cpu->CPU_INFO_MIN, 99);
                        }
                return 0;
                }
                fprintf(stderr, "%sMin Frequency must be positive, non-zero integer value%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
        case 't':
                if (string_is_digits(optarg)) {
                        if (flags->turbo == FLAG_UNINITIALIZED)
                                flags->turbo = strtol(optarg, NULL, 10);
                        return 0;
                }
                fprintf(stderr, "%sTurbo Boost must be either 0 or 1%s\n",
                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                return -1;
        case '?':
                return -1;
        }
        return -1;
}

/*
 * Modify the CPU attributes with the new requested values
 */
static int32_t access_cpu(struct cpu_t *const cpu, struct flag_t *const flags)
{
        switch(flags->action) {
        case ACTION_SET:
                if (geteuid() == 0) {
			const int32_t flag_min = flags->min;
			const int32_t flag_max = flags->max;
			const int32_t flag_turbo = flags->turbo;
                        if (!(flag_max != FLAG_UNINITIALIZED
					|| flag_min != FLAG_UNINITIALIZED 
					|| flag_turbo == 1 || flag_turbo == 0)) {
                                fprintf(stderr, "%sSet called with no target or invalid values%s\n",
                                        PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
                                print_possible_set();
				quit(cpu, flags);
                                return 1;
                        }
			log_debug("MAX is %d %d\n", (flag_max >= 0), flag_max);
			log_debug("MIN is %d %d\n", (flag_min >= 0), flag_min);
			log_debug("TURBO is %d %d\n", (flag_turbo >= 0), flag_turbo);
			const int32_t max = flag_max >= 0 ? flag_max : get_scaling_max(cpu);
			const int32_t real_max = (max > flag_min) ? max : flag_min + 1;
			const int32_t real_min = flag_min >= 0 ? flag_min : get_scaling_min(cpu);
			const int32_t real_turbo = (flag_turbo == 0 || flag_turbo == 1)
				? flag_turbo : get_turbo(cpu);
			log_debug("Resetting CPU to sane defaults\n");
			set_turbo(cpu, 0);
			set_scaling_max(cpu, 100);
			set_scaling_min(cpu, 0);
			log_debug("Set turbo = %d\n", real_turbo);
			log_debug("Set max = %d\n", real_max);
			log_debug("Set min = %d\n", real_min);
			set_turbo(cpu, real_turbo);
			set_scaling_max(cpu, real_max);
			set_scaling_min(cpu, real_min);
			print_output(cpu);
                } else {
                        fprintf(stderr, "%sRoot privilages required%s\n",
                                PYAM_COLOR_BOLD_RED, PYAM_COLOR_OFF);
			quit(cpu, flags);
                        return 1;
                }
                break;
        case ACTION_GET:
		if (flags->iosched != NULL)
			print_io_scheduler(cpu);
		else
			print_output(cpu);
                break;
        default:
                print_help();
                break;
        }
	quit(cpu, flags);
        return 0;
}

/*
 * Print possible commands when running with --set
 */
static void print_possible_set()
{
        printf("Possible set values are: \n");
        printf("    -m | --max   Set the max scaling frequency to a number between 0 and 100 inclusive\n");
        printf("    -n | --min   Set the min scaling frequency to a number between 0 and 100 inclusive\n");
        printf("    -t | --turbo Set the state of turbo boost to either 1 (OFF) or 0 (ON)\n");
        printf("    -p | --plan  Set a predefined power plan\n");
}

/*
 * Print the currently active IO SCHEDULER
 */
static void print_io_scheduler(struct cpu_t *const cpu)
{
        printf("%s    pstate::%sIO_SCHED  -> %s%s%s",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu->IO_SCHED, PYAM_COLOR_OFF);
}

/*
 * Print program results to the stdout
 */
static void print_output(struct cpu_t *const cpu)
{
        const int32_t turbo = get_turbo(cpu);
        const char* turbo_string = turbo == 0 ? "ON" : (turbo == 1 ? "OFF" : "INVALID");
        const int32_t cpu_max = get_scaling_max(cpu);
        const int32_t cpu_min = get_scaling_min(cpu);
        const int32_t max_mhz = get_scaling_max_freq(cpu);
        const int32_t min_mhz = get_scaling_min_freq(cpu);
#ifdef VERSION
        printf("%spstate-frequency %s%s%s\n",
                PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_BOLD_MAGENTA,
                VERSION, PYAM_COLOR_OFF);
#else
        printf("%spstate-frequency%s\n",
                PYAM_COLOR_BOLD_BLUE, PYAM_COLOR_OFF);
#endif
        printf("%s    pstate::%sCPU_DRIVER    -> %s%s%s",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu->CPU_DRIVER, PYAM_COLOR_OFF);
        printf("%s    pstate::%sCPU_GOVERNOR  -> %s%s%s",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu->CPU_GOVERNOR, PYAM_COLOR_OFF);
        printf("%s    pstate::%sNO_TURBO      -> %s%d : %s%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                turbo, turbo_string, PYAM_COLOR_OFF);
        printf("%s    pstate::%sCPU_MIN       -> %s%d%% : %dKhz%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu_min, min_mhz, PYAM_COLOR_OFF);
        printf("%s    pstate::%sCPU_MAX       -> %s%d%% : %dKhz%s\n",
                PYAM_COLOR_BOLD_WHITE, PYAM_COLOR_BOLD_GREEN, PYAM_COLOR_BOLD_CYAN,
                cpu_max, max_mhz, PYAM_COLOR_OFF);
        printf("\n");
}

/*
 * Print program version
 */
static void print_version()
{
#ifdef VERSION
        printf("pstate-frequency version %s\n", VERSION);
#endif
        printf("pstate-frequency comes with ABSOLUTELY NO WARRANTY.\n");
        printf("This is free software, and you are welcome to redistribute it\n");
        printf("under certain conditions;\n");
}

/*
 * Print program help message
 */
static void print_help()
{
        print_version();
        printf("\n");
        printf("usage:\n");
	printf("pstate-frequency [action] [option]\n");
        printf("\n");
        printf("    actions:\n");
	printf("        unprivilaged:\n");
        printf("            -h | --help                 Display this help and exit\n");
        printf("            -v | --version              Display application version and exit\n");
        printf("            -d | --debug                Print debugging messages to stdout\n");
        printf("            -g | --get                  Access current CPU values\n");
	printf("\n");
	printf("        privilaged:\n");
        printf("            -s | --set                  Modify current CPU values\n");
	printf("\n");
        printf("    options:\n");
	printf("        unprivilaged:\n");
	printf("            -i | --io | --io-scheduler  Get the IO Scheduler for the given device\n");
	printf("\n");
	printf("        privilaged:\n");
        printf("            -p | --plan		        Set a predefined power plan\n");
        printf("            -m | --max		        Modify current CPU max frequency\n");
	printf("            -i | --io | --io-scheduler  Set the IO Scheduler for the given device\n");
	printf("            -o | --gov | --governor     Set the cpufreq governor\n");
        printf("            -n | --min                  Modify current CPU min frequency\n");
        printf("            -t | --turbo                Modify curent CPU turbo boost state\n");
        printf("\n");
}
