#include "centralCommittee.h"


static char *const HOME = "HOME";

static char *const RESOLVE_QUEUE_PATH_ERROR = "Error resolving home directory path\n";
static char *const GENERATE_QUEUE_TOKEN_ERROR = "Error generating queue token";
static char *const CREATE_QUEUE_ERROR = "Error creating queue";
static char *const RECEIVE_MSG_ERROR = "Error receiving message";
static char *const SEND_MSG_ERROR = "Error sending message";
static char *const CLOSE_QUEUE_ERROR = "Error closing server queue";


static HashMap *clientQueues;
static bool shutdownFlag;


void initShutdown(MessageContent *msg, ssize_t msgSize) {
    shutdownFlag = true;
}

void registerClient(MessageContent *msg, ssize_t msgSize) {
    Client client;
    client.pid = msg->senderPid;
    client.queueId = *(int *) msg->text;

    HashMap_add(clientQueues, (void *) client.pid, (void *) client.queueId);

    Message response;
    response.mtype = CLIENT;
    response.content.senderPid = getpid();
    memcpy(response.content.text, &msg->senderPid, sizeof(msg->senderPid));
    response.content.text[sizeof(response.content.senderPid)] = '\0';

    if (msgsnd(client.queueId, &response, sizeof(response.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}

void echoService(MessageContent *msg, ssize_t msgSize) {
    Message response;
    response.mtype = ECHO;
    response.content.senderPid = getpid();
    strncpy(response.content.text, msg->text, MAX_MSG_TEXT_SIZE);

    int clientQueueId = (int) HashMap_get(clientQueues, (void *) msg->senderPid);

    if (msgsnd(clientQueueId, &response, sizeof(response.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}

static void mapString(char *string, int (*mapped)(int)) {
    while (*string != '\0') {
        *string = (char) (*mapped)(*string);
        string++;
    }
}

void capitalizeService(MessageContent *msg, ssize_t msgSize) {
    Message response;
    response.mtype = CAPITALIZE;
    response.content.senderPid = getpid();
    strncpy(response.content.text, msg->text, MAX_MSG_TEXT_SIZE);

    mapString(response.content.text, toupper);

    int clientQueueId = (int) HashMap_get(clientQueues, (void *) msg->senderPid);

    if (msgsnd(clientQueueId, &response, sizeof(response.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}

void timeService(MessageContent *msg, ssize_t msgSize) {
    Message response;
    response.mtype = TIME;
    response.content.senderPid = getpid();
    time_t now = time(NULL);
    strncpy(response.content.text, ctime(&now), MAX_MSG_TEXT_SIZE);

    mapString(response.content.text, toupper);

    int clientQueueId = (int) HashMap_get(clientQueues, (void *) msg->senderPid);

    if (msgsnd(clientQueueId, &response, sizeof(response.content), 0) == -1) {
        perror(SEND_MSG_ERROR);
        exit(EXIT_FAILURE);
    }
}


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

static void exit_removeQueue(int ignored, void *pQueueDescriptor) {
    if (msgctl(*(int *) pQueueDescriptor, IPC_RMID, NULL) == -1) {
        perror(CLOSE_QUEUE_ERROR);
    }
    free(pQueueDescriptor);
}

static int createQueueForKey(key_t key) {
    int *queueDescriptor = safe_malloc(sizeof(int));

    *queueDescriptor = msgget(key, IPC_CREAT | 0600);

    if (*queueDescriptor == -1) {
        perror(CREATE_QUEUE_ERROR);
        exit(EXIT_FAILURE);
    }

    on_exit(exit_removeQueue, queueDescriptor);
    return *queueDescriptor;
}

int createServerQueue(void) {
    return createQueueForKey(getQueueKeyForPath(getQueuePath()));
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


void listenForMessages(int queue, HashMap *msgHandlers) {
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
        (*msgHandler)(&msgBuffer.content, msgTextSize);
    }

    HashMap_delete(clientQueues);
}


int main(void) {
    int queue = createServerQueue();
    HashMap *messageHandlers = registerMessageHandlers();

    listenForMessages(queue, messageHandlers);

    HashMap_delete(messageHandlers);
    return 0;
}
