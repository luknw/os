#define _GNU_SOURCE


#include <pthread.h>
#include <stdbool.h>

#include "libsafe/safe.h"
#include "liblogger/logger.h"


#define SENT_SIGNAL SIGFPE


static const unsigned int THREAD_COUNT = 4;

static pthread_t *threads;
static pthread_barrier_t initialized;


void signalHandler_logSignal(int signo) {
    MLOG("Signal handler: PID: %d signal: %d", getpid(), signo);
    pthread_exit(NULL);
}

void *threadStart_simulatedWork(void *signaler) {
    pthread_detach(pthread_self());

    mlog("Setting signal handler");
    sigset_t empty;
    safe_sigemptyset(&empty);

    struct sigaction action;
    action.sa_handler = signalHandler_logSignal;
    action.sa_mask = empty;
    action.sa_flags = 0;

    safe_sigaction(SENT_SIGNAL, &action, NULL);

//        mlog("Blocking signal");
//        sigset_t sig;
//        safe_sigfillset(&sig);
//        safe_sigaddset(&sig, SENT_SIGNAL);
//        pthread_sigmask(SIG_BLOCK, &sig, NULL);

    pthread_barrier_wait(&initialized);

    if (signaler) {
        sleep(1);
        mlog("Signalling");
//        safe_kill(0, SENT_SIGNAL);
//        pthread_kill(threads[1], SENT_SIGNAL);
        volatile int i = 1 / 0;
        sleep(1);
    } else {
        sleep(3);
    }

    mlog("Thread finished");
    return NULL;
}


int main(int argc, char **argv) {
    mlog("<- Main thread");

    threads = safe_calloc(THREAD_COUNT, sizeof(pthread_t));
    EXIT_FREE(threads);

    pthread_barrier_init(&initialized, NULL, THREAD_COUNT + 1);

    MLOG("Spawning %u threads", THREAD_COUNT);
    pthread_create(&threads[0], NULL, threadStart_simulatedWork, (void *) true);
    for (int i = 1; i < THREAD_COUNT; ++i) {
        pthread_create(&threads[i], NULL, threadStart_simulatedWork, (void *) false);
    }

//    mlog("Main thread: setting signal handler");
//    sigset_t empty;
//    safe_sigemptyset(&empty);
//
//    struct sigaction action;
//    action.sa_handler = signalHandler_logSignal;
//    action.sa_mask = empty;
//    action.sa_flags = 0;
//
//    safe_sigaction(SENT_SIGNAL, &action, NULL);
//    safe_signal(SENT_SIGNAL, signalHandler_logSignal);

//    mlog("Main thread: blocking signal");
//    sigset_t sig;
//    safe_sigfillset(&sig);
//    safe_sigaddset(&sig, SENT_SIGNAL);
//    pthread_sigmask(SIG_BLOCK, &sig, NULL);
//
    pthread_barrier_wait(&initialized);

    sleep(3);

//    for (int i = 0; i < THREAD_COUNT; ++i) {
//        pthread_join(threads[i], NULL);
//    }

    mlog("Main thread: exiting");
    pthread_exit(NULL);
}
