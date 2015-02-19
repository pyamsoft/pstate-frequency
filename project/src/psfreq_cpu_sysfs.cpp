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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "include/psfreq_cpu.h"
#include "include/psfreq_log.h"

namespace psfreq {

bool cpu::sysfs::write(const std::string &path, const std::string &file,
		const std::string &buffer) const
{
	std::ostringstream oss;
	oss << path << file;
	const std::string absolutePath = oss.str();
	std::ofstream outputFile;
	outputFile.open(absolutePath.c_str());
	if (!outputFile.is_open()) {
		std::cerr << "Failed to write to file: " << absolutePath
			<< std::endl;
		return false;
	}
	outputFile << buffer << std::endl;
	outputFile.close();
	return true;
}

bool cpu::sysfs::write(const std::string &path,const std::string &file,
		const int number) const
{
	std::ostringstream oss;
	oss << number;
	return write(path, file, oss.str());
}

bool cpu::sysfs::write(const std::string &file, const std::string &buffer) const
{
	return write(basePath, file, buffer);
}

bool cpu::sysfs::write(const std::string &file, const int number) const
{
	return write(basePath, file, number);
}

const std::string cpu::sysfs::read(const std::string &path,
		const std::string &file) const
{
	std::ostringstream oss;
	oss << path << file;
	const std::string absolutePath = oss.str();
	std::string content;
	std::ifstream inputFile;
	inputFile.open(absolutePath.c_str());
	if (!inputFile.is_open()) {
		std::cerr << "Failed to read from file: " << absolutePath
			<< std::endl;
		return std::string();
	}
	std::getline(inputFile, content);
	inputFile.close();
	return content;
}

const std::string cpu::sysfs::read(const std::string &file) const
{
	return read(basePath, file);
}

const std::vector<std::string> cpu::sysfs::readAll(const std::string &file) const
{
	return readAll(basePath, file);
}

const std::vector<std::string> cpu::sysfs::readAll(const std::string &path,
		const std::string &file) const
{
	std::ostringstream oss;
	oss << path << file;
	const std::string absolutePath = oss.str();
	std::ifstream inputFile;
	inputFile.open(absolutePath.c_str());
	if (!inputFile.is_open()) {
		return std::vector<std::string>();
	}
	std::vector<std::string> contents = std::vector<std::string>();
	while (true) {
		std::string content;
		inputFile >> content;
		if (inputFile.eof()) {
			break;
		}
		contents.push_back(content);
	}
	inputFile.close();
	return contents;
}

const std::vector<std::string> cpu::sysfs::readPipe(const char* command,
		const unsigned int number) const
{
	std::FILE *pipe = popen(command, "r");
	if (pipe != NULL) {
		size_t n = 0;
		std::vector<std::string> lines = std::vector<std::string>();
		for (unsigned int i = 0; i < number; ++i) {
			char *line = NULL;
			if (getline(&line, &n, pipe) == -1) {
				pclose(pipe);
				return std::vector<std::string>();
			}
			lines.push_back(std::string(line));
			std::free(line);
		}
		pclose(pipe);
		return lines;
	}
	return std::vector<std::string>();
}

}
