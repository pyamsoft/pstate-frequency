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

#ifndef CPP_PSFREQ_LOG_H
#define CPP_PSFREQ_LOG_H

#define PSFREQ_LOG_ALL_QUIET 0
#define PSFREQ_LOG_QUIET 1
#define PSFREQ_LOG_NORMAL 2
#define PSFREQ_LOG_DEBUG 3

namespace psfreq {

class log {
private:
	static unsigned int verbosity;
public:
	static bool isAllQuiet();
	static bool isQuiet();
	static bool isNormal();
	static bool isDebug();
	static void setAllQuiet();
	static void setQuiet();
	static void setNormal();
	static void setDebug();
};

}

#endif
