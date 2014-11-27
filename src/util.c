#define _GNU_SOURCE
#include <stdio.h>
#include "src/util.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int32_t debug = 0;

int32_t str_starts_with(const char *restrict string, const char *restrict prefix)
{
        while (*prefix) {
                if (*prefix++ != *string++)
                        return 0;
        }
        return 1;
}


double str_to_num(char *const line)
{
        const double value = strtol(line, NULL, 10);
        free(line);
        return value;
}

double fix_value_range(const double value, const double min, const double max)
{
        if (value < min)
                return min;
        else if (value > max)
                return max;
        else
                return value;
}


int32_t string_is_digits(const char *const string)
{
        const int32_t length = strlen(string);
        for (int32_t i = 0; i < length; ++i) {
                if (!isdigit(string[i]))
                        return 0;
        }
        return 1;
}

void log_debug(const char *const format, ...)
{
        if (debug) {
                va_list arg;
                va_start(arg, format);
                vfprintf(stdout, format, arg);
                va_end (arg);
        }
}

void log_error(const char *const format, ...)
{
        if (debug) {
                va_list arg;
                va_start(arg, format);
                vfprintf(stderr, format, arg);
                va_end (arg);
        }
}
