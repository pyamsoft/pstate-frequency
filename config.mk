##
# Version Number
##
VERSION:=1.1.11.git

##
# Standard
# The standard has been decided as C++11 for a couple of reasons. The kernel
# required for using the intel_pstate driver is (atleast) kernel 3.9 C++11 came
# out before kernel 3.9, meaning that if you are able to run kernel 3.9, you
# should hopefully be on a distribution which sports a C++ compiler that
# supports the new C++11 standard.
#
# This being said, the project has been updated to the point that you should be
# able to compile successfully without needing to declare a C++ standard version
# at all.
##
STD:=-std=c++11

##
# Include the current directory so that paths for headers are properly
# resolved.
##
INCS:=-Iproject

##
# Include a bash completion file
# 0 NO / 1 YES
##
INCLUDE_BASH_COMPLETION:=1

##
# Include a zsh completion file
# 0 NO / 1 YES
##
INCLUDE_ZSH_COMPLETION:=1

##
# Include a udev rule which will set the pstate to powersave on battery and
# performance on AC power
# 0 NO / 1 YES
##
INCLUDE_UDEV_RULE:=1

##
# C++ Compiler to use
# This will default to the GNU C++ compiler, g++
# The project has been tested against g++ and clang++
# both successfully build on all levels of optimizations, though
# -O3 is the default for the most speed and performance benefits.
##
CXX?=g++

##
# Prefix of install
##
PREFIX?=/usr/local

##
# Configuration directory
##
ETCDIR?=/etc

##
# Permissions
##
BIN_PERMISSION=755

##
# Linker Flags
##
LDFLAGS:= -Wl,-O3,--sort-common,--as-needed,-z,relro,-z,now,--strip-all

##
# Compiler flags
##
CXXFLAGS:= -DVERSION=\"${VERSION}[${CXX}]\" \
	-DINCLUDE_UDEV_RULE=${INCLUDE_UDEV_RULE} ${STD} ${INCS} -O3 \
	-march=native -mtune=generic -pipe \
	-Wall -Wextra -Werror -Wpedantic -Wmissing-declarations \
	-Wunreachable-code
