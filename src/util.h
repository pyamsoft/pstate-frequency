
#ifndef C_PYAM_UTIL_H_
#define C_PYAM_UTIL_H_

#include <stdint.h>
#include <stdarg.h>

extern int32_t debug;

int32_t str_starts_with(const char *restrict string, const char *restrict prefix);
double str_to_num(char *const line);
double fix_value_range(const double value, const double min, const double max);
int32_t string_is_digits(const char *const string);
void log_debug(const char *const format, ...);
void log_error(const char *const format, ...);

#endif
