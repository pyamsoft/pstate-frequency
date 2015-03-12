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

#ifndef C_PSFREQ_COLORS_H_
#define C_PSFREQ_COLORS_H_

namespace psfreq {

class Color
{
private:
	static bool enabled;
	static const char *const COLOR_RESET;
	static const char *const BOLD_RED;
	static const char *const BOLD_GREEN;
	static const char *const BOLD_BLUE;
	static const char *const BOLD_MAGENTA;
	static const char *const BOLD_CYAN;
	static const char *const BOLD_WHITE;
	static const char *const NO_COLOR;
public:
	static void setEnabled();
	static const char *reset();
	static const char *boldRed();
	static const char *boldGreen();
	static const char *boldBlue();
	static const char *boldMagenta();
	static const char *boldCyan();
	static const char *boldWhite();
};

}

#endif
