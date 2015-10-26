/**
 * @file psfreq_color.c
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 * Constants for defining various color related arguments
 */

#include <stdbool.h>

#include "psfreq_color.h"

static bool colored = false;
static char *const none = "";
static char *const reset = "\033[0m";
static char *const red = "\033[1;31m";
static char *const green = "\033[1;32m";
static char *const blue = "\033[1;34m";
static char *const magenta = "\033[1;35m";
static char *const cyan = "\033[1;36m";
static char *const white = "\033[1;37m";

static char *psfreq_color_set(char *color);

static char *psfreq_color_set(char *color)
{
        char *s;
        if (colored) {
                s = color;
        } else {
                s = none;
        }
        return s;
}

void psfreq_color_enable(void)
{
        colored = true;
}

char *psfreq_color_reset(void)
{
        return psfreq_color_set(reset);
}

char *psfreq_color_red(void)
{
        return psfreq_color_set(red);
}

char *psfreq_color_green(void)
{
        return psfreq_color_set(green);
}

char *psfreq_color_blue(void)
{
        return psfreq_color_set(blue);
}

char *psfreq_color_magenta(void)
{
        return psfreq_color_set(magenta);
}

char *psfreq_color_cyan(void)
{
        return psfreq_color_set(cyan);
}

char *psfreq_color_white(void)
{
        return psfreq_color_set(white);
}

