#include <stdio.h>
#include <stdlib.h>


static char *const USAGE = "Gimme 2 of'em numbahs";


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", USAGE);
        exit(EXIT_FAILURE);
    }

    unsigned int bastards = (unsigned int) atol(argv[1]);
    unsigned int barbings = (unsigned int) atol(argv[2]);

    while(bastards--){
        spawnLongHairedBastard(barbings);
    }
    reheatDinnerUntilBastardsComeBack();

    return 0;
}
