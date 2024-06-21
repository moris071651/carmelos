#include "talk.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef TALK_SOCKET_FILE
#define TALK_SOCKET_FILE "/tmp/carmelos.sock"
#endif

int socketfd = 0;

void destroy_talk(void) {

}

void setup_talk(void) {
    if (access(TALK_SOCKET_FILE, F_OK) != 0) {
        fprintf(stderr, "Server Not Running\n");
        exit(EXIT_FAILURE);
    }
    
    socketfd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, TALK_SOCKET_FILE);

    if (connect(socketfd, (struct sockaddr*) &server_addr, sizeof(server_addr))) {
        fprintf(stderr, "connect failed\n");
        exit(EXIT_FAILURE);
    }
}
