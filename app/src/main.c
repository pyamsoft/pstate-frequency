#include <unistd.h>

#include "psfreq_cpu.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_option.h"
#include "psfreq_output.h"
#include "psfreq_sysfs.h"

unsigned char init_cpu_and_sysfs(psfreq_cpu_type *cpu,
                psfreq_sysfs_type *sysfs);

unsigned char init_cpu_and_sysfs(psfreq_cpu_type *cpu,
                psfreq_sysfs_type *sysfs)
{
        psfreq_sysfs_init(sysfs);
        if (!psfreq_cpu_init(cpu, sysfs)) {
                return 0;
        }
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

        psfreq_log("%d", options.action);
        psfreq_log("%s", options.cpu_turbo);
        psfreq_log("%s", options.cpu_max);
        psfreq_log("%s", options.cpu_min);
        psfreq_log("%s", options.cpu_governor);
        psfreq_log("%s", options.cpu_plan);
        psfreq_log("%u", options.cpu_get_type);
        psfreq_log("%u", options.cpu_sleep);
        psfreq_log("%u", options.color_enabled);

        if (options.action == ACTION_TYPE_HELP) {
                /* Print Help */
                return EXIT_SUCCESS;
        } else if (options.action == ACTION_TYPE_VERSION) {
                /* Print Version */
                return EXIT_SUCCESS;
        }

        if (options.action == ACTION_TYPE_CPU_SET) {
                if (geteuid() == 0) {
                        /* KLUDGE
                         * Making this section into a function with pointers
                         * causes memory leaks
                         */
                        if (!init_cpu_and_sysfs(&cpu, &sysfs)) {
                                return EXIT_FAILURE;
                        }
                        /* Set cpu */

                        /* After setting the cpu, re-init the changed areas */
                        psfreq_cpu_reinit(&cpu, &sysfs);
                } else {
                        psfreq_log_error("main", "You must be root.");
                        return EXIT_FAILURE;
                }
        } else if (options.action == ACTION_TYPE_CPU_GET) {
                /* KLUDGE
                 * Making this section into a function with pointers
                 * causes memory leaks
                 */
                if (!init_cpu_and_sysfs(&cpu, &sysfs)) {
                        return EXIT_FAILURE;
                }
        }
        psfreq_out_get_cpu(&cpu);
	psfreq_cpu_destroy(&cpu);
	return EXIT_SUCCESS;
}
