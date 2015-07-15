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

#include "include/psfreq_color.h"

namespace psfreq {

bool Color::enabled = false;
const char *const Color::COLOR_RESET = "\033[0m";
const char *const Color::BOLD_RED = "\033[1;31m";
const char *const Color::BOLD_GREEN = "\033[1;32m";
const char *const Color::BOLD_BLUE = "\033[1;34m";
const char *const Color::BOLD_MAGENTA = "\033[1;35m";
const char *const Color::BOLD_CYAN = "\033[1;36m";
const char *const Color::BOLD_WHITE = "\033[1;37m";
const char *const Color::NO_COLOR = "";

void Color::setEnabled()
{
	enabled = true;
}

const char *Color::reset()
{
	return enabled ? COLOR_RESET : NO_COLOR;
}

const char *Color::boldRed()
{
	return enabled ? BOLD_RED : NO_COLOR;
}

const char *Color::boldGreen()
{
	return enabled ? BOLD_GREEN : NO_COLOR;
}

const char *Color::boldBlue()
{
	return enabled ? BOLD_BLUE : NO_COLOR;
}

const char *Color::boldMagenta()
{
	return enabled ? BOLD_MAGENTA : NO_COLOR;
}

const char *Color::boldCyan()
{
	return enabled ? BOLD_CYAN : NO_COLOR;
}

const char *Color::boldWhite()
{
	return enabled ? BOLD_WHITE : NO_COLOR;
}

}
