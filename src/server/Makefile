
all: server client

server: main.c AES.o database.o filestorage.o CRC32.o socket.o
	gcc -o server main.c AES.o database.o filestorage.o CRC32.o socket.o -lsqlite3

AES.o: AES-PCBC.c AES-PCBC.h
	gcc -c AES-PCBC.c -o AES.o

database.o: sqlite.c sqlite.h
	gcc -c sqlite.c -o database.o

filestorage.o: fileStorage.c fileStorage.h
	gcc -c fileStorage.c -o filestorage.o

CRC32.o: CRC.c CRC.h
	gcc -c CRC.c -o CRC32.o

socket.o: socket_work.c socket_work.h
	gcc -c socket_work.c -o socket.o

client: client_socket_test.c socket.o
	gcc -o client client_socket_test.c socket.o



