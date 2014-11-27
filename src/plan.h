#ifndef C_PYAM_PLAN_H_
#define C_PYAM_PLAN_H_

#include <stdint.h>
#include "src/cpu.h"

int32_t set_plan(int32_t *const value_min, int32_t *const value_max,
                int32_t *const value_turbo);

#endif
