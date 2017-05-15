//
// Created by luknw on 2017-05-03
//

#ifndef SAFE_SIGNAL_H
#define SAFE_SIGNAL_H


#include <signal.h>


typedef __sighandler_t sighandler_t;


void safe_kill(pid_t pid, int signalNumber);

sighandler_t safe_signal(int signalNumber, sighandler_t handler);

void safe_sigaction(int signalNumber, const struct sigaction *action, struct sigaction *oldAction);


void safe_sigprocmask(int how, const sigset_t *signalSet, sigset_t *oldSignalSet);

void safe_sigwait(sigset_t *signalSet, int *deliveredSignal);


void safe_sigemptyset(sigset_t *signalSet);

void safe_sigfillset(sigset_t *signalSet);

void safe_sigaddset(sigset_t *signalSet, int signalNumber);

void safe_sigdelset(sigset_t *signalSet, int signalNumber);


#endif //SAFE_SIGNAL_H
