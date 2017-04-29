#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include "centralCommittee.h"


static char *const HOME = "HOME";

static char *const RESOLVE_QUEUE_PATH_ERROR = "Error resolving home directory path\n";
static char *const GENERATE_QUEUE_TOKEN_ERROR = "Error generating queue token";
static char *const CREATE_QUEUE_ERROR = "Error creating queue";
static char *const RECEIVE_MSG_ERROR = "Error receiving message";
static char *const SEND_MSG_ERROR = "Error sending message";
static char *const CLOSE_QUEUE_ERROR = "Error closing server queue";
static char *const UNLINK_QUEUE_ERROR = "Erorr unlinking queue";
static char *const OPEN_QUEUE_ERROR = "Error opening queue";


static HashMap *clientQueues;
static bool shutdownFlag;


void initShutdown(MessageContent *msg) {
    shutdownFlag = true;
}


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


void registerClient(MessageContent *msg) {
    Client client;
    client.pid = msg->senderPid;
#ifndef POSIX_QUEUES
    client.queueId = *(QueueDescriptor *) msg->text;
#else
    client.queueId = mq_open(msg->text, O_WRONLY);
    if (client.queueId == -1) {
        perror(OPEN_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }
#endif

    HashMap_add(clientQueues, (void *) client.pid, (void *) client.queueId);

    Message response;
    response.mtype = CLIENT;
    response.content.senderPid = getpid();
    memcpy(response.content.text, &msg->senderPid, sizeof(msg->senderPid));
    response.content.text[sizeof(response.content.senderPid)] = '\0';

    sendMessage(client.queueId, &response);
}

void echoService(MessageContent *msg) {
    Message response;
    response.mtype = ECHO;
    response.content.senderPid = getpid();
    strncpy(response.content.text, msg->text, MAX_MSG_TEXT_SIZE - 1);

    QueueDescriptor clientQueueId = (QueueDescriptor) HashMap_get(clientQueues, (void *) msg->senderPid);
    sendMessage(clientQueueId, &response);
}

static void mapString(char *string, int (*mapped)(int)) {
    while (*string != '\0') {
        *string = (char) (*mapped)(*string);
        string++;
    }
}

void capitalizeService(MessageContent *msg) {
    Message response;
    response.mtype = CAPITALIZE;
    response.content.senderPid = getpid();
    strncpy(response.content.text, msg->text, MAX_MSG_TEXT_SIZE - 1);

    mapString(response.content.text, toupper);

    QueueDescriptor clientQueueId = (QueueDescriptor) HashMap_get(clientQueues, (void *) msg->senderPid);
    sendMessage(clientQueueId, &response);
}

void timeService(MessageContent *msg) {
    Message response;
    response.mtype = TIME;
    response.content.senderPid = getpid();
    time_t now = time(NULL);
    strncpy(response.content.text, ctime(&now), MAX_MSG_TEXT_SIZE - 1);

    QueueDescriptor clientQueueId = (QueueDescriptor) HashMap_get(clientQueues, (void *) msg->senderPid);
    sendMessage(clientQueueId, &response);
}


#ifndef POSIX_QUEUES

static char *getQueuePath(void) {
    char *path = getenv(HOME);

    if (path == NULL) {
        fprintf(stderr, RESOLVE_QUEUE_PATH_ERROR);
        exit(EXIT_FAILURE);
    }

    return path;
}


static key_t getQueueKeyForPath(char *queuePath) {
    key_t key = ftok(queuePath, PROJ_ID);

    if (key == -1) {
        perror(GENERATE_QUEUE_TOKEN_ERROR);
        exit(EXIT_FAILURE);
    }

    return key;
}

#else

static char *getQueuePath(void) {
    return SERVER_QUEUE_PATH;
}

#endif

#ifndef POSIX_QUEUES

static void exit_removeQueue(int ignored, void *pQueueDescriptor) {
    if (msgctl(*(QueueDescriptor *) pQueueDescriptor, IPC_RMID, NULL) == -1) {
        perror(CLOSE_QUEUE_ERROR);
    }
    safe_free(pQueueDescriptor);
}

#else

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

#ifndef POSIX_QUEUES

static QueueDescriptor createQueue(key_t key) {
    QueueDescriptor *queueDescriptor = safe_malloc(sizeof(QueueDescriptor));
    *queueDescriptor = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
#else

    static QueueDescriptor createQueue() {
        QueueDescriptor *queueDescriptor = safe_malloc(sizeof(QueueDescriptor));
        *queueDescriptor = mq_open(getQueuePath(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, NULL);
#endif

    if (*queueDescriptor == -1) {
        perror(CREATE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    on_exit(exit_removeQueue, queueDescriptor);
    return *queueDescriptor;
}

QueueDescriptor createServerQueue(void) {
#ifndef POSIX_QUEUES
    key_t key = getQueueKeyForPath(getQueuePath());
    return createQueue(key);
#else
    return createQueue();
#endif
}


static size_t keyPointerCastHash(void *key) {
    return (size_t) key;
}

HashMap *registerMessageHandlers(void) {
    HashMap *msgHandlers = HashMap_new(keyPointerCastHash);

    HashMap_add(msgHandlers, (void *) SHUTDOWN, initShutdown);
    HashMap_add(msgHandlers, (void *) CLIENT, registerClient);
    HashMap_add(msgHandlers, (void *) ECHO, echoService);
    HashMap_add(msgHandlers, (void *) CAPITALIZE, capitalizeService);
    HashMap_add(msgHandlers, (void *) TIME, timeService);

    return msgHandlers;
}


#ifdef POSIX_QUEUES

void listenForMessages(QueueDescriptor queue, HashMap *msgHandlers) {
    Message msgBuffer;
    clientQueues = HashMap_new(keyPointerCastHash);

    shutdownFlag = false;
    while (!shutdownFlag) {
        ssize_t msgTextSize = mq_receive(queue, (char *) &msgBuffer, MAX_MSG_SIZE, NULL);
        if (msgTextSize == -1) {
            perror(RECEIVE_MSG_ERROR);
            exit(EXIT_FAILURE);
        }

        MessageHandler msgHandler = HashMap_get(msgHandlers, (void *) msgBuffer.mtype);
        (*msgHandler)(&msgBuffer.content);
    }

    HashMapEntry *clientQueue = clientQueues->entries;
    while (clientQueue->next != clientQueues->entries) {
        clientQueue = clientQueue->next;
        if (mq_close((QueueDescriptor) (clientQueue->value)) == -1) {
            perror(CLOSE_QUEUE_ERROR);
            exit(EXIT_FAILURE);
        }
    }

    HashMap_delete(clientQueues);
}

#else

void listenForMessages(QueueDescriptor queue, HashMap *msgHandlers) {
    Message msgBuffer;

    clientQueues = HashMap_new(keyPointerCastHash);

    shutdownFlag = false;
    while (!shutdownFlag) {
        ssize_t msgTextSize = msgrcv(queue, &msgBuffer, MAX_MSG_CONTENT_SIZE, 0, MSG_NOERROR);
        if (msgTextSize == -1) {
            perror(RECEIVE_MSG_ERROR);
            exit(EXIT_FAILURE);
        }

        MessageHandler msgHandler = HashMap_get(msgHandlers, (void *) msgBuffer.mtype);
        (*msgHandler)(&msgBuffer.content);
    }

    HashMap_delete(clientQueues);
}

#endif


int main(void) {
    QueueDescriptor queue = createServerQueue();
    HashMap *messageHandlers = registerMessageHandlers();

    listenForMessages(queue, messageHandlers);

    HashMap_delete(messageHandlers);
    return 0;
}
