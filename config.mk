##
# Version Number
##
VERSION:=1.2.12

##
# C++ Standard
#
# The standard has been decided as C++11 for a couple of reasons. The kernel
# required for using the intel_pstate driver is (atleast) kernel 3.9 C++11 came
# out before kernel 3.9, meaning that if you are able to run kernel 3.9, you
# should hopefully be on a distribution which sports a C++ compiler that
# supports the new C++11 standard.
#
# This being said, the project has been updated to the point that you should be
# able to compile successfully without needing to declare a C++ standard version
# at all.
#
# [c++11] | [NONE] | OTHER
##
STD:=-std=c++11

##
# bash-completion
#
# Include a bash completion file
#
# 0 NO | [1 YES]
##
INCLUDE_BASH_COMPLETION?=1

##
# zsh-completion
#
# Include a zsh completion file
#
# [0 NO] | 1 YES
##
INCLUDE_ZSH_COMPLETION?=0

##
# systemd services
#
# Include a systemd unit
#
# 0 NO | [1 YES]
##
INCLUDE_SYSTEMD_UNIT?=1

##
# x86_energy_perf_policy policies
#
# Modify the x86_energy_perf_policy systemd rule to set the policy to
# a given preset
#
# see X86_ENERGY_PERF_POLICY(8) for details
#
# 0 POWERSAVE | [1 NORMAL] | 2 PERFORMANCE
##
X86_ENERGY_PERF_POLICY?=1

##
# udev rules
#
# Include a udev rule which will set the pstate to powersave on battery and
# performance on AC power
#
# [0 NO] | 1 YES
##
INCLUDE_UDEV_RULE?=0

##
# Runs the specified power plans on AC and BAT
#
# POWER_PLAN_AC
#    1 powersave | [2 performance] | 3 max-performance
# POWER_PLAN_BAT
#    [1 powersave] | 2 performance | 3 max-performance
##
POWER_PLAN_AC=performance
POWER_PLAN_BAT=powersave

##
# Install README.md
#
# Installs a copy of the README.md file to
# $(PREFIX)/share/doc/pstate-frequency/README.md
#
# 0 NO | [1 YES]
##
INCLUDE_DOC?=1

##
# Install copy of source code
#
# Installs a copy of the C++ related source code to
# $(PREFIX)/share/doc/pstate-frequency/project
#
# 0 NO | [1 YES]
##
INCLUDE_SRC?=1

##
# C++ Compiler
#
# This will default to the GNU C++ compiler, g++
# The project has been tested against g++ and clang++
# both successfully build on all levels of optimizations, though
# -O2 is the default for the balance between speed and stability
#
# [g++] | clang++ | OTHER
##
CXX?=g++

##
# Install prefix
#
# Understand that this prefix will only affect the destination of the
# installed pstate-frequency binary and the installed documentation
#
# The bash-completion, zsh-completion, udev rule, and systemd unit files
# expect to be installed into the /usr directory, and may not work if they
# are installed anywhere other than the /usr directory.
#
# [/usr/local] | /usr | other
##
PREFIX?=/usr/local

##
# Linker Flags
#
# Do NOT modify these unless you know what you are doing
##
LDFLAGS:= -Wl,-O2,--sort-common,--as-needed,-z,relro,-z,now,--strip-all \
	 $(LDFLAGS)

##
# Compiler flags
#
# Do NOT modify these unless you know what you are doing
##
CXXFLAGS:= $(STD) -O2 \
	-march=native -pipe \
	-Wall -Wextra -Werror -Wpedantic -Wmissing-declarations \
	-Wunreachable-code $(CXXFLAGS)

##
# Add the VERSION to the CXXFLAGS if it is defined
# (NOTE) VERSION should be defined for a sane build!
##
ifdef VERSION
	CXXFLAGS+= -DVERSION=\"$(VERSION)[$(CXX)]\"
endif

##
# Add the POWER_PLAN_AC to the CXXFLAGS if it is defined
##
ifdef POWER_PLAN_AC
	CXXFLAGS+= -DPOWER_PLAN_AC=\"$(POWER_PLAN_AC)\"
endif

##
# Add the POWER_PLAN_BAT to the CXXFLAGS if it is defined
##
ifdef POWER_PLAN_BAT
	CXXFLAGS+= -DPOWER_PLAN_BAT=\"$(POWER_PLAN_BAT)\"
endif

##
# Add the INCLUDE_UDEV_RULE to the CXXFLAGS if it is defined
# (NOTE) As of version 1.2.10, the INCLUDE_UDEV_RULE does NOT
# affect the ability of pstate-frequency to use the automatic power plan.
##
ifeq ($(INCLUDE_UDEV_RULE), 1)
	CXXFLAGS+= -DINCLUDE_UDEV_RULE=$(INCLUDE_UDEV_RULE)
endif

##
# Make Flags
#
# Do NOT modify these unless you know what you are doing
##
MAKEFLAGS:= $(MAKEFLAGS)
