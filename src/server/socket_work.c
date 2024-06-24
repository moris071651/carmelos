#include "socket_work.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <sys/un.h>

void Socket_Init(Socket *sock) {
    sock->connected = false;
}

void Socket_Connect(Socket *sock, char *filename){
    struct sockaddr_un addr;
    sock->socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock->socket == -1) {
        perror("ERROR opening socket");
        exit(1);
    }
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filename);
    if (connect(sock->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("ERROR connecting");
        exit(1);
    }

    sock->connected = true;
}

void Socket_Send(Socket *sock, AllData *data) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    write(sock->socket, data, sizeof(AllData));
}

void Socket_RecieveContent(Socket *sock, char *content, size_t size){
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    read(sock->socket, content, size);
}

void Socket_Recieve(Socket *sock, AllData *data) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    read(sock->socket, data, sizeof(AllData));
}

void Socket_Close(Socket *sock) {
    close(sock->socket);
    sock->connected = false;
}

void Socket_SendContent(Socket *sock, char *content, size_t size){
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    write(sock->socket, content, size);
}

void Socket_SendFile(Socket *sock, FileSocket *file, char *content) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    AllData *data = malloc(sizeof(AllData));
    data->type = 8;
    data->getItem_response = *file;
    Socket_Send(sock, data);
    Socket_SendContent(sock, content, file->size);
}

void Socket_SendResponse(Socket *sock, userResponse *response) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    AllData *data = malloc(sizeof(AllData));
    data->type = 3;
    data->response = *response;
    Socket_Send(sock, data);
}

void Socket_SendFileMeta(Socket *sock, FileMeta_Socket *meta, int type) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    AllData *data = malloc(sizeof(AllData));
    data->type = type;
    data->getItem = *meta;
    Socket_Send(sock, data);
}

void Socket_SendFileMetas(Socket *sock, FileMeta_Socket *metas, int count) {
    if (!sock->connected) {
        perror("ERROR not connected");
        exit(1);
    }
    AllData *data = malloc(sizeof(AllData));
    data->type = 5;
    for (int i = 0; i < count; i++) {
        data->newItem_response = metas[i];
        Socket_Send(sock, data);
    }
}