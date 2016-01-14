/**
 * @file psfreq_sysfs.h
 * @author pyamsoft <pyam(dot)soft(at)gmail(dot)com>
 *
 * @section LICENSE
 *
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

/*
 * NULL is defined as 0, just hard code it here.
 * No need to include the entire stdlib.h
 */
#define SYSFS_UNDEFINED         0

/**
 * Struct representing the interface to the sysfs
 *
 */
typedef struct psfreq_sysfs_type {
        const char *base_path;
} psfreq_sysfs_type;

/**
 * Initialize a new psfreq_sysfs_type
 *
 * @param sysfs psfreq_sysfs_type instance to use
 * @return A new instance of psfreq_sysfs_type using the default base path
 */
void psfreq_sysfs_init(psfreq_sysfs_type *sysfs);

/**
 * Write a given value to a file
 *
 * @param sysfs psfreq_sysfs_type instance to use
 * @param file relative file path from sysfs->base_path to write to
 * @param buf buffer containing the value to write to file
 * @return Boolean value, true if writing to the file was successfully
 *         performed, false otherwise
 */
unsigned char psfreq_sysfs_write(const psfreq_sysfs_type *sysfs,
                const char *file, const char *buf);

/**
 * Wrapper around psfreq_sysfs_write to work with integer values
 *
 * @param sysfs psfreq_sysfs_type instance to use
 * @param file relative file path from sysfs->base_path to write to
 * @param num Number value to work with
 * @return Boolean value, true if writing to the file was successfully
 *         performed, false otherwise
 */
unsigned char psfreq_sysfs_write_num(const psfreq_sysfs_type *sysfs,
                const char *file, const int *num);

/**
 * Read string value from a file
 *
 * @param sysfs psfreq_sysfs_type instance to use
 * @param file relative file path from sysfs->base_path to write to
 * @return String containing file contents, NULL otherwise
 */
char *psfreq_sysfs_read(const psfreq_sysfs_type *sysfs, const char *file);

#endif

