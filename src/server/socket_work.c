#include "socket_work.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>



// needed small functions




// implementation of the functions

void Socket_Init(Socket *sock) {
    sock->connected = false;
}

void Socket_Open(Socket *sock, int port) {
    sock->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->socket < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock->socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(sock->socket, 5) < 0) {
        perror("listen");
        exit(1);
    }

    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);
    sock->socket = accept(sock->socket, (struct sockaddr *)&client, &client_size);
    if(sock->socket < 0) {
        perror("accept");
        exit(1);
    }

    sock->connected = true;
}

void Socket_Close(Socket *sock) {
    close(sock->socket);
    sock->connected = false;
}

void Socket_Wait(Socket *sock) {
    if (!sock->connected) {
        return;
    }

    char buffer[1];
    //wait until something is being sent but should be able to read the same thing later
    // should immediately return if something is being sent
    while (recv(sock->socket, buffer, 1, MSG_PEEK) <= 0) {
        usleep(1000);
    }
}

void Socket_Send(Socket *sock, void *data, size_t size) {
    if (!sock->connected) {
        return;
    }

    if (send(sock->socket, data, size, 0) < 0) {
        perror("send");
        exit(1);
    }
}

void Socket_Receive(Socket *sock, void *data, size_t size) {
    if (!sock->connected) {
        return;
    }

    if (recv(sock->socket, data, size, 0) < 0) {
        perror("recv");
        exit(1);
    }
}

void Socket_SendFile(Socket *sock, File *file) {
    if (!sock->connected) {
        return;
    }

    Socket_Send(sock, file->id, sizeof(file->id));
    Socket_Send(sock, file->filename, sizeof(file->filename));
    Socket_Send(sock, &file->size, sizeof(file->size));
    Socket_Send(sock, file->content, sizeof(file->content));
}

void Socket_ReceiveFile(Socket *sock, File *file) {
    if (!sock->connected) {
        return;
    }

    Socket_Receive(sock, file->id, sizeof(file->id));
    Socket_Receive(sock, file->filename, sizeof(file->filename));
    Socket_Receive(sock, &file->size, sizeof(file->size));
    Socket_Receive(sock, file->content, sizeof(file->content));
}

void Socket_SendResponse(Socket *sock, Response *response) {
    if (!sock->connected) {
        return;
    }

    Socket_Send(sock, &response->success, sizeof(response->success));
    Socket_Send(sock, response->message, sizeof(response->message));
}

void Socket_SendFileMetas(Socket *sock, FileMeta *metas, int count) {
    if (!sock->connected) {
        return;
    }
    Socket_Send(sock, &count, sizeof(count));

    for (int i = 0; i < count; i++) {
        Socket_Send(sock, metas[i].id, sizeof(metas[i].id));
        Socket_Send(sock, metas[i].filename, sizeof(metas[i].filename));
        Socket_Send(sock, &metas[i].timestamp, sizeof(metas[i].timestamp));
    }
}

void Socket_ReceiveFileMeta(Socket *sock, FileMeta *meta) {
    if (!sock->connected) {
        return;
    }

    Socket_Receive(sock, meta->id, sizeof(meta->id));
    Socket_Receive(sock, meta->filename, sizeof(meta->filename));
    Socket_Receive(sock, &meta->timestamp, sizeof(meta->timestamp));
}

void Socket_RecieveTitle(Socket *sock, char *string) {
    if (!sock->connected) {
        return;
    }

    Socket_Receive(sock, string, 256);
}

void Socket_SendTitle(Socket *sock, char *string) {
    if (!sock->connected) {
        return;
    }

    Socket_Send(sock, string, 256);
}