#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "libsafe/safe.h"
#include "config.h"


static char *const USAGE = "Usage: oasis PIPE_PATH RESOLUTION\n";
static char *const DATA_FILE_PATHNAME = "data";
static char *const PLOTTER = "gnuplot";
static char *const PLOTTER_COMMAND = "set view map\nset xrange [0:%zu]\nset yrange [0:%zu]\nplot '%s' with image\n";


size_t findIndex(double value, double rangeStart, double rangeEnd, size_t indexFence) {
    return (size_t) (((value - rangeStart) / (rangeEnd - rangeStart)) * (indexFence - 1));
}

void assembleMasterplan(char **lineBuffer, ssize_t *lineBufferSize, FILE *inputStream,
                        size_t resolution, unsigned int **iterationsMap) {
    double x, y;
    unsigned int iterations;

    while (safe_getline_content(lineBuffer, lineBufferSize, inputStream) != EOF) {
        if (sscanf(*lineBuffer, "%lf %lf %u", &x, &y, &iterations) < 3) continue;

        size_t i = findIndex(x, X_MIN, X_MAX, resolution);
        size_t j = findIndex(y, Y_MIN, Y_MAX, resolution);

        iterationsMap[i][j] = iterations;
    }
}

void saveMasterplan(size_t resolution, unsigned int **iterationsMap) {
    FILE *dataFileStream = safe_fopen(DATA_FILE_PATHNAME, "w");
    for (size_t i = 0; i < resolution; ++i) {
        for (size_t j = 0; j < resolution; ++j) {
            fprintf(dataFileStream, "%zu %zu %u\n", i, j, iterationsMap[i][j]);
        }
    }
    safe_fclose(dataFileStream);
}

void plotMasterplan(size_t resolution) {
    FILE *plotterStream;
    if ((plotterStream = popen(PLOTTER, "w")) == NULL) {
        perror("Error opening plotter");
        exit(EXIT_FAILURE);
    }

    fprintf(plotterStream, PLOTTER_COMMAND, resolution, resolution, DATA_FILE_PATHNAME);
    fflush(plotterStream);
    getchar();

    int plotterStatus = pclose(plotterStream);
    if (plotterStatus == -1) {
        perror("Error closing plotter");
        exit(EXIT_FAILURE);
    } else if (plotterStatus != 0) {
        fprintf(stderr, "Plotter error\n");
        exit(EXIT_FAILURE);
    }
}

void createMasterplan(char **lineBuffer, ssize_t *lineBufferSize, FILE *inputStream,
                      size_t resolution, unsigned int **iterationsMap) {
    assembleMasterplan(lineBuffer, lineBufferSize, inputStream, resolution, iterationsMap);
    saveMasterplan(resolution, iterationsMap);
    plotMasterplan(resolution);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf(USAGE);
        exit(EXIT_FAILURE);
    }

    char *inputStreamPathName = argv[1];
    size_t resolution = strtoul(argv[2], NULL, 10);

    unsigned int **iterationsMap = safe_calloc(resolution, sizeof(unsigned int *));
    for (size_t i = 0; i < resolution; ++i) {
        iterationsMap[i] = safe_calloc(resolution, sizeof(unsigned int));
    }

    if (mkfifo(inputStreamPathName, 00777) == -1) {
        perror("Error creating named pipe");
        exit(EXIT_FAILURE);
    }
    FILE *inputStream = safe_fopen(inputStreamPathName, "r");

    char *lineBuffer = NULL;
    ssize_t lineBufferSize = 0;


    createMasterplan(&lineBuffer, &lineBufferSize, inputStream, resolution, iterationsMap);


    safe_free(lineBuffer);

    safe_fclose(inputStream);
    if (remove(inputStreamPathName) == -1) {
        perror("Error removing named pipe");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < resolution; ++i) {
        safe_free(iterationsMap[i]);
    }
    safe_free(iterationsMap);

    return 0;
}