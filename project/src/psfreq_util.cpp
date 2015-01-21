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
 * For questions please contact pyamsoft at pyam.soft@gmail.com
 */

#include <iostream>
#include <sstream>

#include "include/psfreq_util.h"

namespace psfreq {

double stringToNumber(const std::string &line)
{
        std::istringstream iss(line);
	int result;
	iss >> result;
	return result;
}

int boundValue(const int value, const int minBound,
		const int maxBound)
{
	if (value < minBound) {
		return minBound;
	} else if (value > maxBound) {
		return maxBound;
	} else {
		return value;
	}
}

bool stringStartsWith(const std::string &control, const std::string &value)
{
	return control.compare(0, value.length(), value) == 0;
}

}
