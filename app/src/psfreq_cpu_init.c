#include <stdlib.h>
#include <string.h>

#include "psfreq_cpu.h"
#include "psfreq_log.h"
#include "psfreq_strings.h"
#include "psfreq_util.h"

void psfreq_cpu_init(struct psfreq_cpu_type *cpu,
                const struct psfreq_sysfs_type *sysfs)
{
        cpu->cpu_num = psfreq_cpu_init_find_number_cpus();
        cpu->has_pstate = psfreq_cpu_init_system_has_pstate(sysfs);
}

/*
 * Find the total number of CPUS (logical and physical) that exist on the
 * system.
 */
uint8_t psfreq_cpu_init_find_number_cpus(void)
{
        const char *const cmd = "grep processor /proc/cpuinfo | wc -l";

        uint8_t size = 1;
        char **res = NULL;
        psfreq_util_read_pipe(&res, cmd, &size);
        if (res == NULL) {
                psfreq_log_error("psfreq_cpu_init_find_number_cpus",
                                "Failed to find number of cpus");
                return 0;
        }

        const uint32_t n = psfreq_strings_to_uint(res[0]);
        psfreq_log_debug("psfreq_cpu_init_find_number_cpus",
                        "Free memory held by res");
        free(res[0]);
        free(res);
        psfreq_log_debug("psfreq_cpu_init_find_number_cpus",
                        "Number of cpus: %u", n);
        return n;
}

bool psfreq_cpu_init_system_has_pstate(const struct psfreq_sysfs_type *sysfs)
{
        char* driver = psfreq_sysfs_read(sysfs,
                        "cpu0/cpufreq/scaling_driver");
        if (driver == NULL) {
                psfreq_log_error("psfreq_cpu_init_system_has_pstate",
                                "Unable to check for intel_pstate driver");
                return false;
        }

        const char* cmp = "intel_pstate\n";
        psfreq_log_debug("psfreq_cpu_init_system_has_pstate",
                        "Compare driver '%s' with '%s'", driver, cmp);
        const bool r = (strcmp(driver, cmp) == 0);
        free(driver);
        return r;
}

