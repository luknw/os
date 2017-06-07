#include "clerk.h"
#include "libhashmap/hashMap.h"
#include "libarrayqueue/arrayQueue.h"


static char *const USAGE = "USAGE: clerk PORT_NUMBER SOCKET_PATH";

static const struct timeval DEFAULT_TIMEOUT = {100, 0};

static char *localSocketPath;
static bool shutdownFlag;


static int fdMax;
static fd_set workerFdSet;
static HashMap *workers;
static ArrayQueue *workerJobQueue;
static pthread_mutex_t workerMutex = PTHREAD_MUTEX_INITIALIZER;


static size_t valueHashcode(void *val) {
    return (size_t) val;
}

/*
void *pingWorkers(void *ignored) {
    pthread_detach(pthread_self());

    clientResponsive = HashMap_new(valueHashcode);

    while (!shutdownFlag) {
        pthread_mutex_lock(&workerMutex);
        {
            Message ping = {PING};
            for (int fdClient = 0; fdClient < fdMax; ++fdClient) {
                if (!FD_ISSET(fdClient, &workers)) continue;

                MLOG("Ping %d", fdClient);
                sendMessage(fdClient, &ping);
            }
        }
        pthread_mutex_unlock(&workerMutex);

        sleep(1);

        pthread_mutex_lock(&workerMutex);
        {
            fd_set readSet = set;
            struct timeval timeout = savedTimeout;
            while (select(fdServer + 1, &readSet, NULL, NULL, &timeout) > 0) {
                int fdNewClient = accept(fdServer, NULL, 0);
                FD_SET(fdNewClient, &workers);
                if (fdNewClient > fdMax) fdMax = fdNewClient;

                readSet = set;
                timeout = savedTimeout;
            }


        }
        pthread_mutex_unlock(&workerMutex);

        sleep(1);
    }
    return NULL;
}
 */

/*
Message getMessage() {
    Message r = {JOB, {{ADD, 2, 3}}};
//    scanf("%lf %c %lf", &r.left, r.type, &r.right)
//    if (safe_getline_content(&lineBuffer, &lineBufferSize, stdin) == EOF) {
//        return NULL;
//    }
//
//    Message *request = interpretLine(lineBuffer, lineBufferSize);
//    lineCount++;
//    printf("%u%s", lineCount, PS);
//
//    return request;
    return r;
}
 */

/*
int chooseWorker() {
    return (int) ((rand() / (double) RAND_MAX) * fdMax);
}
 */


static char getJobTypeSymbol(JobType jobType) {
    switch (jobType) {
        case ADD:
            return '+';
        case SUB:
            return '-';
        case MUL:
            return '*';
        case DIV:
            return '/';
        default:
            return '0';
    }
}


/// must have workerMutex
static bool isWorkerNameUnique(char *name) {
    HashMapEntry *workerEntry = workers->entries->next;

    while (workerEntry != workers->entries) {
        Worker *w = workerEntry->value;
        if (strcmp(name, w->name) == 0) return false;
        workerEntry = workerEntry->next;
    }

    return true;
}

/// must have workerMutex
static void registerWorker(int fdWorker, char *name) {
    if (!isWorkerNameUnique(name)) {
        Message m = {NAME_TAKEN};
        sendMessage(fdWorker, &m);
        MLOG("Name taken: %s", name);
        return;
    }

    Worker *registered = safe_malloc(sizeof(Worker));
    strncpy(registered->name, name, sizeof(registered->name));
    registered->fd = fdWorker;
    registered->responsive = true;

    HashMap_add(workers, (void *) (uintptr_t) registered->fd, registered);

    ArrayQueue_add(workerJobQueue, registered);

    Message ok = {OK};
    sendMessage(fdWorker, &ok);

    MLOG("Added worker: %s", registered->name);
}

/// must have workerMutex
static void removeWorker(int fdWorker) {
    FD_CLR(fdWorker, &workerFdSet);
    if (fdWorker == fdMax) --fdMax;
    close(fdWorker);

    Worker *removed = HashMap_remove(workers, (void *) (uintptr_t) fdWorker);
    MLOG("Removed worker: %s", removed->name);

    free(removed);
}

/// must have workerMutex
static char *getNameByFd(int fd) {
    Worker *w = HashMap_get(workers, (void *) (uintptr_t) fd);
    return w->name;
}

/// must have workerMutex
static void handleJobResult(JobData *job) {
    if (!job->ok) {
        printf("%d> error\n", job->id);
        return;
    }

    char typeSymbol = getJobTypeSymbol(job->type);

    printf("%d> %lf %c %lf = %lf\n", job->id, job->left, typeSymbol, job->right, job->result);
}

/// must have workerMutex
static void handleMessage(int fdSource, Message *msg) {
    switch (msg->type) {
        case OK:
            printf("Ok\n");
            break;
        case REGISTER:
            MLOG("REGISTER from %d", fdSource);
            registerWorker(fdSource, ((RegisterData *) msg->data)->name);
        case PING:
            MLOG("PING from %s", getNameByFd(fdSource));
            Message pong = {PONG};
            sendMessage(fdSource, &pong);
            break;
        case PONG:
            MLOG("PONG from %s", getNameByFd(fdSource));
            break;
        case JOB:
            MLOG("JOB from %s", getNameByFd(fdSource));
            ArrayQueue_add(workerJobQueue, HashMap_get(workers, (void *) (uintptr_t) fdSource));
            handleJobResult((JobData *) msg->data);
        default:
            MLOG("Unsupported message type %d from %d", msg->type, fdSource);
            break;
    }
}

static void runServer(int fdLocalSocket) {
    listen(fdLocalSocket, SOMAXCONN);
    mlog("Server up");

    fdMax = 0;
    if (fdLocalSocket > fdMax) fdMax = fdLocalSocket;
    FD_ZERO(&workerFdSet);
    workers = HashMap_new(valueHashcode);
    workerJobQueue = ArrayQueue_new(FD_SETSIZE);

    while (!shutdownFlag) {
        pthread_mutex_lock(&workerMutex);
        fd_set readSet = workerFdSet;
        pthread_mutex_unlock(&workerMutex);

        struct timeval timeout = DEFAULT_TIMEOUT;

        if (select(fdMax + 1, &readSet, NULL, NULL, &timeout) < 1) continue;

        pthread_mutex_lock(&workerMutex);
        {
            for (int fd = 0; fd <= fdMax; ++fd) {
                if (!FD_ISSET(fd, &readSet)) continue;

                if (fd != fdLocalSocket) {
                    Message message;
                    ssize_t readBytes = recv(fd, &message, sizeof(message), MSG_WAITALL);

                    if (readBytes < sizeof(message)) {
                        removeWorker(fd);
                    } else {
                        handleMessage(fd, &message);
                    }
                } else {
                    int fdNewWorker = accept(fdLocalSocket, NULL, 0);
                    FD_SET(fdNewWorker, &workerFdSet);
                    if (fdNewWorker > fdMax) fdMax = fdNewWorker;
                }
            }
        }
        pthread_mutex_unlock(&workerMutex);
//        mlog("loop");
    }

    ArrayQueue_delete(workerJobQueue);
    HashMap_delete(workers);
}


static void signalHandler_shutdown(int ignored) {
    unlink(localSocketPath);
    shutdownFlag = true;
}


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", USAGE);
        exit(EXIT_FAILURE);
    }

    shutdownFlag = false;

    localSocketPath = argv[2];
    int fdLocalSocket = createLocalSocket();
    bindLocalSocket(fdLocalSocket, localSocketPath);

    safe_signal(SIGINT, signalHandler_shutdown);
    safe_signal(SIGTERM, signalHandler_shutdown);

    runServer(fdLocalSocket);

    return 0;
}
