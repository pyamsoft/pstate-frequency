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

#include "psfreq_color.h"

/**
 * Set the output stream to a specified color
 *
 * @param color The color code to use
 * @return The color code if color is enabled, else empty string
 */
static char *psfreq_color_set(char *color);

static unsigned char COLORED = 1;
static unsigned char colored = 0;
static char *const none = "";
static char *const reset = "\033[0m";
static char *const red = "\033[1;31m";
static char *const green = "\033[1;32m";
static char *const blue = "\033[1;34m";
static char *const magenta = "\033[1;35m";
static char *const cyan = "\033[1;36m";
static char *const white = "\033[1;37m";

/**
 * Enable colored output
 */
void psfreq_color_enable(void)
{
        colored = 1;
}

/**
 * Reset output color
 *
 * @return Reset output color
 */
char *psfreq_color_reset(void)
{
        return psfreq_color_set(reset);
}

/**
 * Red output color
 *
 * @return Red output color
 */
char *psfreq_color_red(void)
{
        return psfreq_color_set(red);
}

/**
 * Green output color
 *
 * @return Green output color
 */
char *psfreq_color_green(void)
{
        return psfreq_color_set(green);
}

/**
 * Blue output color
 *
 * @return Blue output color
 */
char *psfreq_color_blue(void)
{
        return psfreq_color_set(blue);
}

/**
 * Magenta output color
 *
 * @return Magenta output color
 */
char *psfreq_color_magenta(void)
{
        return psfreq_color_set(magenta);
}

/**
 * Cyan output color
 *
 * @return Cyan output color
 */
char *psfreq_color_cyan(void)
{
        return psfreq_color_set(cyan);
}

/**
 * White output color
 *
 * @return White output color
 */
char *psfreq_color_white(void)
{
        return psfreq_color_set(white);
}

/**
 * Set the output stream to a specified color
 *
 * @param color The color code to use
 * @return The color code if color is enabled, else empty string
 */
static char *psfreq_color_set(char *color)
{
        char *s;
        if (colored == COLORED) {
                s = color;
        } else {
                s = none;
        }
        return s;
}

