CC = gcc

MODE = debug

CFLAGS = -D_POSIX_C_SOURCE -Wall -std=c99
DEBUG_CFLAGS = -g3 -O0
RELEASE_CFLAGS = -O2 -DNDEBUG

CLIENT_LDFLAGS = -lncurses
SERVER_LDFLAGS = -lcrypto -lsqlite3

BUILD_DIRS = build/client build/server bin/
