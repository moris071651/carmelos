# make the server AES-PCBC.c and .h into a library and include it in compiling the main file
# it is in /src/server/main.c
#compile it with the library

all: server

server: main.o AES-PCBC.o
	gcc -o server main.o AES-PCBC.o -lcrypto

main.o: src/server/main.c
	gcc -c src/server/main.c

AES-PCBC.o: src/server/AES-PCBC.c src/server/AES-PCBC.h
	gcc -c src/server/AES-PCBC.c

clean:
	rm -f *.o server