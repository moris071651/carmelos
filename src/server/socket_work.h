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

#include "socketTypes.h"



void Socket_Init(Socket *sock);

void Socket_Open(Socket *sock, char *filename);

void Socket_Close(Socket *sock);

void Socket_Send(Socket *sock, AllData *data);

void Socket_Receive(Socket *sock, AllData *data);

void Socket_SendFile(Socket *sock, FileSocket *file, char *content);

void Socket_SendResponse(Socket *sock, userResponse *response);

void Socket_SendFileMeta(Socket *sock, FileMeta_Socket *meta, int type);

void Socket_SendFileMetas(Socket *sock, FileMeta_Socket *metas, int count);

void Socket_ReceiveContent(Socket *sock, char *content, size_t size);

void Socket_SendContent(Socket *sock, char *content, size_t size);


#endif

