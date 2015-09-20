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

#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

#include "psfreq_color.h"
#include "psfreq_log.h"
#include "psfreq_util.h"

namespace psfreq {

double stringToNumber(const std::string &line)
{
        const unsigned int length = line.length();
        for (unsigned int i = 0; i < length; ++i) {
                const char check = line[i];
                if (!std::isdigit(check) && !std::iscntrl(check)) {
                        if (!Log::isAllQuiet()) {
                                std::cerr << Color::boldRed()
                                        << "[Error] string: '" << line
                                        << "' is non digit at [" << i << "]"
                                        << Color::reset()
                                        << std::endl;
                        }
                        return BAD_NUMBER;
                }
        }
        std::istringstream iss(line);
        int result;
        iss >> result;
        if (Log::isDebug()) {
                std::cout << "[Debug] string: '" << line
                        << "' to number: '" << result << "'"
                        << std::endl;
        }
        return result;
}

const std::string numberToString(const unsigned int number)
{
        std::ostringstream oss;
        oss << number;
        const std::string s = oss.str();
        if (Log::isDebug()) {
                std::cout << "[Debug] number: '" << number
                        << "' to string: '" << s << "'"
                        << std::endl;
        }
        return s;
}

int boundValue(const int value, const int minBound,
                const int maxBound)
{
        if (value < minBound) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] value: '" << value
                                << "' bound to min: '" << minBound << "'"
                                << std::endl;
                }
                return minBound;
        } else if (value > maxBound) {
                if (Log::isDebug()) {
                        std::cout << "[Debug] value: '" << value
                                << "' bound to max: '" << maxBound << "'"
                                << std::endl;
                }
                return maxBound;
        } else {
                if (Log::isDebug()) {
                        std::cout << "[Debug] value: '" << value
                                << "' not bound" << std::endl;
                }
                return value;
        }
}

bool stringStartsWith(const std::string &control, const std::string &value)
{
        if (Log::isDebug()) {
                std::cout << "[Debug] compare control string: '" << control
                        << "' starts with string '" << value
                        << "'" << std::endl;
        }
        return control.compare(0, value.length(), value) == 0;
}

const std::vector<std::string> &splitString(const std::string &s, char delim,
        std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

const std::vector<std::string> splitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    splitString(s, delim, elems);
    return elems;
}

}

