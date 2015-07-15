/*
 * pstate-frequency Easier control of the Intel p-state driver
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
 * For questions please contact pyamsoft at pyam(dot)soft(at)gmail(dot)com
 */

#ifndef C_PSFREQ_OUTPUT_H_
#define C_PSFREQ_OUTPUT_H_


#include "psfreq_cpu.h"

namespace psfreq {

void printCpuValues(const psfreq::Cpu &cpu);
void printRealtimeFrequency(const psfreq::Cpu &cpu);
void printGPL();
void printVersion();
void printHelp();
void printPlanHelp();
void printGovernorHelp(const std::vector<std::string> &availableGovernors);

}

#endif

