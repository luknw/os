#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "libarrayqueue/arrayQueue.h"


static char *const USAGE = "Usage: gandalf CHILD_COUNT PASS_REQUEST_THRESHOLD\n";

static const int FENCE_CHILD_SLEEP_SECONDS = 10;

static char *const MSG_CHILD_TERMINATED = "%3ld | PID: %6d  Status: %2d | Child terminated\n";
static char *const MSG_SIGNAL_RECEIVED = "%3ld | PID: %6d  Signal: %2d";

static char *const ERROR_TIME = "Error getting time";
static char *const ERROR_PASS_REQUEST = "Error sending pass request";
static char *const ERROR_FORK = "Error while forking";
static char *const ERROR_SIGNAL_SET = "Error operating on signal set";
static char *const ERROR_SIGNAL_MASK = "Error operating on signal mask";
static char *const ERROR_WAIT_FOR_CHILD = "Error waiting for child";
static char *const ERROR_PERMIT_WAIT = "Error waiting for parent pass permit";

static const int NANOSECONDS_PER_SECOND = 1000 * 1000 * 1000;


static unsigned int childCount;
static unsigned int requestThreshold;
static bool requestThresholdMet;
static ArrayQueue *passRequests;
static time_t startTime;

static unsigned int randomSeed;


static void blockAllSignals(void) {
    sigset_t allSignals;
    if (sigfillset(&allSignals) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_SETMASK, &allSignals, NULL) == -1) {
        perror(ERROR_SIGNAL_MASK);
        exit(EXIT_FAILURE);
    }
}

static time_t sendSignalToPid(pid_t pid, int signal) {
    if (kill(pid, signal) == -1) {
        perror(ERROR_PASS_REQUEST);
        exit(EXIT_FAILURE);
    }

    time_t tm;
    if ((tm = time(NULL)) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }
    return tm;
}

static void childSleep(void) {
    time_t tm;
    if ((tm = time(NULL)) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }
    if ((randomSeed = ((unsigned int) tm) ^ ((unsigned int) getpid())) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }
    unsigned int sleepSeconds = (unsigned int) (rand_r(&randomSeed) % FENCE_CHILD_SLEEP_SECONDS);
    sleep(sleepSeconds);
}

static void setupParentAcknowledgedSignalSet(sigset_t *parentAcknowledgedSignalSet) {
    if (sigemptyset(parentAcknowledgedSignalSet) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(parentAcknowledgedSignalSet, SIGUSR1) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(parentAcknowledgedSignalSet, SIGUSR2) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(parentAcknowledgedSignalSet, SIGTERM) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
}

static bool requestPassFromParentWithConfirmation(pid_t ppid) {
    printf("Gandalf!\n");

    int tryCount = 0;
    bool parentAcknowledged = false;
    bool parentPermit = false;
    sigset_t parentAcknowledgedSignalSet;
    siginfo_t siginfo;
    struct timespec timeout = {0, NANOSECONDS_PER_SECOND / 100};

    setupParentAcknowledgedSignalSet(&parentAcknowledgedSignalSet);

    do {
        ++tryCount;
        sendSignalToPid(ppid, SIGUSR1);

        if (sigtimedwait(&parentAcknowledgedSignalSet, &siginfo, &timeout) == -1) {
            if (errno == EAGAIN) continue;
            perror(ERROR_PASS_REQUEST);
            exit(EXIT_FAILURE);
        }

        switch (siginfo.si_signo) {
            case SIGUSR1:
                parentAcknowledged = true;
                break;
            case SIGUSR2:
                parentAcknowledged = true;
                parentPermit = true;
                break;
            default:
            case SIGTERM:
                exit(EXIT_FAILURE);
        }

    } while (!parentAcknowledged);
    printf("PID: %d Tries: %d\n", getpid(), tryCount);

    return parentPermit;
}

static void setupParentPermitSignalSet(sigset_t *parentPermitSignalSet) {
    if (sigemptyset(parentPermitSignalSet) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(parentPermitSignalSet, SIGUSR2) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(parentPermitSignalSet, SIGTERM) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }
}

static void waitForParentPermit(pid_t ppid) {
    sigset_t parentPermitSignalSet;
    siginfo_t siginfo;

    setupParentPermitSignalSet(&parentPermitSignalSet);

    do {
        if (sigwaitinfo(&parentPermitSignalSet, &siginfo) == -1) {
            perror(ERROR_PERMIT_WAIT);
            exit(EXIT_FAILURE);
        }
    } while (siginfo.si_pid != ppid);

    if (siginfo.si_signo == SIGTERM) {
        exit(EXIT_FAILURE);
    }
}

static time_t notifyParent(pid_t ppid) {
    printf("Nooooooo!\n");
    int signal = SIGRTMIN + rand_r(&randomSeed) % (SIGRTMAX - SIGRTMIN + 1);
    return sendSignalToPid(ppid, signal);
}

static void childJob(pid_t ppid) {
    childSleep();

    time_t requestSentTime;
    if ((requestSentTime = time(NULL)) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }

    bool permit = requestPassFromParentWithConfirmation(ppid);
    if (!permit) waitForParentPermit(ppid);
    time_t parentNotifiedTime = notifyParent(ppid);

    exit((int) (parentNotifiedTime - requestSentTime));
}

static void spawnChild(pid_t ppid) {
    pid_t pid = fork();
    if (pid == 0) {
        childJob(ppid);
    } else if (pid < 0) {
        perror(ERROR_FORK);
        exit(EXIT_FAILURE);
    }
}

static void handleChildPassRequest(pid_t childPid) {
    if (requestThresholdMet) {
        sendSignalToPid(childPid, SIGUSR2);
        return;
    }

    sendSignalToPid(childPid, SIGUSR1);

    pid_t *childRequestPid = safe_malloc(sizeof(pid_t));
    *childRequestPid = childPid;
    ArrayQueue_add(passRequests, childRequestPid);

    if (passRequests->len >= requestThreshold) {
        printf("Run you fools!\n");
        requestThresholdMet = true;

        while (!ArrayQueue_isEmpty(passRequests)) {
            pid_t *passRequest = ArrayQueue_remove(passRequests);
            sendSignalToPid(*passRequest, SIGUSR2);
            safe_free(passRequest);
        }
    }
}

static void waitForTerminatedChildren(void) {
    int status;
    pid_t childPid;
    while (childCount > 0 && (childPid = waitpid(-1, &status, WNOHANG)) > 0) {
        childCount--;

        time_t tm;
        if ((tm = time(NULL)) == -1) {
            perror(ERROR_TIME);
            exit(EXIT_FAILURE);
        }

        printf(MSG_CHILD_TERMINATED, tm - startTime, childPid, WEXITSTATUS(status));
    }
    if (childPid == -1) {
        perror(ERROR_WAIT_FOR_CHILD);
        exit(EXIT_FAILURE);
    }
}

static void terminateChildren(void) {
    sendSignalToPid(0, SIGTERM);
}

void handleSignal(siginfo_t *signalInfo) {
    time_t tm;
    if ((tm = time(NULL)) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }

    printf(MSG_SIGNAL_RECEIVED, tm - startTime, signalInfo->si_pid, signalInfo->si_signo);

    switch (signalInfo->si_signo) {
        case SIGUSR1:
            printf(" | SIGUSR1\n");
            handleChildPassRequest(signalInfo->si_pid);
            break;
        case SIGCHLD:
            printf(" | SIGCHLD\n");
            waitForTerminatedChildren();
            break;
        case SIGINT:
            printf(" | SIGINT\n");
            terminateChildren();
            break;
        default:
            printf(" | SIGRT\n");
            break;
    }
}

void listenForChildren(void) {
    sigset_t allSignals;
    siginfo_t signalInfo;

    if (sigfillset(&allSignals) == -1) {
        perror(ERROR_SIGNAL_SET);
        exit(EXIT_FAILURE);
    }

    do {
        if (sigwaitinfo(&allSignals, &signalInfo) == -1) {
            perror(ERROR_WAIT_FOR_CHILD);
            exit(EXIT_FAILURE);
        }
        handleSignal(&signalInfo);
    } while (childCount > 0);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf(USAGE);
        exit(EXIT_SUCCESS);
    }

    childCount = (unsigned int) atol(argv[1]);
    requestThreshold = (unsigned int) atol(argv[2]);
    requestThresholdMet = false;


    blockAllSignals();

    if ((startTime = time(NULL)) == -1) {
        perror(ERROR_TIME);
        exit(EXIT_FAILURE);
    }

    pid_t ppid = getpid();
    for (int i = 0; i < childCount; ++i) {
        spawnChild(ppid);
    }

    passRequests = ArrayQueue_new(childCount);
    listenForChildren();

    ArrayQueue_delete(passRequests);
    return 0;
}
