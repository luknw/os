//
// Created by luknw on 2017-06-06
//

#ifndef HELPDESK_PROTOCOL_H
#define HELPDESK_PROTOCOL_H

#include <stdbool.h>


#define MAX_MESSAGE_DATA_LEN 256
#define MAX_CLIENT_NAME_LEN 32


typedef struct sockaddr sockaddr;
typedef struct sockaddr_un sockaddr_un;


typedef struct JobData JobData;
typedef struct RegisterData RegisterData;
typedef struct Message Message;


typedef enum MessageType {
    OK,
    REGISTER,
    NAME_TAKEN,
    PING,
    PONG,
    JOB,
} MessageType;

struct Message {
    MessageType type;
    char data[MAX_MESSAGE_DATA_LEN];
};


struct RegisterData {
    char name[MAX_CLIENT_NAME_LEN];
};


typedef enum JobType {
    ADD,
    SUB,
    MUL,
    DIV,
} JobType;

struct JobData {
    int id;
    JobType type;
    double left;
    double right;
    bool ok;
    double result;
};


int createLocalSocket(void);

void bindLocalSocket(int fdSocket, char *path);

int connectLocalSocket(int fdSocket, char *path);

void sendMessage(int fdTargetSocket, Message *msg);


#endif //HELPDESK_PROTOCOL_H
