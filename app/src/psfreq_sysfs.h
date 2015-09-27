/**
 * @file psfreq_sysfs.h
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
 * psfreq_sysfs_type makes it easy for operations which involve the sysfs
 * interfaces which are exposed on modern Linux systems.
 *
 * The sysfs requires two parameters in order to be effective, an instance
 * of the psfreq_cpu_type and the base path for the sysfs cpu device interfaces
 * which is usually located at /sys/devices/system/cpu
 */
#ifndef PSFREQ_SYSFS_H
#define PSFREQ_SYSFS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct psfreq_sysfs_type {
        const char *base_path;
} psfreq_sysfs_type;

void psfreq_sysfs_init(psfreq_sysfs_type *sysfs);

bool psfreq_sysfs_write(const psfreq_sysfs_type *sysfs,
                const char *file, const char *buf);
bool psfreq_sysfs_write_num(const psfreq_sysfs_type *sysfs,
                const char *file, const int32_t *num);
char *psfreq_sysfs_read(const psfreq_sysfs_type *sysfs,
                const char *file);

#endif

