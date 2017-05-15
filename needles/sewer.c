
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdbool.h>

#include "sewer.h"


static char *const ARG_INFO = "Arguments: THREAD_COUNT FILENAME RECORDS_READ KEYWORD";

static bool keywordFound;
static unsigned int threadCount;
static unsigned int recordsRead;
static char *keyword;
static pthread_t *threads;
static pthread_key_t readRecord;
static pthread_mutex_t lck;

int searchedFile;


void *threadStart_searchRecords(void *pOffset) {
#ifdef ASYNC
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif
#ifdef DETACH
    pthread_detach(pthread_self());
#endif

    Record *recordBuffer = safe_calloc(recordsRead, sizeof(Record));
    pthread_setspecific(readRecord, recordBuffer);

    while (safe_read(searchedFile, recordBuffer, recordsRead * sizeof(Record)) != 0) {
        Record const *r = recordBuffer;
        for (int i = 0; i < recordsRead; ++i) {
            if (strstr(recordBuffer->text, keyword) == NULL) {
                ++r;
                continue;
            }

            pthread_mutex_lock(&lck);
            if (keywordFound) {
                pthread_mutex_unlock(&lck);
                continue;
            }
            keywordFound = true;
            printf("Keyword found by thread %ld in record %d\n", syscall(SYS_gettid), r->id);
#ifndef DETACH
            pthread_t threadSelf = pthread_self();
            for (int j = 0; j < threadCount; ++j) {
                if (!pthread_equal(threads[j], threadSelf)) pthread_cancel(threads[j]);
            }
#endif
            --threadCount;
            pthread_mutex_unlock(&lck);
            pthread_exit(NULL);
        }
    }

    pthread_mutex_lock(&lck);
    --threadCount;
    pthread_mutex_unlock(&lck);

    return NULL;
}


int main(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, ARG_INFO);
        exit(EXIT_FAILURE);
    }

    threadCount = (unsigned int) atol(argv[1]);
    char *filename = argv[2];
    recordsRead = (unsigned int) atol(argv[3]);
    keyword = argv[4];

    threads = safe_calloc(threadCount, sizeof(pthread_t));
    EXIT_FREE(threads);

    searchedFile = open(filename, O_RDONLY);
    EXIT_CLOSE(searchedFile);

    pthread_attr_t threadAttributes;
    pthread_attr_init(&threadAttributes);

    pthread_key_create(&readRecord, free);
    pthread_mutex_init(&lck, NULL);

    keywordFound = false;

    pthread_mutex_lock(&lck);
    for (int i = 0; i < threadCount; ++i) {
        pthread_create(&threads[i], &threadAttributes, threadStart_searchRecords, (void *) (uintptr_t) i);
    }
    pthread_mutex_unlock(&lck);
#ifndef DETACH
    for (int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i], NULL);
    }
#else
    while (1) {
        pthread_mutex_lock(&lck);
        if (threadCount <= 0) {
            pthread_mutex_unlock(&lck);
            break;
        }
        pthread_mutex_unlock(&lck);
        sleep(1);
    }
#endif
    if (!keywordFound) printf("Keyword not found\n");

    pthread_attr_destroy(&threadAttributes);

    return 0;
}
