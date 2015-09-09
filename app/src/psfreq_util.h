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

#ifndef CPP_PSFREQ_UTIL_H
#define CPP_PSFREQ_UTIL_H

#include <string>
#include <vector>

#define BAD_NUMBER -1

namespace psfreq {

const std::string UNINITIALIZED_STR = std::string();

double stringToNumber(const std::string &line);
const std::string numberToString(const unsigned int number);
int boundValue(const int value, const int minBound,
	const int maxBound);
bool stringStartsWith(const std::string &control, const std::string &value);
const std::vector<std::string> &splitString(const std::string &s, char delim,
	std::vector<std::string> &elems);
const std::vector<std::string> splitString(const std::string &s, char delim);

}

#endif
