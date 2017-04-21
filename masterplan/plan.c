//
// Created by luknw on 4/17/17.
//

#include <stdlib.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>

#include "libsafe/safe.h"
#include "config.h"


static unsigned int seed;


double complex f(double complex c, double complex z) {
    return z * z + c;
}

double randomRange(double min, double max) {
    double partition = rand_r(&seed) / (double) RAND_MAX;
    return min + partition * (max - min);
}

unsigned int findPointIterations(double complex point, unsigned int maxIterations) {
    double complex z = 0;
    unsigned int i;
    for (i = 0; i < maxIterations && cabs(z) <= 2; ++i) {
        z = f(point, z);
    }
    return i;
}

int main(int argc, char **argv) {
    if (argc < 4) return -1;

    char *outputStreamPathName = argv[1];
    unsigned int pointCount = (unsigned int) strtoul(argv[2], NULL, 10);
    unsigned int maxIterations = (unsigned int) strtoul(argv[3], NULL, 10);

    FILE *outputStream = safe_fopen(outputStreamPathName, "w");

    seed = (unsigned int) (time(NULL) ^ getpid());
    for (int i = 0; i < pointCount; ++i) {
        double complex point = randomRange(X_MIN, X_MAX) + randomRange(Y_MIN, Y_MAX) * I;
        unsigned int pointIterations = findPointIterations(point, maxIterations);
        fprintf(outputStream, "%lf %lf %u\n", creal(point), cimag(point), pointIterations);
    }

    safe_fclose(outputStream);

    return 0;
}
