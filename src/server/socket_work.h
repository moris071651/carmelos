#ifndef SOCKET_WORK_H
#define SOCKET_WORK_H

#include <stdbool.h>
#include <time.h>
#include <unistd.h>



typedef struct {
    int socket;
    bool connected;
} Socket;

// data structures that will be sent over the network
// should be limited to know how much to read on the other side

typedef struct {
    char username[256];
    char password[256];
} User;

typedef struct {
    char id[512];
    char filename[256];
    size_t size;
    char content[1024];
} File;

typedef struct {
    char id[512];
    char filename[256];
    time_t timestamp;
} FileMeta;

typedef struct {
    bool success;
    char message[256];
} Response;

void Socket_Init(Socket *sock);

void Socket_Open(Socket *sock, int port);

void Socket_Close(Socket *sock);

void Socket_Send(Socket *sock, void *data, size_t size);

void Socket_Receive(Socket *sock, void *data, size_t size);

void Socket_SendFile(Socket *sock, File *file);

void Socket_ReceiveFile(Socket *sock, File *file);

void Socket_SendResponse(Socket *sock, Response *response);

void Socket_SendFileMetas(Socket *sock, FileMeta *metas, int count);

void Socket_ReceiveFileMeta(Socket *sock, FileMeta *meta);

void Socket_RecieveTitle(Socket *sock, char *string);

void Socket_Wait(Socket *sock);

void Socket_SendTitle(Socket *sock, char *string);


#endif

