# Version
VERSION=0.4.1

# Standard
STD=-std=c99

# Compiler
CC=clang
# CC=gcc

# includes and libs
INCS=-I.
LIBS=

# Permissions
BIN_PERMISSION=755

# Flags
CFLAGS+= -D_GNU_SOURCE=1 -DVERSION=\"${VERSION}[${CC}]\" ${STD} -O2 -Wall -Wextra -Wmissing-prototypes -Wunreachable-code ${INCS}
LDFLAGS+= -Wl,-O2,--sort-common,--as-needed,-z,relro,-s ${LIBS}
