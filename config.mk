##
# Version Number
##
VERSION:=1.2.9

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
# Include a bash completion file
# 0 NO / 1 YES
##
INCLUDE_BASH_COMPLETION?=1

##
# Include a zsh completion file
# 0 NO / 1 YES
##
INCLUDE_ZSH_COMPLETION?=0

##
# Include a systemd unit
# 0 NO / 1 YES
#
# Enable this unit only if you have also enabled to udev rule, as the
# unit will issue calls to the automatic power plan, which is only enabled
# when compiled with the udev rule.
##
INCLUDE_SYSTEMD_UNIT?=1

##
# Modify the x86_energy_perf_policy systemd rule to set the policy to
# powersave instead of normal
# 0 NO / 1 YES
##
X86_PERF_POLICY_POWERSAVE?=0

##
# Include a udev rule which will set the pstate to powersave on battery and
# performance on AC power
# 0 NO / 1 YES
##
INCLUDE_UDEV_RULE?=0

##
# Installs a copy of the README.md file to
# $(PREFIX)/share/doc/pstate-frequency/README.md
##
INCLUDE_DOC?=1

##
# Installs a copy of the C++ related source code to
# $(PREFIX)/share/doc/pstate-frequency/project
##
INCLUDE_SRC?=1

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
#
# Understand that this prefix will only affect the destination of the
# installed pstate-frequency binary and the installed documentation
#
# The bash-completion, zsh-completion, udev rule, and systemd unit files
# expect to be installed into the /usr directory, and may not work if they
# are installed anywhere other than the /usr, or /etc directories
##
PREFIX?=/usr/local

##
# Linker Flags
##
LDFLAGS:= -Wl,-O2,--sort-common,--as-needed,-z,relro,-z,now,--strip-all \
	 $(LDFLAGS)

##
# Compiler flags
##
CXXFLAGS:= $(STD) -O2 \
	-march=native -pipe \
	-Wall -Wextra -Werror -Wpedantic -Wmissing-declarations \
	-Wunreachable-code $(CXXFLAGS)

ifdef VERSION
	CXXFLAGS+= -DVERSION=\"$(VERSION)[$(CXX)]\"
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	CXXFLAGS+= -DINCLUDE_UDEV_RULE=$(INCLUDE_UDEV_RULE)
endif

##
# Make Flags
##
MAKEFLAGS:= $(MAKEFLAGS)
