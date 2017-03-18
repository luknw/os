#include <argp.h>
#include <string.h>
#include <stdlib.h>

#include "ioBenchmark.h"


static const char *ACTION_GENERATE = "generate";
static const char *ACTION_SHUFFLE = "shuffle";
static const char *ACTION_SORT = "sort";

static const char *PROVIDER_LIB = "lib";
static const char *PROVIDER_SYS = "sys";


static const argp_option options[] =
        {{"action",   'a', "action", /* flags */ 0, "Action to take, one of: generate, shuffle, sort"},
         {"provider", 'p', "action_provider",    0, "Provider of the action, one of: sys, lib"},
         {"count",    'c', "record_count",       0, "Count of the records in used file"},
         {"size",     's', "record_size",        0, "Size of each record in used file"},
         {"file",     'f', "target_file",        0, "File to use during benchmark"},
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
            args->recordCount = atoi(arg);
            return 0;
        case 's':
            args->recordSize = atoi(arg);
            return 0;
        case 'f':
            args->filePath = arg;
        case ARGP_KEY_INIT:
            args->action = (Action) -1;
            args->provider = (ActionProvider) -1;
            args->recordCount = -1;
            args->recordSize = -1;
            args->filePath = NULL;
            return 0;
        case ARGP_KEY_END:
            if (args->action == -1) {
                printf("No valid action specified, assuming: %s\n", ACTION_GENERATE);
                args->action = DEFAULT_ARGS.action;
            }
            if (args->provider == -1) {
                printf("No valid action provider specified, assuming: %s\n", PROVIDER_LIB);
                args->provider = DEFAULT_ARGS.provider;
            }
            if (args->recordCount == -1) {
                printf("No valid record count specified, assuming: %d\n", DEFAULT_ARGS.recordCount);
                args->recordCount = DEFAULT_ARGS.recordCount;
            }
            if (args->recordSize == -1) {
                printf("No valid record size specified, assuming: %d\n", DEFAULT_ARGS.recordSize);
                args->recordSize = DEFAULT_ARGS.recordSize;
            }
            if (args->filePath == NULL) {
                printf("No valid file specified, assuming: %s", DEFAULT_ARGS.filePath);
                args->filePath = DEFAULT_ARGS.filePath;
            }
        default:
            return ARGP_ERR_UNKNOWN;
    }
}

static const argp sArgp = {options, parser};


void generate(int recordCount, int recordSize, char *filePath);

void shuffleLib(int recordCount, int recordSize, char *filePath);

void shuffleSys(int recordCount, int recordSize, char *filePath);

void sortLib(int recordCount, int recordSize, char *filePath);

void sortSys(int recordCount, int recordSize, char *filePath);


int main(int argc, char **argv) {
    Arguments args;
    argp_parse(&sArgp, argc, argv, 0, NULL, &args);

//    switch (args.action) {
//        case GENERATE:
//            generate(args.recordCount, args.recordSize, args.filePath);
//            break;
//        case SHUFFLE:
//            if (LIBRARY == args.provider) {
//                shuffleLib(args.recordCount, args.recordSize, args.filePath);
//            } else {
//                shuffleSys(args.recordCount, args.recordSize, args.filePath);
//            }
//            break;
//        case SORT:
//            if (LIBRARY == args.provider) {
//                sortLib(args.recordCount, args.recordSize, args.filePath);
//            } else {
//                sortSys(args.recordCount, args.recordSize, args.filePath);
//            }
//            break;
//    }

    return 0;
}
