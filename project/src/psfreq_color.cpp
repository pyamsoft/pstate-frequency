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

#include "include/psfreq_color.h"

namespace psfreq {

bool Color::enabled = false;
const char *const Color::OFF = "\033[0m";
const char *const Color::BOLD_RED = "\033[1;31m";
const char *const Color::BOLD_GREEN = "\033[1;32m";
const char *const Color::BOLD_BLUE = "\033[1;34m";
const char *const Color::BOLD_MAGENTA = "\033[1;35m";
const char *const Color::BOLD_CYAN = "\033[1;36m";
const char *const Color::BOLD_WHITE = "\033[1;37m";

void Color::setEnabled()
{
	enabled = true;
}

const char *Color::reset()
{
	if (enabled) {
		return OFF;
	} else {
		return "";
	}
}

const char *Color::boldRed()
{
	if (enabled) {
		return BOLD_RED;
	} else {
		return "";
	}
}

const char *Color::boldGreen()
{
	if (enabled) {
		return BOLD_GREEN;
	} else {
		return "";
	}
}

const char *Color::boldBlue()
{
	if (enabled) {
		return BOLD_BLUE;
	} else {
		return "";
	}
}

const char *Color::boldMagenta()
{
	if (enabled) {
		return BOLD_MAGENTA;
	} else {
		return "";
	}
}

const char *Color::boldCyan()
{
	if (enabled) {
		return BOLD_CYAN;
	} else {
		return "";
	}
}

const char *Color::boldWhite()
{
	if (enabled) {
		return BOLD_WHITE;
	} else {
		return "";
	}
}

}
