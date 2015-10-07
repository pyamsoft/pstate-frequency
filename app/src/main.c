#include <unistd.h>

#include "psfreq_cpu.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_option.h"
#include "psfreq_output.h"
#include "psfreq_plan.h"
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
                      const int turbo, const char* const gov,
                      const unsigned char do_sleep);

static unsigned char has_reqeusted_options(const psfreq_option_type *options)
{
        if (options->cpu_max == CPU_UNDEFINED
            && options->cpu_min == CPU_UNDEFINED
            && options->cpu_turbo == TURBO_UNDEFINED
            && options->cpu_governor == GOVERNOR_UNDEFINED
            && options->cpu_plan == PLAN_UNDEFINED) {
                return 0;
        }
        return 1;
}

static unsigned char set_cpu_values_raw(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options, int *const max,
                int *const min, int *const turbo, char **const gov)
{
        *max = get_cpu_max(cpu, options);
        if (*max < 0) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Max %d", *max);
                return 0;
        }
        *min = get_cpu_min(cpu, options);
        if (*min < 0) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Min %d", *min);
                return 0;
        }
        *turbo = get_cpu_turbo(cpu, options);
        if (*turbo < 0) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Turbo %d", *turbo);
                return 0;
        }
        *gov = get_cpu_gov(cpu, options);
        if (*gov == NULL) {
                psfreq_log_error("set_cpu_values_raw",
                                "Bad Gov %s", gov);
                return 0;
        }
        return 1;
}

static int get_cpu_max(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        int max;
        if (options->cpu_max != CPU_UNDEFINED) {
                max = psfreq_input_max_from_optarg(options->cpu_max);
        } else {
                max = psfreq_cpu_get_scaling_max(cpu);
        }
        return max;
}

static int get_cpu_min(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        int min;
        if (options->cpu_min != CPU_UNDEFINED) {
                min = psfreq_input_min_from_optarg(options->cpu_min);
        } else {
                min = psfreq_cpu_get_scaling_min(cpu);
        }
        return min;
}

static char get_cpu_turbo(const psfreq_cpu_type *cpu,
                const psfreq_option_type *options)
{
        char turbo;
        if (options->cpu_turbo != TURBO_UNDEFINED) {
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
        if (options->cpu_governor != GOVERNOR_UNDEFINED) {
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
        if (!psfreq_cpu_init(cpu, sysfs)) {
                return 0;
        }
        return 1;
}

static unsigned char set_cpu(const psfreq_cpu_type *cpu,
                      const psfreq_sysfs_type *sysfs,
                      const int max, const int min,
                      const int turbo, const char* const gov,
                      const unsigned char do_sleep)
{
        const int sane_max = 100;
        const int sane_min = 0;
        const int sane_turbo = 1;
        const char *const sane_gov = "powersave";
        if (cpu == NULL) {
                psfreq_log_error("set_cpu",
                                "cpu is NULL");
                return 0;
        }
        if (sysfs == NULL) {
                psfreq_log_error("set_cpu",
                                "sysfs is NULL");
                return 0;
        }

        /* Set sane cpu values first */
        psfreq_cpu_set_max(cpu, sysfs, &sane_max);
        psfreq_cpu_set_min(cpu, sysfs, &sane_min);
        psfreq_cpu_set_gov(cpu, sysfs, sane_gov);
        psfreq_cpu_set_turbo(sysfs, &sane_turbo);

        if (do_sleep) {
                sleep(2);
        }

        psfreq_log_debug("set_cpu", "Set cpu values: %d, %d, %d, %s",
                        max, min, turbo, gov);
        psfreq_cpu_set_max(cpu, sysfs, &max);
        psfreq_cpu_set_min(cpu, sysfs, &min);
        psfreq_cpu_set_gov(cpu, sysfs, gov);
        psfreq_cpu_set_turbo(sysfs, &turbo);
        return 1;
}

int main(int argc, char **argv)
{
	psfreq_cpu_type cpu;
	psfreq_sysfs_type sysfs;
	psfreq_option_type options;

	psfreq_option_init(&options);
	if (psfreq_input_parse(&options, argc, argv) > 0) {
                return EXIT_FAILURE;
        }

        if (options.action == ACTION_TYPE_HELP ||
                        options.action == ACTION_TYPE_UNDEFINED) {
                /* Print Help */
                return EXIT_SUCCESS;
        } else if (options.action == ACTION_TYPE_VERSION) {
                /* Print Version */
                return EXIT_SUCCESS;
        }

        if (options.action == ACTION_TYPE_CPU_SET) {
                int max;
                int min;
                int turbo;
                char *gov = NULL;
                if (geteuid() != 0) {
                        psfreq_log_error("main", "You must be root.");
                        return EXIT_FAILURE;
                }
                if (!has_reqeusted_options(&options)) {
                        psfreq_log_error("main",
                                        "An operation must be specified");
                        return EXIT_FAILURE;
                }
                if (!init_cpu_and_sysfs(&cpu, &sysfs)) {
                        return EXIT_FAILURE;
                }
                /* Set cpu */
                if (options.cpu_plan == PLAN_UNDEFINED) {
                        if (!set_cpu_values_raw(&cpu, &options, &max,
                                                &min, &turbo, &gov)) {
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }

                        /* Actual set */
                        set_cpu(&cpu, &sysfs, max, min, turbo, gov,
                                        options.cpu_sleep);
                } else {
                        /* Pass max,min,turbo,gov and set by plan */
                        unsigned char r;
                        const char plan = \
                                        psfreq_input_plan_from_optarg(
                                                options.cpu_plan);
                        if (plan < 0) {
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }
                        r = psfreq_plan_set_cpu(&plan, &max, &min,
                                        &turbo, &gov);
                        if (!r) {
                                psfreq_log_error("main",
                                                "Failed to set plan.");
                                if (gov != NULL) {
                                        free(gov);
                                }
                                psfreq_cpu_destroy(&cpu);
                                return EXIT_FAILURE;
                        }

                        set_cpu(&cpu, &sysfs, max, min, turbo, gov,
                                        options.cpu_sleep);
                        /* Actual set */
                        if (gov != NULL) {
                                free(gov);
                        }
                }
                /* After setting the cpu, re-init the changed areas */
                psfreq_cpu_reinit(&cpu, &sysfs);
        } else if (options.action == ACTION_TYPE_CPU_GET) {
                if (!init_cpu_and_sysfs(&cpu, &sysfs)) {
                        return EXIT_FAILURE;
                }
        }
        psfreq_output_get_cpu(&cpu, &options);
	psfreq_cpu_destroy(&cpu);
	return EXIT_SUCCESS;
}
