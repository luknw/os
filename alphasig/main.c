#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


static char SIGINT_RECEIVED_MESSAGE[] = "Odebrano sygnał SIGINT\n";

static char delta = 1;
static bool receivedSigInt = false;


static void sigTstpHandler(int signum) {
    delta = -delta;
}

static void sigIntHandler(int signum) {
    write(STDOUT_FILENO, SIGINT_RECEIVED_MESSAGE, sizeof(SIGINT_RECEIVED_MESSAGE));
    receivedSigInt = !receivedSigInt;
}


int main() {
    if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
        perror("Error changing signal action");
        exit(EXIT_FAILURE);
    }

    struct sigaction sigTstpAction;
    sigTstpAction.sa_handler = sigTstpHandler;
    if (sigaction(SIGTSTP, &sigTstpAction, NULL) == -1) {
        perror("Error changing signal action");
        exit(EXIT_FAILURE);
    }

    char c = 'a';

    while (!receivedSigInt) {
        printf("%c\n", c);

        c = c + delta;
        if (c > 'z') c = 'a';
        else if (c < 'a') c = 'z';

        sleep(1);
    }

    return 0;
}
