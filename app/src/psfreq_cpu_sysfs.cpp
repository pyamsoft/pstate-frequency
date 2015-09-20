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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "psfreq_color.h"
#include "psfreq_cpu.h"
#include "psfreq_log.h"

namespace psfreq {

bool Cpu::Sysfs::write(const std::string &path, const std::string &file,
                const std::string &buffer) const
{
        std::ostringstream oss;
        oss << path << file;
        const std::string absolutePath = oss.str();
        if (Log::isDebug()) {
                std::cout << "[Debug] Attempt to write to file: '"
                          << absolutePath << "'" << std::endl;
        }
        std::ofstream outputFile;
        outputFile.open(absolutePath.c_str());
        if (!outputFile.is_open()) {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to write to file: "
                                << absolutePath
                                << Color::reset() << std::endl;
                }
                return false;
        } else {
                if (Log::isDebug()) {
                        std::cout << "[Debug] Write buffer: '" << buffer
                                  << "' to file: '" << absolutePath
                                  << "'" << std::endl;
                }
                outputFile << buffer << std::endl;
                outputFile.close();
                return true;
        }
}

bool Cpu::Sysfs::write(const std::string &path, const std::string &file,
                const int number) const
{
        std::ostringstream oss;
        oss << number;
        return write(path, file, oss.str());
}

bool Cpu::Sysfs::write(const std::string &file,
                const std::string &buffer) const
{
        return write(basePath, file, buffer);
}

bool Cpu::Sysfs::write(const std::string &file, const int number) const
{
        return write(basePath, file, number);
}

const std::string Cpu::Sysfs::read(const std::string &path,
                const std::string &file) const
{
        std::ostringstream oss;
        oss << path << file;
        const std::string absolutePath = oss.str();
        if (Log::isDebug()) {
                std::cout << "[Debug] Attempt to read from file: '"
                          << absolutePath << "'" << std::endl;
        }
        std::string content;
        std::ifstream inputFile;
        inputFile.open(absolutePath.c_str());
        if (!inputFile.is_open()) {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to read from file: "
                                << absolutePath
                                << Color::reset() << std::endl;
                }
                return Cpu::BAD_READ;
        } else {
                std::getline(inputFile, content);
                inputFile.close();
                if (Log::isDebug()) {
                        std::cout << "[Debug] Read buffer: '" << content
                                << "' from file: '" << absolutePath
                                  << "'" << std::endl;
                }
                return content;
        }
}

const std::string Cpu::Sysfs::read(const std::string &file) const
{
        return read(basePath, file);
}

const std::vector<std::string> Cpu::Sysfs::readAll(
                const std::string &file) const
{
        return readAll(basePath, file);
}

const std::vector<std::string> Cpu::Sysfs::readAll(const std::string &path,
                const std::string &file) const
{
        std::ostringstream oss;
        oss << path << file;
        const std::string absolutePath = oss.str();
        if (Log::isDebug()) {
                std::cout << "[Debug] Attempt to read from file: '"
                          << absolutePath << "'" << std::endl;
        }
        std::ifstream inputFile;
        inputFile.open(absolutePath.c_str());
        if (!inputFile.is_open()) {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to read from file: "
                                << absolutePath
                                << Color::reset() << std::endl;
                }
                return Cpu::BAD_VECTOR;
        } else {
                std::vector<std::string> contents = std::vector<std::string>();
                while (true) {
                        std::string content;
                        inputFile >> content;
                        if (inputFile.eof()) {
                                break;
                        }
                        if (Log::isDebug()) {
                                std::cout << "[Debug] ReadAll buffer: '"
                                        << content << "' from file: '"
                                        << absolutePath << "'" << std::endl;
                        }
                        contents.push_back(content);
                }
                inputFile.close();
                return contents;
        }
}

const std::vector<std::string> Cpu::Sysfs::readPipe(const char* command,
                const unsigned int number) const
{
        if (Log::isDebug()) {
                std::cout << "[Debug] Attempt to read from pipe: '"
                          << command << "'" << std::endl;
        }
        std::FILE *pipe = popen(command, "r");
        if (pipe != NULL) {
                size_t n = 0;
                std::vector<std::string> lines = std::vector<std::string>();
                for (unsigned int i = 0; i < number; ++i) {
                        char *line = NULL;
                        if (getline(&line, &n, pipe) == -1) {
                                if (!Log::isAllQuiet()) {
                                        std::cerr << Color::boldRed()
                                                << "[Error] Failed to read "
                                                << "from pipe[" << i << "]: "
                                                << command << Color::reset()
                                                << std::endl;
                                }
                                pclose(pipe);
                                return Cpu::BAD_VECTOR;
                        }
                        lines.push_back(std::string(line));
                        std::free(line);
                }
                pclose(pipe);
                return lines;
        } else {
                if (!Log::isAllQuiet()) {
                        std::cerr << Color::boldRed()
                                << "[Error] Failed to read from pipe: "
                                << command
                                << Color::reset() << std::endl;
                }
                return Cpu::BAD_VECTOR;
        }
}

}

