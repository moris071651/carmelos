#include "socket_work.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


void Socke_open_client(Socket *sock, char *filename) {
    struct sockaddr_un addr;
    sock->socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock->socket == -1) {
        perror("ERROR opening socket");
        exit(1);
    }
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filename);
    if (connect(sock->socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("ERROR connecting");
        exit(1);
    }
    sock->connected = true;
}


int main(int argc, char *argv[]) {
    Socket sock;
    Socke_open_client(&sock, "socket");
    while(1){
        if(sock.connected){
            break;
        }
    }
    AllData data;
    data.type = 2;
    strcpy(data.signup.username, "test");
    printf("username: %s\n", data.signup.username);
    strcpy(data.signup.password, "test");
    Socket_Send(&sock, &data);
    Socket_Receive(&sock, &data);
    printf("type: %d\n", data.type);
    printf("login: %s\n", data.response.username);

    //test with a file
    char content[6] = "hello";
    content[5] = '\0';
    AllData *data2 = malloc(sizeof(AllData));
    data2->type = 4;
    data2->newItem.size = 5;
    strcpy(data2->newItem.filename, "test.txt");
    strcpy(data2->newItem.username, "test");
    data2->newItem.timestamp = time(NULL);
    Socket_Send(&sock, data2);
    Socket_SendContent(&sock, content, 5);
    Socket_Receive(&sock, &data);
    printf("type: %d\n", data.type);
    char name[100];
    strcpy(name, data.newItem_response.id);

    //get the file
    AllData *data3 = malloc(sizeof(AllData));
    data3->type = 8;
    strcpy(data3->getItem.id, name);
    data3->getItem.timestamp = data.newItem_response.timestamp;
    Socket_Send(&sock, data3);
    Socket_Receive(&sock, &data);
    char *content2 = malloc(data.getItem_response.size);
    Socket_ReceiveContent(&sock, content2, data.getItem_response.size);
    printf("content: %s\n", content2);
    printf("type: %d\n", data.type);
    Socket_Close(&sock);
    
    

    return 0;
}