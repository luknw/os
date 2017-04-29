#include <stdio.h>
#include <stdlib.h>


static char *const ABUSAGE = "Oy man, need to know me chares number";


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, ABUSAGE);
        exit(EXIT_FAILURE);
    }

    unsigned int charesCount = (unsigned int) atol(argv[1]);

    openBarberShop(charesCount);
    while (!kitkat) {
        while (isClientWaiting()) {
            cutHimRightly();
        }
        developWorldConquestPlans();
    }
    closeBarberShop();

    return 0;
}
