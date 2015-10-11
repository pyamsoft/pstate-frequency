##
# Version Number
##
VERSION:=2.0.0

##
# C Standard
#
# The standard is implicitly set to C11 in the newest GCC 5. To have support
# for the widest range of machines, we compile with the same standard as
# Linux, explicitly setting to gnu89. Due to the current nature of the project
# the standard is no longer a configurable value.
##

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
# 0 NO | [1 YES]
##
INCLUDE_UDEV_RULE?=1

##
# Preset power plan values
#
#	cpu_max cpu_min cpu_turbo cpu_governor
#
# cpu_max: Number between 0 (min) and 100 (max)
# cpu_min: Number between 0 (min) and 100 (max)
# cpu_turbo: IF using intel_pstate, 1 (OFF) or 0 (ON).
#	     IF using acpi-cpufreq, 0 (OFF) or 1 (ON).
# cpu_governor: Name of governor.
#
# Example:
#   PRESET_POWER_PLAN_MAX_PERFORMANCE="100 100 0 performance"
#   PRESET_POWER_PLAN_PERFORMANCE="100 0 1 powersave"
#   PRESET_POWER_PLAN_POWERSAVE="0 0 1 powersave"
#
PRESET_POWER_PLAN_MAX_PERFORMANCE="100 100 0 performance"
PRESET_POWER_PLAN_PERFORMANCE="100 0 1 powersave"
PRESET_POWER_PLAN_POWERSAVE="0 0 1 powersave"

##
# Runs the specified power plans on AC and BAT
#
# AUTO_POWER_PLAN_AC
#    1 powersave | [2 performance] | 3 max-performance
# AUTO_POWER_PLAN_BAT
#    [1 powersave] | 2 performance | 3 max-performance
##
AUTO_POWER_PLAN_AC="performance"
AUTO_POWER_PLAN_BAT="powersave"

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
# Install copy of the GPLv2 License
#
# Installs a copy of the GPLv2 License
# $(PREFIX)/share/doc/pstate-frequency/project
#
# 0 NO | [1 YES]
##
INCLUDE_LICENSE?=1

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
CC?=gcc

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
CFLAGS:= -std=c90 -O2 \
	-march=native -pipe \
	-Wall -Wextra -Werror -Wpedantic -Wmissing-declarations \
	-Wunreachable-code $(CFLAGS)

##
# Add the VERSION to the CFLAGS if it is defined
# (NOTE) VERSION should be defined for a sane build!
##
ifdef VERSION
	CFLAGS+= -DVERSION=\"$(VERSION)[$(CC)]\"
endif

##
# Add the AUTO_POWER_PLAN_AC to the CFLAGS if it is defined
##
ifdef AUTO_POWER_PLAN_AC
	CFLAGS+= -DAUTO_POWER_PLAN_AC=\"$(AUTO_POWER_PLAN_AC)\"
endif

##
# Add the AUTO_POWER_PLAN_BAT to the CFLAGS if it is defined
##
ifdef AUTO_POWER_PLAN_BAT
	CFLAGS+= -DAUTO_POWER_PLAN_BAT=\"$(AUTO_POWER_PLAN_BAT)\"
endif

##
# Add the PRESET_POWER_PLAN_MAX_PERFORMANCE to the CFLAGS if it is defined
##
ifdef PRESET_POWER_PLAN_MAX_PERFORMANCE
	CFLAGS+= -DPRESET_POWER_PLAN_MAX_PERFORMANCE=\"$(PRESET_POWER_PLAN_MAX_PERFORMANCE)\"
endif

##
# Add the PRESET_POWER_PLAN_PERFORMANCE to the CFLAGS if it is defined
##
ifdef PRESET_POWER_PLAN_PERFORMANCE
	CFLAGS+= -DPRESET_POWER_PLAN_PERFORMANCE=\"$(PRESET_POWER_PLAN_PERFORMANCE)\"
endif

##
# Add the PRESET_POWER_PLAN_POWERSAVE to the CFLAGS if it is defined
##
ifdef PRESET_POWER_PLAN_POWERSAVE
	CFLAGS+= -DPRESET_POWER_PLAN_POWERSAVE=\"$(PRESET_POWER_PLAN_POWERSAVE)\"
endif

##
# Add the INCLUDE_UDEV_RULE to the CFLAGS if it is defined
# (NOTE) As of version 1.2.10, the INCLUDE_UDEV_RULE does NOT
# affect the ability of pstate-frequency to use the automatic power plan.
##
ifeq ($(INCLUDE_UDEV_RULE), 1)
	CFLAGS+= -DINCLUDE_UDEV_RULE=$(INCLUDE_UDEV_RULE)
endif

##
# Make Flags
#
# Do NOT modify these unless you know what you are doing
##
MAKEFLAGS:= $(MAKEFLAGS) --no-print-directory
