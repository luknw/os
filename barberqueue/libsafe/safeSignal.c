//
// Created by luknw on 2017-05-03
//

#include "safeSignal.h"

#include <stdlib.h>
#include <stdio.h>


void safe_kill(pid_t pid, int signalNumber) {
    if (kill(pid, signalNumber) == -1) {
        perror("Error sending signal");
        exit(EXIT_FAILURE);
    }
}

sighandler_t safe_signal(int signalNumber, sighandler_t handler) {
    sighandler_t oldHandler = signal(signalNumber, handler);

    if (oldHandler == SIG_ERR) {
        perror("Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    return oldHandler;
}

void safe_sigaction(int signalNumber, const struct sigaction *action, struct sigaction *oldAction) {
    if (sigaction(signalNumber, action, oldAction) == -1) {
        perror("Error setting signal handler");
        exit(EXIT_FAILURE);
    }
}


void safe_sigprocmask(int how, const sigset_t *signalSet, sigset_t *oldSignalSet) {
    if (sigprocmask(how, signalSet, oldSignalSet) == -1) {
        perror("Error changing process signal mask");
        exit(EXIT_FAILURE);
    }
}

void safe_sigwait(sigset_t *signalSet, int *deliveredSignal) {
    if (sigwait(signalSet, deliveredSignal) > 0) {
        fprintf(stderr, "%s\n", "Error waiting for specified signal set");
        exit(EXIT_FAILURE);
    }
}


void safe_sigemptyset(sigset_t *signalSet) {
    if (sigemptyset(signalSet) == -1) {
        perror("Error creating empty signal set");
        exit(EXIT_FAILURE);
    }
}

void safe_sigfillset(sigset_t *signalSet) {
    if (sigfillset(signalSet) == -1) {
        perror("Error creating filled signal set");
        exit(EXIT_FAILURE);
    }
}

void safe_sigaddset(sigset_t *signalSet, int signalNumber) {
    if (sigaddset(signalSet, signalNumber) == -1) {
        perror("Error adding signal to set");
        exit(EXIT_FAILURE);
    }
}

void safe_sigdelset(sigset_t *signalSet, int signalNumber) {
    if (sigdelset(signalSet, signalNumber) == -1) {
        perror("Error removing signal from set");
        exit(EXIT_FAILURE);
    }
}
