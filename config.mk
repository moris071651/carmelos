CC = gcc

MODE = debug

CFLAGS = -Wall -Wextra
DEBUG_CFLAGS = -g3 -O0
RELEASE_CFLAGS = -O2 -march=native 

CLIENT_LDFLAGS = -lncurses
SERVER_LDFLAGS = -lcrypto

BUILD_DIRS = build/client build/server bin/
