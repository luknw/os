#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#include "libsafe/safeAlloc.h"
#include "libsafe/safeExit.h"
#include "liblogger/logger.h"


static char *const USAGE =
        "Usage: spindle TABLE_SIZE READER_COUNT WRITER_COUNT [-i]\n"
                "\t-i\t\tlog indices verbosely";

static char *const VERBOSE_FLAG = "-i";


static const int MIN_TABLE_VALUE = 1;
static const int MAX_TABLE_VALUE = 100;
static volatile bool verbose;

static pthread_barrier_t initialization;


static size_t tableSize;
static int *table;


#ifdef SEMAPHORE
/// starves readers

#include <semaphore.h>

unsigned int writerCount;
unsigned int readerCount;

sem_t readerSemaphore;
sem_t writerSemaphore;
sem_t readTrySemaphore;
sem_t tableSemaphore;

void initThreadSync(unsigned int threadCount) {
    pthread_barrier_init(&initialization, NULL, 1 + threadCount);

    readerCount = 0;
    writerCount = 0;

    sem_init(&readerSemaphore, 0, 1);
    sem_init(&writerSemaphore, 0, 1);
    sem_init(&readTrySemaphore, 0, 1);
    sem_init(&tableSemaphore, 0, 1);
}


void startRead(void) {
    sem_wait(&readTrySemaphore);
    sem_wait(&readerSemaphore);

    ++readerCount;
    if (readerCount == 1) sem_wait(&tableSemaphore);

    sem_post(&readerSemaphore);
    sem_post(&readTrySemaphore);
}

void endRead(void) {
    sem_wait(&readerSemaphore);

    --readerCount;
    if (readerCount == 0) sem_post(&tableSemaphore);

    sem_post(&readerSemaphore);
}

void startWrite(void) {
    sem_wait(&writerSemaphore);

    ++writerCount;
    if (writerCount == 1) sem_wait(&readTrySemaphore);

    sem_post(&writerSemaphore);

    sem_wait(&tableSemaphore);
}

void endWrite(void) {
    sem_post(&tableSemaphore);

    sem_wait(&writerSemaphore);

    --writerCount;
    if (writerCount == 0) sem_post(&readTrySemaphore);

    sem_post(&writerSemaphore);
}

#else
/// fifo

#include "libarrayqueue/arrayQueue.h"


pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tableMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queueMoved = PTHREAD_COND_INITIALIZER;
pthread_cond_t noReaders = PTHREAD_COND_INITIALIZER;

unsigned int readerCount;

ArrayQueue *queue;


void initThreadSync(unsigned int threadCount) {
    pthread_barrier_init(&initialization, NULL, 1 + threadCount);

    readerCount = 0;

    queue = ArrayQueue_new(threadCount);
    ArrayQueue_exit_delete(queue);
}

void enqueueAndWait(void) {
    pthread_mutex_lock(&queueMutex);
    {
        pthread_t *pSelf = safe_malloc(sizeof(pthread_t));
        *pSelf = pthread_self();
        ArrayQueue_add(queue, pSelf);
        if (queue->len > 1) {
            while (pSelf != *(queue->front)) {
                pthread_cond_wait(&queueMoved, &queueMutex);
            }
        }
    }
    pthread_mutex_unlock(&queueMutex);
}

void dequeue(void) {
    pthread_mutex_lock(&queueMutex);
    {
        pthread_t *pSelf = ArrayQueue_remove(queue);
        safe_free(pSelf);
        pthread_cond_broadcast(&queueMoved);
    }
    pthread_mutex_unlock(&queueMutex);
}

void startRead(void) {
    enqueueAndWait();

    pthread_mutex_lock(&tableMutex);
    {
        ++readerCount;
        dequeue();
    }
    pthread_mutex_unlock(&tableMutex);
}

void endRead(void) {
    pthread_mutex_lock(&tableMutex);
    {
        --readerCount;
        if (readerCount == 0) pthread_cond_broadcast(&noReaders);
    }
    pthread_mutex_unlock(&tableMutex);
}

void startWrite(void) {
    enqueueAndWait();

    pthread_mutex_lock(&tableMutex);
    {}

    while (readerCount > 0) pthread_cond_wait(&noReaders, &tableMutex);
    dequeue();
}

void endWrite(void) {
    {}
    pthread_mutex_unlock(&tableMutex);
}

#endif


double randRange(double a, double b) {
    return a + (b - a) * (rand() / (double) RAND_MAX);
}

double randRange_r(double a, double b, unsigned int *seed) {
    return a + (b - a) * (rand_r(seed) / (double) RAND_MAX);
}

void *readerAction(void *readerData) {
    pthread_detach(pthread_self());

    int divisor = (int) (uintptr_t) readerData;

    size_t dividendCount;

    pthread_barrier_wait(&initialization);

    while (1) {
        startRead();

        dividendCount = 0;
        for (int i = 0; i < tableSize; ++i) {
            if (table[i] % divisor == 0) {
                ++dividendCount;
                if (verbose) MLOG("%d | %d at %d", divisor, table[i], i);
            }
        }
        MLOG("#{x: %d | x} = %zu", divisor, dividendCount);

        endRead();
    }

    return NULL;
}

void *writerAction(void *writerData) {
    pthread_detach(pthread_self());

    unsigned int seed = (unsigned int) (uintptr_t) writerData;

    pthread_barrier_wait(&initialization);

    while (1) {
        startWrite();
        int writeCount = (int) randRange_r(1, 100, &seed);

        for (int i = 0; i < writeCount; ++i) {
            int iWrite = (int) randRange_r(0, tableSize - 1, &seed);

            int newValue = (int) randRange_r(0, 100, &seed);
            int oldValue = table[iWrite];

            table[iWrite] = newValue;
            if (verbose) MLOG("%d -> %d at %d", oldValue, newValue, iWrite);
        }
        MLOG("<=%d entries modified", writeCount);
        endWrite();
    }

    return NULL;
}


void initTable(size_t tableSize) {
    table = safe_calloc(tableSize, sizeof(int));
    EXIT_FREE(table);

    srand((unsigned int) time(NULL));
    for (int i = 0; i < tableSize; ++i) {
        table[i] = (int) randRange(MIN_TABLE_VALUE, MAX_TABLE_VALUE);
    }
}


int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "%s\n", USAGE);
        exit(EXIT_FAILURE);
    }
    verbose = argc >= 5 && strcmp(VERBOSE_FLAG, argv[4]) == 0;

    tableSize = (size_t) atol(argv[1]);
    unsigned int readerCount = (unsigned int) (size_t) atol(argv[2]);
    unsigned int writerCount = (unsigned int) (size_t) atol(argv[3]);

    initTable(tableSize);

    unsigned int threadCount = readerCount + writerCount;

    initThreadSync(threadCount);

    pthread_t ignored;
    for (int i = 0; i < readerCount; ++i) {
        pthread_create(&ignored, NULL, readerAction,
                       (void *) (uintptr_t) (int) randRange(MIN_TABLE_VALUE, MAX_TABLE_VALUE));
    }
    for (int i = 0; i < writerCount; ++i) {
        int seed = rand();
        pthread_create(&ignored, NULL, writerAction, (void *) (uintptr_t) seed);
    }
    pthread_barrier_wait(&initialization);

    pthread_exit(NULL);
}
