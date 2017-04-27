//
// Created by luknw on 2017-04-23
//


#include <sys/msg.h>

#include "libsafe/safe.h"

#include "executiveOrders.h"


static char *const HOME = "HOME";
static char *const SERVER_NAME = "Server";
static char *const CLIENT_NAME = "Client";

static char *const RESOLVE_QUEUE_PATH_ERROR = "Error resolving home directory path\n";
static char *const CREATE_QUEUE_ERROR = "Error creating queue";
static char *const CLOSE_QUEUE_ERROR = "Error closing server queue";
static char *const SEND_MSG_ERROR = "Error sending message";
static char *const RECEIVE_MSG_ERROR = "Error receiving message";
static char *const GENERATE_QUEUE_TOKEN_ERROR = "Error generating queue token";

static char *const REQUEST_CHOICE_MSG =
        "Choose request:\n"
                "\te: echo\n"
                "\tc: capitalize\n"
                "\tt: time\n"
                "\ts: server shutdown";

static char *const CONTENT_PROMPT = "Enter message content";
static char *const SERVER_TERMINATED = "Server terminated";
static char *lineBuffer;
static ssize_t lineBufferSize;

static int clientQueue;
static int serverQueue;


static void exit_removeQueue(int ignored, void *pQueueDescriptor) {
    if (msgctl(*(int *) pQueueDescriptor, IPC_RMID, NULL) == -1) {
        perror(CLOSE_QUEUE_ERROR);
    }
    free(pQueueDescriptor);
}

int createClientQueue() {
    int *queueDescriptor = safe_malloc(sizeof(int));

    *queueDescriptor = msgget(IPC_PRIVATE, 0600);

    if (*queueDescriptor == -1) {
        perror(CREATE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    on_exit(exit_removeQueue, queueDescriptor);
    return *queueDescriptor;
}


static char *getServerQueuePath(void) {
    char *path = getenv(HOME);

    if (path == NULL) {
        fprintf(stderr, RESOLVE_QUEUE_PATH_ERROR);
        exit(EXIT_FAILURE);
    }

    return path;
}

static key_t getServerQueueKeyForPath(char *queuePath) {
    key_t key = ftok(queuePath, PROJ_ID);

    if (key == -1) {
        perror(GENERATE_QUEUE_TOKEN_ERROR);
        exit(EXIT_FAILURE);
    }

    return key;
}

int getServerQueue() {
    int queueDescriptor = msgget(getServerQueueKeyForPath(getServerQueuePath()), 0);

    if (queueDescriptor == -1) {
        perror(CREATE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    return queueDescriptor;
}


void registerWithServer(void) {
    Message request;
    request.mtype = CLIENT;
    request.content.senderPid = getpid();
    memcpy(request.content.text, &clientQueue, sizeof(clientQueue));
    request.content.text[sizeof(clientQueue)] = '\0';

    if (msgsnd(serverQueue, &request, sizeof(request.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    Message msgBuffer;
    ssize_t msgTextSize = msgrcv(clientQueue, &msgBuffer, MAX_MSG_CONTENT_SIZE, 0, MSG_NOERROR);
    if (msgTextSize == -1) {
        perror(RECEIVE_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    printf("%s: ClientID = %d\n", SERVER_NAME, *(int *) msgBuffer.content.text);
}


static void shutdownRequest(void) {
    Message request;
    request.mtype = SHUTDOWN;
    request.content.senderPid = getpid();

    if (msgsnd(serverQueue, &request, sizeof(request.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}

static void waitForServerResponse(void) {
    Message msgBuffer;
    ssize_t msgTextSize = msgrcv(clientQueue, &msgBuffer, MAX_MSG_CONTENT_SIZE, 0, MSG_NOERROR);
    if (msgTextSize == -1) {
        perror(RECEIVE_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    printf("%s: %s\n", SERVER_NAME, msgBuffer.content.text);
}

static void checkServerStatus(void) {
    int err = errno;
    if (msgget(getServerQueueKeyForPath(getServerQueuePath()), 0) == -1 && errno == ENOENT) {
        printf("%s\n", SERVER_TERMINATED);
        exit(EXIT_SUCCESS);
    }
    errno = err;
}

static void sendStringRequest(RequestType type) {
    printf("%s\n%s_%d: ", CONTENT_PROMPT, CLIENT_NAME, getpid());
    if (safe_getline_content(&lineBuffer, &lineBufferSize, stdin) == EOF) return;

    Message request;
    request.mtype = type;
    request.content.senderPid = getpid();
    strncpy(request.content.text, lineBuffer, MAX_MSG_TEXT_SIZE);

    if (msgsnd(serverQueue, &request, sizeof(request.content), 0) == -1) {
        checkServerStatus();
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    waitForServerResponse();
}

static void sendRequest(RequestType type) {
    Message request;
    request.mtype = type;
    request.content.senderPid = getpid();

    if (msgsnd(serverQueue, &request, sizeof(request.content), 0) == -1) {
        checkServerStatus();
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    waitForServerResponse();
}

void requestLoop() {
    lineBuffer = NULL;
    lineBufferSize = 0;
    char action;

    printf("%s\n%s_%d: ", REQUEST_CHOICE_MSG, CLIENT_NAME, getpid());
    while (safe_getline_content(&lineBuffer, &lineBufferSize, stdin) != EOF) {
        sscanf(lineBuffer, " %c", &action);
        switch (action) {
            case 'e':
                sendStringRequest(ECHO);
                break;
            case 'c':
                sendStringRequest(CAPITALIZE);
                break;
            case 't':
                sendRequest(TIME);
                break;
            case 's':
                shutdownRequest();
                exit(EXIT_SUCCESS);
            default:
                break;
        }
        printf("%s\n%s_%d: ", REQUEST_CHOICE_MSG, CLIENT_NAME, getpid());
        safe_fflush(stdout);
    }
}


int main(void) {
    clientQueue = createClientQueue();
    serverQueue = getServerQueue();

    registerWithServer();
    requestLoop();

    return 0;
}
