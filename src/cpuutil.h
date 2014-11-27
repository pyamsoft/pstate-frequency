
#ifndef C_PYAM_CPUUTIL_H_
#define C_PYAM_CPUUTIL_H_

#include <stdint.h>
#include <stdarg.h>
#include "src/cpu.h"

char* resolve_path_to_file(struct cpu_t *const cpu, const char *const file_name);
void safe_malloc(struct cpu_t *const cpu, const int32_t result,
        const char *const error_message, ...);
void check_file(struct cpu_t *const cpu, FILE *const file,
        const char *const file_name);
#endif

