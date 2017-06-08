//
// Created by luknw on 2017-06-08
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include "Metadata.h"

int main(int argc, char *argv[]) {
    int sockfd = 0;
    ssize_t n = 0;
    Metadata meta(0, 0);
    FILE *dataFile = fopen("res/received.bin", "w");
    char dataBuffer[1024];
    struct sockaddr_in serv_addr;

    if (argc != 2) {
        printf("\n Usage: %s <ip of server> \n", argv[0]);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    if ((n = read(sockfd, &meta, sizeof(meta))) > 0) {
        if (n < 0) {
            printf("\n Read error \n");
        }

        std::cout << meta.dataType() << std::endl << meta.dataSize() << std::endl;
        ssize_t expected = meta.dataSize();
        while (expected > 0) {
            ssize_t chunkSize = (expected > sizeof(dataBuffer))
                                ? sizeof(dataBuffer) : (size_t) expected;
            n = recv(sockfd, dataBuffer, (size_t) chunkSize, MSG_WAITALL);
            std::cout << n << std::endl;
            if (n != chunkSize) {
                std::cout << "Chunk receive error" << std::endl;
                exit(EXIT_FAILURE);
            }
            n = fwrite(dataBuffer, 1, (size_t) chunkSize, dataFile);
            if (n != chunkSize) {
                std::cout << "Chunk save error: " << n << std::endl;
                exit(EXIT_FAILURE);
            }
            expected -= n;
        }
    }

    fclose(dataFile);

    return 0;
}
