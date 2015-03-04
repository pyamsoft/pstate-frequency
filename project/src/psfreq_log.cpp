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

unsigned int Log::verbosity = PSFREQ_LOG_NORMAL;

bool Log::isAllQuiet()
{
	return verbosity == PSFREQ_LOG_ALL_QUIET;
}

bool Log::isQuiet()
{
	return verbosity == PSFREQ_LOG_QUIET;
}

bool Log::isNormal()
{
	return verbosity == PSFREQ_LOG_NORMAL;
}

bool Log::isDebug()
{
	return verbosity == PSFREQ_LOG_DEBUG;
}

bool Log::isOutputCapable()
{
	return (verbosity == PSFREQ_LOG_DEBUG
			|| verbosity == PSFREQ_LOG_NORMAL);
}

void Log::setAllQuiet()
{
	verbosity = PSFREQ_LOG_ALL_QUIET;
}

void Log::setQuiet()
{
	verbosity = PSFREQ_LOG_QUIET;
}

void Log::setNormal()
{
	verbosity = PSFREQ_LOG_NORMAL;
}

void Log::setDebug()
{
	verbosity = PSFREQ_LOG_DEBUG;
}

}
