#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>


typedef enum SignalingStrategy {
    KILL,
    SIGQUEUE,
    RT_KILL
} SignalingStrategy;


static char *const USAGE = "dinner REQUESTS SIGNALING_STRATEGY\n"
        "\tSIGNALING_STRATEGY: 0 - kill\n"
        "\t                    1 - sigqueue\n"
        "\t                    2 - rt kill\n";


static char *const SIGNALING_STRATEGY_ERROR = "Invalid signaling strategy\n";
static char *const PARENT_SIGNALING_ERROR = "Error while parent signaling";
static char const CHILD_SIGNALING_ERROR[] = "Error while child signaling\n";
static char *const SIGNAL_SET_ERROR = "Error operating on signal set";
static char *const FORKING_ERROR = "Error while forking";
static char *const HANDLER_SET_ERROR = "Error setting handler";
static char *const SLEEPING_ERROR = "Error sleeping between signals";

static char *const PARENT_SUMMARY = "parent:    sent: %d\treceived: %d\n";
static char *const CHILD_SUMMARY = "child: received: %d\t    sent: %d\n";

static const struct timespec SIGNALING_INTERVAL = {0, 1000 * 1000 * 1000 / 100}; // works for sigusr on my computer
static const struct timespec MSG_FADE_INTERVAL = {0, 1000 * 1000 * 1000 / 1000};

sig_atomic_t sigSentCount;
sig_atomic_t sigReceivedCount;

sig_atomic_t exitFlag;

pid_t parentPid;


void childSignalHandler(int signo) {
    if (signo == SIGUSR2 || signo == SIGRTMAX) {
        exitFlag = 1;
        return;
    }

    ++sigReceivedCount;
    if (kill(parentPid, signo) == -1) {
        exitFlag = 1;
        write(STDERR_FILENO, CHILD_SIGNALING_ERROR, sizeof(CHILD_SIGNALING_ERROR));
    }
    ++sigSentCount;
}

void parentSignalHandler(int signo) {
    ++sigReceivedCount;
}

void registerChildSignalHandlers(void) {
    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    };
    if (sigaddset(&mask, SIGUSR1) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&mask, SIGUSR2) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&mask, SIGRTMIN) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&mask, SIGRTMAX) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }

    struct sigaction action;
    action.sa_handler = childSignalHandler;
    action.sa_mask = mask;
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &action, NULL) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGRTMIN, &action, NULL) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGRTMAX, &action, NULL) == -1) {
        perror(HANDLER_SET_ERROR);
        exit(EXIT_FAILURE);
    }
}

void registerParentSignalHandlers(void) {
    sigset_t mask;
    if (sigemptyset(&mask) == -1) {
        perror(SIGNAL_SET_ERROR);
        exit(EXIT_FAILURE);
    };

    struct sigaction action;
    action.sa_handler = parentSignalHandler;
    action.sa_mask = mask;
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror(PARENT_SIGNALING_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGRTMIN, &action, NULL) == -1) {
        perror(PARENT_SIGNALING_ERROR);
        exit(EXIT_FAILURE);
    }
}

int sigQueueWrapper(pid_t pid, int signo) {
    return sigqueue(pid, signo, (union sigval) {0});
}


void parentJob(int pendingSignals, SignalingStrategy strategy, pid_t childPid) {
    int (*signaler)(pid_t pid, int signo);
    int sigMessage, sigDelimiter;

    switch (strategy) {
        case KILL:
            signaler = kill;
            sigMessage = SIGUSR1;
            sigDelimiter = SIGUSR2;
            break;
        case SIGQUEUE:
            signaler = sigQueueWrapper;
            sigMessage = SIGUSR1;
            sigDelimiter = SIGUSR2;
            break;
        case RT_KILL:
            signaler = kill;
            sigMessage = SIGRTMIN;
            sigDelimiter = SIGRTMAX;
            break;
        default:
            fprintf(stderr, SIGNALING_STRATEGY_ERROR);
            exit(EXIT_FAILURE);
    }

    for (int i = 0; i < pendingSignals; ++i) {
        if ((*signaler)(childPid, sigMessage) == -1) {
            perror(PARENT_SIGNALING_ERROR);
            exit(EXIT_FAILURE);
        }
        ++sigSentCount;

        struct timespec left = SIGNALING_INTERVAL;
        while (nanosleep(&left, &left) == -1) {
            if (errno != EINTR) {
                perror(SLEEPING_ERROR);
                exit(EXIT_FAILURE);
            }
        }
    }
    if ((*signaler)(childPid, sigDelimiter) == -1) {
        perror(PARENT_SIGNALING_ERROR);
        exit(EXIT_FAILURE);
    }

    struct timespec left = MSG_FADE_INTERVAL;
    while (nanosleep(&left, NULL) == -1) {
        if (errno != EINTR) {
            perror(SLEEPING_ERROR);
            exit(EXIT_FAILURE);
        }
    }

    printf(PARENT_SUMMARY, sigSentCount, sigReceivedCount);
}

void childJob(void) {
    while (exitFlag == 0) {
        pause();
    }

    printf(CHILD_SUMMARY, sigReceivedCount, sigSentCount);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        printf(USAGE);
        exit(EXIT_SUCCESS);
    }

    int pendingSignals = atoi(argv[1]);
    SignalingStrategy strategy = (SignalingStrategy) atoi(argv[2]);

    sigSentCount = 0;
    sigReceivedCount = 0;
    exitFlag = 0;


    parentPid = getpid();

    registerChildSignalHandlers();

    pid_t pid = fork();
    if (pid > 0) {
        registerParentSignalHandlers();
        parentJob(pendingSignals, strategy, pid);
    } else if (pid == 0) {
        childJob();
    } else {
        perror(FORKING_ERROR);
        exit(EXIT_FAILURE);
    }

    return 0;
}