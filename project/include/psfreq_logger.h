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

#ifndef CPP_PSFREQ_LOGGER_H
#define CPP_PSFREQ_LOGGER_H

#include <iostream>
#include <sstream>
#include <string>

namespace psfreq {

class logger {
private:

	static std::ostringstream oss;
	static int verbose;

	static const std::string flush()
	{
		const std::string str = oss.str();
		oss.clear();
		return str;
	}

public:
	logger()
	{
	}

	~logger()
	{
	}

	static void n(const std::string& log)
	{
		if (verbose != -1) {
			oss << log;
			const std::string str = flush();
			std::cout << str;
		}
	}

	static void d(const std::string& log)
	{
		if (verbose == 1) {
			oss << log;
			const std::string str = flush();
			std::cout << str;
		}
	}

	static void e(const std::string& log)
	{
		oss << log;
		const std::string str = flush();
		std::cerr << str;
	}

	static void close()
	{
		oss.clear();
		verbose = 0;
	}

	static bool isNormal()
	{
		return verbose == 0;
	}

	static bool isDebug()
	{
		return verbose == 1;
	}

	static bool isQuiet()
	{
		return verbose == -1;
	}

	static void setQuiet()
	{
		verbose = -1;
	}

	static void setNormal()
	{
		verbose = 0;
	}

	static void setDebug()
	{
		verbose = 1;
	}
};

}

#endif
