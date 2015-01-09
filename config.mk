# Version
VERSION:=$(shell printf "1.r.$(shell git rev-list --count HEAD).$(shell git rev-parse --short HEAD)")

# Standard
STD:=-std=c99

# includes and libs
INCS:=-I.
LIBS:=

# Allow use of the msr module and wrmsr binary
# to write sepcific bits on the CPU register
# This is needed on some machines to disble Turbo Boost
#
# PLEASE NOTE that setting this option to 1 does not require
# you to have the msr module or wrmsr installed.
# pstate-frequency will check to see if you have these
# and silently skip over if you do not
# 0 OFF
# 1 ON
WRITE_MSR?=0

# Include a bash completion file
# 0 NO
# 1 YES
INCLUDE_BASH_COMPLETION?=1

# Include a zsh completion file
# 0 NO
# 1 YES
INCLUDE_ZSH_COMPLETION?=1

# Include a udev rule which will set the pstate to powersave
# on battery and performance on AC power
# (Generally useful for laptops and mobile machines)
INCLUDE_UDEV_RULE?=1

# Options you may change
# C Compiler to use
CC?=cc

# Prefix of install
PREFIX?=/usr/local

# Configuration director
ETCDIR?=/etc

# Permissions
# Owner executable
# Group read/write
# Other read/write
BIN_PERMISSION=755

# Linker Flags
LDFLAGS+= -Wl,-O3,--sort-common,--as-needed,-z,relro,-s ${LIBS}

# Compiler flags
CFLAGS+= -DWRITE_MSR=${WRITE_MSR} -DVERSION=\"${VERSION}[${CC}]\" -DINCLUDE_UDEV_RULE=${INCLUDE_UDEV_RULE} ${STD} -O3 -Wall -Wextra -Werror -Wmissing-prototypes -Wunreachable-code ${INCS}
