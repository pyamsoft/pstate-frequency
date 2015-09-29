#include "psfreq_cpu.h"
#include "psfreq_input.h"
#include "psfreq_log.h"
#include "psfreq_option.h"
#include "psfreq_sysfs.h"

int main(int argc, char **argv)
{
	psfreq_cpu_type cpu;
	psfreq_sysfs_type sysfs;
	psfreq_option_type options;
	char res;

	psfreq_option_init(&options);
	res = psfreq_input_parse(&options, argc, argv);
	if (res == OPTION_RETURNCODE_STOP_SUCCESS) {
		return 0;
	} else if (res == OPTION_RETURNCODE_STOP_FAILURE) {
		return 1;
	}

	psfreq_sysfs_init(&sysfs);
	if (!psfreq_cpu_init(&cpu, &sysfs)) {
		return 2;
	}

#ifdef VERSION
	psfreq_log("pstate-frequency %s", VERSION);
#else
	psfreq_log("pstate-frequency");
#endif
	psfreq_cpu_destroy(&cpu);
	return 0;
}
