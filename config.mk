# Version
VERSION=$(shell printf "0.r.$(shell git rev-list --count HEAD).$(shell git rev-parse --short HEAD)")

# Standard
STD=-std=c99

# includes and libs
INCS=-I.
LIBS=

# Debug messages enabled or not
# 0 : OFF
# 1 : ON
DEBUG=0

# Permissions
BIN_PERMISSION=755

# Flags
CFLAGS+= -DDEBUG="${DEBUG}" -DVERSION=\"${VERSION}[${CC}]\" ${STD} -O2 -Wall -Wextra -Werror -Wmissing-prototypes -Wunreachable-code ${INCS}
LDFLAGS+= -Wl,-O2,--sort-common,--as-needed,-z,relro,-s ${LIBS}

# Options you may change
CC?=gcc
PREFIX?=/usr/local
INCLUDE_BASH_COMPLETION?=0
INCLUDE_UDEV_RULE?=0
