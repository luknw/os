#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include "protocol.h"
#include "libsafe/safe.h"
#include "liblogger/logger.h"


static bool shutdownFlag;


static char *const USAGE = "USAGE: worker PORT_NUMBER SOCKET_PATH NAME";

static void handleJob(int fdSource, JobData *job) {
    switch (job->type) {
        case ADD:
            job->result = job->left + job->right;
            break;
        case SUB:
            job->result = job->left - job->right;
            break;
        case MUL:
            job->result = job->left * job->right;
            break;
        case DIV:
            if (job->right == 0.0) {
                job->ok = false;
            } else {
                job->result = job->left / job->right;
            }
            break;
    }

    Message msg;
    msg.type = JOB;
    memcpy(msg.data, job, sizeof(JobData));
    sendMessage(fdSource, &msg);
}

static void handleMessage(int fdSource, Message *msg) {
    switch (msg->type) {
        case OK:
            printf("Registered with server\n");
            break;
        case NAME_TAKEN:
            fprintf(stderr, "Client with this name already exists\n");
            exit(EXIT_FAILURE);
        case PING:
            mlog("PING from server");
            Message pong = {PONG};
            sendMessage(fdSource, &pong);
            break;
        case PONG:
            mlog("PONG from server");
            break;
        case JOB:
            mlog("JOB from server");
            handleJob(fdSource, (JobData *) msg->data);
        default:
            MLOG("Unsupported message type %d from %d", msg->type, fdSource);
            break;
    }
}


static void runWorker(int fdLocalSocket, char *name) {
    Message msg;
    msg.type = REGISTER;
    strncpy(msg.data, name, strlen(name));

    sendMessage(fdLocalSocket, &msg);

    while (!shutdownFlag) {
        ssize_t readBytes = recv(fdLocalSocket, &msg, sizeof(msg), MSG_WAITALL);

        if(readBytes < sizeof(msg)) continue;

        handleMessage(fdLocalSocket, &msg);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", USAGE);
        exit(EXIT_FAILURE);
    }

    shutdownFlag = false;

    char *localSocketPath = argv[2];
    char *name = argv[3];

    int fdLocalSocket = createLocalSocket();
    bindLocalSocket(fdLocalSocket, localSocketPath);

    connectLocalSocket(fdLocalSocket, localSocketPath);
    mlog("Connected");

    runWorker(fdLocalSocket, name);

    return 0;
}
