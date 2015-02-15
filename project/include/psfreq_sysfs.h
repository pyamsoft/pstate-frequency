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

#ifndef CPP_PSFREQ_SYSFS_H
#define CPP_PSFREQ_SYSFS_H

#include <string>
#include <vector>

namespace psfreq {

class sysfs {
private:
	std::string basePath;
public:
	sysfs();
	~sysfs();
	bool exists(const std::string &path, const std::string &file) const;
	bool exists(const std::string &file) const;
	void write(const std::string &path, const std::string &file, const std::string &buffer) const;
	void write(const std::string &path,const std::string &file, const int number) const;
	void write(const std::string &file, const std::string &buffer) const;
	void write(const std::string &file, const int number) const;
	const std::string read(const std::string &file) const;
	const std::string read(const std::string &path, const std::string &file) const;
	const std::vector<std::string> readAll(const std::string &file) const;
	const std::vector<std::string> readAll(const std::string &path, const std::string &file) const;
	const std::vector<std::string> readPipe(const char* command, const unsigned int number) const;
};

}

#endif
