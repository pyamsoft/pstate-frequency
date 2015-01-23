# Version Number
VERSION:=1.0.4

# Standard
# The standard has been decided as C++11 for a couple of reasons. The kernel
# required for using the intel_pstate driver is (atleast) kernel 3.9 C++11 came
# out before kernel 3.9, meaning that if you are able to run kernel 3.9, you
# should hopefully be on a distribution which sports a C++ compiler that
# supports the new C++11 standard.
STD:=-std=c++11

# Include the current directory so that paths for headers are properly
# resolved.
INCS:=-Iproject

# Allow use of the msr module and wrmsr binary to write sepcific bits on the
# CPU register. This is needed on some machines to disble Turbo Boost when
# using the legacy acpi-cpufreq driver.
# 0 NO / 1 YES
#
#	WRITE_MSR flag is not yet implemented
#	WRITE_MSR:=0

# Include a bash completion file
# 0 NO / 1 YES
INCLUDE_BASH_COMPLETION:=1

# Include a zsh completion file
# 0 NO / 1 YES
INCLUDE_ZSH_COMPLETION:=1

# Include a udev rule which will set the pstate to powersave on battery and
# performance on AC power
# 0 NO / 1 YES
INCLUDE_UDEV_RULE:=1

# C++ Compiler to use
# This will default to the environment defined binary if it is specified.
# clang is recommended, but not required.
CXX?=c++

# Prefix of install
PREFIX?=/usr/local

# Configuration directory
ETCDIR?=/etc

# Permissions
BIN_PERMISSION=755

# Linker Flags
LDFLAGS:= -Wl,-O3,--sort-common,--as-needed,-z,relro,-z,now,--strip-all

# Compiler flags
CXXFLAGS:= -DWRITE_MSR=${WRITE_MSR} -DVERSION=\"${VERSION}[${CXX}]\" \
	-DINCLUDE_UDEV_RULE=${INCLUDE_UDEV_RULE} ${STD} -O3 -Wall \
	-march=native -mtune=generic -pipe -fstack-protector-strong \
	--param=ssp-buffer-size=4 \
	-Wextra -Werror -Wpedantic -Wmissing-declarations -Wunreachable-code ${INCS}
