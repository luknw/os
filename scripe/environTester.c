//
// Created by luknw on 3/25/17.
//


#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
    if (argc < 2) return -1;

    char *envVar = getenv(argv[1]);

    if (envVar == NULL) {
        printf("%s is not set\n", argv[1]);
    } else {
        printf("%s=%s\n", argv[1], envVar);
    }

    return 0;
}
