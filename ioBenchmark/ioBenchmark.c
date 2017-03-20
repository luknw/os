#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>

#include "ioBenchmark.h"
#include "libsafealloc/safeAlloc.h"
#include "libsafefile/safeFile.h"


static const char *ACTION_GENERATE = "generate";
static const char *ACTION_SHUFFLE = "shuffle";
static const char *ACTION_SORT = "sort";

static const char *PROVIDER_LIB = "lib";
static const char *PROVIDER_SYS = "sys";

static const char *RANDOM_SOURCE = "/dev/urandom";


static const argp_option options[] =
        {{"action",   'a', "ACTION", /* flags */ 0, "Action to take, one of: generate, shuffle, sort"},
         {"provider", 'p', "ACTION_PROVIDER",    0, "Provider of the action, one of: sys, lib"},
         {"count",    'c', "RECORD_COUNT",       0, "Count of the records in used file"},
         {"size",     's', "RECORD_SIZE",        0, "Size of each record in used file"},
         {"file",     'f', "TARGET_FILE",        0, "File to use during benchmark"},
         {0}};

static const Arguments DEFAULT_ARGS = {GENERATE, LIBRARY, 100, 512, "ioBenchmarkRecords.bin"};

static error_t parser(int key, char *arg, struct argp_state *state) {
    Arguments *args = state->input;

    switch (key) {
        case 'a':
            if (strcmp(ACTION_GENERATE, arg) == 0) {
                args->action = GENERATE;
            } else if (strcmp(ACTION_SHUFFLE, arg) == 0) {
                args->action = SHUFFLE;
            } else if (strcmp(ACTION_SORT, arg) == 0) {
                args->action = SORT;
            }
            return 0;
        case 'p':
            if (strcmp(PROVIDER_LIB, arg) == 0) {
                args->provider = LIBRARY;
            } else if (strcmp(PROVIDER_SYS, arg) == 0) {
                args->provider = SYSTEM;
            }
            return 0;
        case 'c':
            sscanf(arg, "%zu", &args->recordCount);
            return 0;
        case 's':
            sscanf(arg, "%zu", &args->recordSize);
            return 0;
        case 'f':
            args->filePath = arg;
        case ARGP_KEY_INIT:
            args->action = (Action) -1;
            args->provider = (ActionProvider) -1;
            args->recordCount = 0;
            args->recordSize = 0;
            args->filePath = NULL;
            return 0;
        case ARGP_KEY_END:
            if (args->action == -1) {
                printf("Assuming action: %s\n", ACTION_GENERATE);
                args->action = DEFAULT_ARGS.action;
            }
            if (args->provider == -1) {
                printf("Assuming action provider: %s\n", PROVIDER_LIB);
                args->provider = DEFAULT_ARGS.provider;
            }
            if (args->recordCount == 0) {
                printf("Assuming record count: %zu\n", DEFAULT_ARGS.recordCount);
                args->recordCount = DEFAULT_ARGS.recordCount;
            }
            if (args->recordSize == 0) {
                printf("Assuming record size: %zu\n", DEFAULT_ARGS.recordSize);
                args->recordSize = DEFAULT_ARGS.recordSize;
            }
            if (args->filePath == NULL) {
                printf("Assuming file: %s\n", DEFAULT_ARGS.filePath);
                args->filePath = DEFAULT_ARGS.filePath;
            }
        default:
            return ARGP_ERR_UNKNOWN;
    }
}

static const argp sArgp = {options, parser};


void generate(size_t recordCount, size_t recordSize, char *filePath) {
    char **records = safe_calloc(recordCount, recordSize);

    FILE *randomSource = safe_fopen(RANDOM_SOURCE, "rb");
    safe_fread(records, recordSize, recordCount, randomSource);
    safe_fclose(randomSource);

    FILE *recordFile = safe_fopen(filePath, "w");
    safe_fwrite(records, recordSize, recordCount, recordFile);
    safe_fclose(recordFile);

    safe_free(records);
}

void libShuffle(size_t recordCount, size_t recordSize, char *filePath) {
    srand((unsigned int) time(NULL));

    FILE *f = safe_fopen(filePath, "r+b");

    unsigned char *a = safe_malloc(recordSize);
    unsigned char *b = safe_malloc(recordSize);

    for (int i = 0; i < recordCount - 1; ++i) {
        int j = i + (int) ((rand() / (double) RAND_MAX) * (recordCount - 1 - i));
        if (i == j) continue;

        safe_fseek(f, recordSize * i, SEEK_SET);
        safe_fread(a, recordSize, 1, f);

        safe_fseek(f, recordSize * (j - i - 1), SEEK_CUR);
        safe_fread(b, recordSize, 1, f);

        safe_fseek(f, recordSize * (i - j - 1), SEEK_CUR);
        safe_fwrite(b, recordSize, 1, f);

        safe_fseek(f, recordSize * (j - i - 1), SEEK_CUR);
        safe_fwrite(a, recordSize, 1, f);
    }

    safe_free(b);
    safe_free(a);
    safe_fclose(f);
}

void sysShuffle(size_t recordCount, size_t recordSize, char *filePath) {
    srand((unsigned int) time(NULL));

    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned char *a = safe_malloc(recordSize);
    unsigned char *b = safe_malloc(recordSize);

    for (int i = 0; i < recordCount - 1; ++i) {
        int j = i + (int) ((rand() / (double) RAND_MAX) * (recordCount - 1 - i));
        if (i == j) continue;

        safe_lseek(fd, recordSize * i, SEEK_SET);
        safe_read(fd, a, recordSize);

        safe_lseek(fd, recordSize * (j - i - 1), SEEK_CUR);
        safe_read(fd, b, recordSize);

        safe_lseek(fd, recordSize * (i - j - 1), SEEK_CUR);
        safe_write(fd, b, recordSize);

        safe_lseek(fd, recordSize * (j - i - 1), SEEK_CUR);
        safe_write(fd, a, recordSize);
    }

    safe_free(b);
    safe_free(a);
    safe_close(fd);
}

void libSort(size_t recordCount, size_t recordSize, char *filePath) {
    if (recordCount < 2) return;
    size_t keySize = sizeof(unsigned char);
    if (keySize > recordSize) {
        fprintf(stderr, "Record size too small: %zu", recordSize);
        exit(EXIT_FAILURE);
    }

    FILE *f = safe_fopen(filePath, "r+b");

    unsigned char *a = safe_malloc(recordSize);
    unsigned char *b = safe_malloc(recordSize);

    bool sorted = false;
    for (size_t i = recordCount - 1; i > 0 && !sorted; --i) {
        sorted = true;
        safe_rewind(f);

        for (size_t j = 0; j < i; ++j) {
            safe_fread(a, keySize, 1, f);
            safe_fseek(f, recordSize - keySize, SEEK_CUR);

            safe_fread(b, keySize, 1, f);
            safe_fseek(f, -keySize, SEEK_CUR);

            if (*a > *b) {
                sorted = false;

                safe_fread(b, recordSize, 1, f);
                safe_fseek(f, -2 * recordSize, SEEK_CUR);
                safe_fread(a, recordSize, 1, f);

                safe_fwrite(a, recordSize, 1, f);
                safe_fseek(f, -2 * recordSize, SEEK_CUR);
                safe_fwrite(b, recordSize, 1, f);
            }
        }
    }

    safe_free(b);
    safe_free(a);
    safe_fclose(f);
}

void sysSort(size_t recordCount, size_t recordSize, char *filePath) {
    if (recordCount < 2) return;
    size_t keySize = sizeof(unsigned char);
    if (keySize > recordSize) {
        fprintf(stderr, "Record size too small: %zu", recordSize);
        exit(EXIT_FAILURE);
    }

    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned char *a = safe_malloc(recordSize);
    unsigned char *b = safe_malloc(recordSize);

    bool sorted = false;
    for (size_t i = recordCount - 1; i > 0 && !sorted; --i) {
        sorted = true;
        safe_lseek(fd, 0, SEEK_SET);

        for (size_t j = 0; j < i; ++j) {
            safe_read(fd, a, keySize);
            safe_lseek(fd, recordSize - keySize, SEEK_CUR);

            safe_read(fd, b, keySize);
            safe_lseek(fd, -keySize, SEEK_CUR);

            if (*a > *b) {
                sorted = false;

                safe_read(fd, b, recordSize);
                safe_lseek(fd, -2 * recordSize, SEEK_CUR);
                safe_read(fd, a, recordSize);

                safe_write(fd, a, recordSize);
                safe_lseek(fd, -2 * recordSize, SEEK_CUR);
                safe_write(fd, b, recordSize);
            }
        }
    }

    safe_free(b);
    safe_free(a);
    safe_close(fd);
}


int main(int argc, char **argv) {
    Arguments args;
    argp_parse(&sArgp, argc, argv, 0, NULL, &args);

    switch (args.action) {
        case GENERATE:
            generate(args.recordCount, args.recordSize, args.filePath);
            break;
        case SHUFFLE:
            if (LIBRARY == args.provider) {
                libShuffle(args.recordCount, args.recordSize, args.filePath);
            } else {
                sysShuffle(args.recordCount, args.recordSize, args.filePath);
            }
            break;
        case SORT:
            if (LIBRARY == args.provider) {
                libSort(args.recordCount, args.recordSize, args.filePath);
            } else {
                sysSort(args.recordCount, args.recordSize, args.filePath);
            }
            break;
    }

    return 0;
}
