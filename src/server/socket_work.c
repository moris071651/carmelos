#include "socket_work.h"



// needed small functions

// static void canRead(Socket *socket, bool *canRead) {
//     fd_set readfds;
//     FD_ZERO(&readfds);
//     FD_SET(socket->socket, &readfds);
//     struct timeval timeout = {0, 0};
//     *canRead = select(socket->socket + 1, &readfds, NULL, NULL, &timeout) > 0;
// }










void Socket_Init(Socket *socket) {
    socket->connected = false;
}