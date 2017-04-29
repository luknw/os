//
// Created by luknw on 4/21/17.
//

#ifndef CENTRALLYPLANNEDECONOMY_EXECUTIVEORDERS_H
#define CENTRALLYPLANNEDECONOMY_EXECUTIVEORDERS_H


#include <stdlib.h>

#define MAX_MSG_SIZE 8192
#define MAX_MSG_CONTENT_SIZE (MAX_MSG_SIZE - sizeof(long))
#define MAX_MSG_TEXT_SIZE (MAX_MSG_CONTENT_SIZE - sizeof(pid_t))

static const int PROJ_ID = 'q';
static char *const SERVER_QUEUE_PATH = "/Server";

typedef struct MessageContent MessageContent;
typedef struct Message Message;

typedef void (*MessageHandler)(MessageContent *msg);


typedef enum RequestType {
    SHUTDOWN = 1,
    CLIENT,
    ECHO,
    CAPITALIZE,
    TIME,
} RequestType;

struct MessageContent {
    pid_t senderPid;
    char text[MAX_MSG_TEXT_SIZE];
};

struct Message {
    long mtype;
    union {
        char *mcontent;
        MessageContent content;
    };
};


#endif //CENTRALLYPLANNEDECONOMY_EXECUTIVEORDERS_H
