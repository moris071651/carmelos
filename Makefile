include config.mk

CLIENT_SRC = $(wildcard src/client/*.c)
CLIENT_OBJ = $(patsubst src/client/%.c,build/client/%.o,$(CLIENT_SRC))

SERVER_SRC = $(wildcard src/server/*.c)
SERVER_OBJ = $(patsubst src/server/%.c,build/server/%.o,$(SERVER_SRC))

all: $(NEEDED_DIRS) $(MODE)

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: bin/client.out bin/server.out

release: CFLAGS += $(RELEASE_CFLAGS)
release: bin/client.out bin/server.out

bin/client.out: $(CLIENT_OBJ)
	$(CC) $(CLIENT_LDFLAGS) -o 'bin/client.out' $^

build/client/%.o: src/client/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/client/%.o: src/client/%.c src/client/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

bin/server.out: $(SERVER_OBJ)
	$(CC) $(SERVER_LDFLAGS) -o 'bin/server.out' $^

build/server/%.o: src/server/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/server/%.o: src/server/%.c src/server/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr build/client/*.o build/server/*.o bin/*.out

cleanup:
	rm -fr files socket test data.db

$(BUILD_DIRS):
	mkdir -p $@

.PHONY: all clean
