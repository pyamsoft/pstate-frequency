/**
 *  Copyright (C) 2014 - 2016 Peter Kenji Yamanaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef PSFREQ_COLOR_H
#define PSFREQ_COLOR_H

/**
 * Enable colored output
 */
void psfreq_color_enable(void);

/**
 * Reset output color
 *
 * @return Reset output color
 */
char *psfreq_color_reset(void);

/**
 * Red output color
 *
 * @return Red output color
 */
char *psfreq_color_red(void);

/**
 * Green output color
 *
 * @return Green output color
 */
char *psfreq_color_green(void);

/**
 * Blue output color
 *
 * @return Blue output color
 */
char *psfreq_color_blue(void);

/**
 * Magenta output color
 *
 * @return Magenta output color
 */
char *psfreq_color_magenta(void);

/**
 * Cyan output color
 *
 * @return Cyan output color
 */
char *psfreq_color_cyan(void);

/**
 * White output color
 *
 * @return White output color
 */
char *psfreq_color_white(void);

#endif

