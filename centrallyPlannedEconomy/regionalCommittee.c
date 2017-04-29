//
// Created by luknw on 2017-04-23
//


#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>

#include "libsafe/safe.h"

#include "executiveOrders.h"
#include "centralCommittee.h"


static char *const HOME = "HOME";

static char *const SERVER_NAME = "Server";
static char *const CLIENT_NAME = "Client";

static char *const CREATE_QUEUE_ERROR = "Error creating queue";
static char *const OPEN_QUEUE_ERROR = "Error opening queue";
static char *const CLOSE_QUEUE_ERROR = "Error closing server queue";
static char *const SEND_MSG_ERROR = "Error sending message";
static char *const RECEIVE_MSG_ERROR = "Error receiving message";
static char *const GENERATE_QUEUE_TOKEN_ERROR = "Error generating queue token";
static char *const REGISTER_CLEANUP_HANDLER_ERROR = "Error registering cleanup handler";
static char *const UNLINK_QUEUE_ERROR = "Error unlinking queue";
static char *const RESOLVE_HOME_DIRECTORY_ERROR = "Error resolving home directory path";


static char *const REQUEST_CHOICE_MSG =
        "Choose request:\n"
                "\te: echo\n"
                "\tc: capitalize\n"
                "\tt: time\n"
                "\ts: server shutdown";

static char *const CONTENT_PROMPT = "Enter message content";
static char *const SERVER_TERMINATED = "Server terminated";
static const int QUEUE_PATH_BUFFER_SIZE = 20;

static char *lineBuffer;
static ssize_t lineBufferSize;

Message msgBuffer;

static QueueDescriptor clientQueue;
static char *clientQueuePath;

#ifndef POSIX_QUEUES
static QueueDescriptor serverQueue;
#endif


#ifndef POSIX_QUEUES

static void exit_removeQueue(int ignored, void *pQueueDescriptor) {
    if (msgctl(*(int *) pQueueDescriptor, IPC_RMID, NULL) == -1) {
        perror(CLOSE_QUEUE_ERROR);
    }
    free(pQueueDescriptor);
}

#else

static void exit_freePathBuffer(void) {
    free(clientQueuePath);
}

static char *getQueuePath(void) {
    if (clientQueuePath != NULL) return clientQueuePath;

    clientQueuePath = safe_malloc(QUEUE_PATH_BUFFER_SIZE);
    if (atexit(exit_freePathBuffer) != 0) {
        perror(REGISTER_CLEANUP_HANDLER_ERROR);
        exit(EXIT_FAILURE);
    }

    int queuePathSeed = getpid();

    snprintf(clientQueuePath, QUEUE_PATH_BUFFER_SIZE, "/Client_%d", queuePathSeed);
    return clientQueuePath;
}

static void exit_removeQueue(int ignored, void *pQueueDescriptor) {
    if (mq_close(*(QueueDescriptor *) pQueueDescriptor) == -1) {
        perror(CLOSE_QUEUE_ERROR);
    }
    safe_free(pQueueDescriptor);

    if (mq_unlink(getQueuePath()) == -1) {
        perror(UNLINK_QUEUE_ERROR);
    }
}

#endif

int createClientQueue(void) {
    QueueDescriptor *queueDescriptor = safe_malloc(sizeof(QueueDescriptor));

#ifndef POSIX_QUEUES
    *queueDescriptor = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
#else
    *queueDescriptor = mq_open(getQueuePath(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, NULL);
#endif

    if (*queueDescriptor == -1) {
        perror(CREATE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    on_exit(exit_removeQueue, queueDescriptor);
    return *queueDescriptor;
}

#ifndef POSIX_QUEUES

static char *getServerQueuePath(void) {
    char *path = getenv(HOME);

    if (path == NULL) {
        perror(RESOLVE_HOME_DIRECTORY_ERROR);
        exit(EXIT_FAILURE);
    }

    return path;
}

#else

static char *getServerQueuePath(void) {
    return SERVER_QUEUE_PATH;
}

#endif

#ifndef POSIX_QUEUES

static key_t getServerQueueKeyForPath(char *queuePath) {
    key_t key = ftok(queuePath, PROJ_ID);

    if (key == -1) {
        perror(GENERATE_QUEUE_TOKEN_ERROR);
        exit(EXIT_FAILURE);
    }

    return key;
}

#endif

static void sendMessage(QueueDescriptor queue, Message *message) {
#ifndef POSIX_QUEUES
    if (msgsnd(queue, message, sizeof((*message).content), 0) == -1) {
#else
        if (mq_send(queue, (const char *) message, MAX_MSG_SIZE, 0) == -1) {
#endif
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}

#ifndef POSIX_QUEUES

void registerWithServer(void) {
    Message request;
    request.mtype = CLIENT;
    request.content.senderPid = getpid();
    memcpy(request.content.text, &clientQueue, sizeof(clientQueue));
    request.content.text[sizeof(clientQueue)] = '\0';

    sendMessage(serverQueue, &request);

    Message msgBuffer;
    ssize_t msgTextSize = msgrcv(clientQueue, &msgBuffer, MAX_MSG_CONTENT_SIZE, 0, MSG_NOERROR);
    if (msgTextSize == -1) {
        perror(RECEIVE_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    printf("%s: ClientID = %d\n", SERVER_NAME, *(int *) msgBuffer.content.text);
}


static void checkServerStatus(void) {
    int err = errno;
    if (msgget(getServerQueueKeyForPath(getServerQueuePath()), 0) == -1 && errno == ENOENT) {
        printf("%s\n", SERVER_TERMINATED);
        exit(EXIT_SUCCESS);
    }
    errno = err;
}

#endif

#ifdef POSIX_QUEUES

static void checkServerStatus(void) {
    int err = errno;

    QueueDescriptor qd;
    if ((qd = mq_open(getServerQueuePath(), O_WRONLY)) == -1) {
        if (errno == ENOENT) {
            printf("%s\n", SERVER_TERMINATED);
            exit(EXIT_SUCCESS);
        }
        perror(OPEN_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }
    if (mq_close(qd) == -1) {
        perror(CLOSE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    errno = err;
}

#endif

QueueDescriptor openServerQueue(void) {
#ifndef POSIX_QUEUES
    QueueDescriptor queueDescriptor = msgget(getServerQueueKeyForPath(getServerQueuePath()), 0);
#else
    checkServerStatus();
    QueueDescriptor queueDescriptor = mq_open(getServerQueuePath(), O_WRONLY);
#endif

    if (queueDescriptor == -1) {
        perror(OPEN_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    return queueDescriptor;
}

#ifdef POSIX_QUEUES

void registerWithServer(void) {
    msgBuffer.mtype = CLIENT;
    msgBuffer.content.senderPid = getpid();
    strncpy(msgBuffer.content.text, clientQueuePath, MAX_MSG_TEXT_SIZE - 1);

    QueueDescriptor serverQueue = openServerQueue();

    sendMessage(serverQueue, &msgBuffer);
    ssize_t msgTextSize = mq_receive(clientQueue, (char *) &msgBuffer, MAX_MSG_SIZE, NULL);

    if (mq_close(serverQueue) == -1) {
        perror(CLOSE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    if (msgTextSize == -1) {
        checkServerStatus();
        perror(RECEIVE_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    printf("%s: ClientID = %d\n", SERVER_NAME, *(int *) msgBuffer.content.text);
}

#endif


static void shutdownRequest(void) {
    msgBuffer.mtype = SHUTDOWN;
    msgBuffer.content.senderPid = getpid();

#ifdef POSIX_QUEUES
    QueueDescriptor serverQueue = openServerQueue();
#endif
    sendMessage(serverQueue, &msgBuffer);
#ifdef POSIX_QUEUES
    if (mq_close(serverQueue) == -1) {
        perror(CLOSE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }
#endif
}

static void waitForServerResponse(void) {
#ifndef POSIX_QUEUES
    ssize_t msgTextSize = msgrcv(clientQueue, &msgBuffer, MAX_MSG_CONTENT_SIZE, 0, MSG_NOERROR);
#else
    ssize_t msgTextSize = mq_receive(clientQueue, (char *) &msgBuffer, MAX_MSG_SIZE, NULL);
#endif
    if (msgTextSize == -1) {
        perror(RECEIVE_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

    printf("%s: %s\n", SERVER_NAME, msgBuffer.content.text);
}

static void sendStringRequest(RequestType type) {
    printf("%s\n%s_%d: ", CONTENT_PROMPT, CLIENT_NAME, getpid());
    if (safe_getline_content(&lineBuffer, &lineBufferSize, stdin) == EOF) return;

    msgBuffer.mtype = type;
    msgBuffer.content.senderPid = getpid();
    strncpy(msgBuffer.content.text, lineBuffer, MAX_MSG_TEXT_SIZE);

#ifndef POSIX_QUEUES
    if (msgsnd(serverQueue, &msgBuffer, sizeof(msgBuffer.content), 0) == -1) {
#else
        QueueDescriptor serverQueue = openServerQueue();
        if (mq_send(serverQueue, (const char *) &msgBuffer, MAX_MSG_SIZE, 0) == -1) {
            if (mq_close(serverQueue) == -1) {
                perror(CLOSE_QUEUE_ERROR);
                exit(EXIT_FAILURE);
            }
#endif
        checkServerStatus();
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

#ifdef POSIX_QUEUES
    if (mq_close(serverQueue) == -1) {
        perror(CLOSE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }
#endif

    waitForServerResponse();
}

static void sendRequest(RequestType type) {
    msgBuffer.mtype = type;
    msgBuffer.content.senderPid = getpid();

#ifndef POSIX_QUEUES
    if (msgsnd(serverQueue, &msgBuffer, sizeof(msgBuffer.content), 0) == -1) {
#else
        QueueDescriptor serverQueue = openServerQueue();
        if (mq_send(serverQueue, (const char *) &msgBuffer, MAX_MSG_SIZE, 0) == -1) {
            if (mq_close(serverQueue) == -1) {
                perror(CLOSE_QUEUE_ERROR);
                exit(EXIT_FAILURE);
            }
#endif
        checkServerStatus();
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }

#ifdef POSIX_QUEUES
    if (mq_close(serverQueue) == -1) {
        perror(CLOSE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }
#endif

    waitForServerResponse();
}

void requestLoop(void) {
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
    clientQueuePath = NULL;
    clientQueue = createClientQueue();
#ifndef POSIX_QUEUES
    serverQueue = openServerQueue();
#endif

    registerWithServer();
    requestLoop();

    return 0;
}
