//
// Created by luknw on 2017-06-08
//

#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "Metadata.h"

int main(int argc, char *argv[]) {
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    FILE *dataFile = fopen("res/data.bin", "rb");
    char dataBuffer[1024];

    fseek(dataFile, 0, SEEK_END);
    ssize_t dataSize = ftell(dataFile);
    rewind(dataFile);
    std::cout << dataSize << std::endl;

    Metadata meta(666, dataSize);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    listen(listenfd, SOMAXCONN);

    bool ever = true;
    while (ever) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

        std::cout << "." << std::flush;

        write(connfd, &meta, sizeof(Metadata));

        ssize_t dataLeft = dataSize;
        while (dataLeft > 0) {
            size_t chunkSize = (dataLeft > sizeof(dataBuffer))
                               ? sizeof(dataBuffer) : dataLeft;
            fread(dataBuffer, chunkSize, 1, dataFile);
            write(connfd, dataBuffer, chunkSize);
            dataLeft -= chunkSize;
        }

        rewind(dataFile);
        close(connfd);
        sleep(1);
    }

    fclose(dataFile);
}
