#include "protocol.h"

#include <sys/socket.h>
#include <sys/un.h>


int createLocalSocket(void) {
    return socket(AF_UNIX, SOCK_STREAM, 0);
}

void bindLocalSocket(int fdSocket, char *path) {
    sockaddr_un address;
    strcpy(address.sun_path, path);
    address.sun_family = AF_UNIX;

    bind(fdSocket, (const struct sockaddr *) &address, sizeof(address));
}

int connectLocalSocket(int fdSocket, char *path) {
    sockaddr_un address;
    strcpy(address.sun_path, path);
    address.sun_family = AF_UNIX;

    return connect(fdSocket, (const sockaddr *) &address, sizeof(address));
}

void sendMessage(int fdTargetSocket, Message *msg) {
    send(fdTargetSocket, msg, sizeof(*msg), 0);
}