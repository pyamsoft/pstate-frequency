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

#include "include/psfreq_log.h"

namespace psfreq {

unsigned int log::verbosity = PSFREQ_LOG_NORMAL;

bool log::isAllQuiet()
{
	return verbosity == PSFREQ_LOG_ALL_QUIET;
}

bool log::isQuiet()
{
	return verbosity == PSFREQ_LOG_QUIET;
}

bool log::isNormal()
{
	return verbosity == PSFREQ_LOG_NORMAL;
}

bool log::isDebug()
{
	return verbosity == PSFREQ_LOG_DEBUG;
}

void log::setAllQuiet()
{
	verbosity = PSFREQ_LOG_ALL_QUIET;
}

void log::setQuiet()
{
	verbosity = PSFREQ_LOG_QUIET;
}

void log::setNormal()
{
	verbosity = PSFREQ_LOG_NORMAL;
}

void log::setDebug()
{
	verbosity = PSFREQ_LOG_DEBUG;
}

}
