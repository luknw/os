#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "main.h"


static char cmdBuffer[3 * PATH_MAX];
static char *filePathBuffer = NULL;
static size_t filePathBufferSize = 0;


static const Action actions[] =
        {{NON_BLOCKING_READ_LOCK,  "Non-blocking read lock for one byte in file"},
         {BLOCKING_READ_LOCK,      "Blocking read lock for one byte in file"},
         {NON_BLOCKING_WRITE_LOCK, "Non-blocking write lock for one byte in file"},
         {BLOCKING_WRITE_LOCK,     "Blocking write lock for one byte in file"},
         {LIST_LOCKS,              "List locks for file"},
         {FREE_LOCK,               "Free the previously acquired lock"},
         {READ_CHAR,               "Read the specified byte from file"},
         {WRITE_CHAR,              "Write the specified byte to file"},
         {QUIT,                    "Exit"},
         {(ActionType) 0}};


static void printActions() {
    printf("Choose action\n");
    for (int i = 0; actions[i].type != 0; ++i) {
        printf("%d: %s\n", actions[i].type, actions[i].description);
    }
}


static void discardLine(FILE *stream) {
    int read;
    while ((read = fgetc(stream)) != EOF && read != '\n');

    if (ferror(stream)) {
        perror("Stream error");
        clearerr(stream);
        return;
    }
}

static int getFilePath() {
    printf("Specify file path\n");

    ssize_t charsRead;
    if ((charsRead = getline(&filePathBuffer, &filePathBufferSize, stdin)) == -1) {
        if (errno != 0) {
            perror("Error getting input");
            clearerr(stdin);
        }
        return -1;
    }
    if (charsRead > 0) {
        filePathBuffer[charsRead - 1] = '\0';
    }

    char *resolvedPath = realpath(filePathBuffer, NULL);
    if (resolvedPath == NULL) {
        perror("Error resolving path");
        return -1;
    }
    free(filePathBuffer);
    filePathBuffer = resolvedPath;

    return 0;
}

static int getFilePathAndOffset(long *offset) {

    if (getFilePath() == -1) return -1;

    printf("Specify offset in bytes from the beginning of the file\n");
    if (scanf("%ld", offset) == EOF) {
        if (ferror(stdin) != 0) {
            perror("Error getting input");
            clearerr(stdin);
        } else {
            fprintf(stderr, "Invalid input\n");
        }
        return -1;
    }
    discardLine(stdin);

    return 0;
}


static struct flock doSetFileByteLock(char *filePath, int waitFlag, short lockType, long offset) {
    struct flock flock;
    flock.l_type = lockType;
    flock.l_whence = SEEK_SET;
    flock.l_start = offset;
    flock.l_len = 1;

    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        flock.l_len = -1;
        return flock;
    }

    if (fcntl(fd, waitFlag, &flock) == -1) {
        perror("Error setting byte lock");
        flock.l_len = -1;
        return flock;
    }

    return flock;
}

static void setFileByteLock(int waitFlag, short lockType) {
    long offset;
    if (getFilePathAndOffset(&offset) == -1) return;

    doSetFileByteLock(filePathBuffer, waitFlag, lockType, offset);
}

void nonBlockingReadLock() {
    setFileByteLock(F_SETLK, F_RDLCK);
}

void blockingReadLock() {
    setFileByteLock(F_SETLKW, F_RDLCK);
}

void nonBlockingWriteLock() {
    setFileByteLock(F_SETLK, F_WRLCK);
}

void blockingWriteLock() {
    setFileByteLock(F_SETLKW, F_WRLCK);
}

void freeLock() {
    setFileByteLock(F_SETLK, F_UNLCK);
}

void readChar() {
    long offset;
    if (getFilePathAndOffset(&offset) == -1) return;

    int fd = open(filePathBuffer, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char charRead = '\0';
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking file");
    } else if (read(fd, &charRead, 1) == -1) {
        perror("Error reading file");
    }
    printf("%c\n", charRead);

    if (close(fd) == -1) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
}

void writeChar() {
    long offset;
    if (getFilePathAndOffset(&offset) == -1) return;

    int fd = open(filePathBuffer, O_WRONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char charWritten;

    printf("Specify character to write\n");
    if (scanf("%c", &charWritten) == EOF) {
        if (ferror(stdin) != 0) {
            perror("Error getting input");
            clearerr(stdin);
        } else {
            fprintf(stderr, "Invalid input\n");
        }
        return;
    }
    discardLine(stdin);


    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking file");
    } else if (write(fd, &charWritten, 1) == -1) {
        perror("Error writing file");
    }

    if (close(fd) == -1) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
}

static void listLocksInvoker(char *selfPath) {
    getFilePath();

    cmdBuffer[0] = '\0';
    strcat(cmdBuffer, selfPath);
    strcat(cmdBuffer, " ");
    strcat(cmdBuffer, selfPath);
    strcat(cmdBuffer, " ");
    strcat(cmdBuffer, filePathBuffer);

    system(cmdBuffer);
}

void listLocks(char *filePath) {
    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    printf("\nLocks on file: %s\n%-8s%-14s%-14s%s\n", filePath, "Type", "Start Byte", "End Byte", "PID");

    off_t endPos = lseek(fd, 0, SEEK_END);
    for (off_t pos = 0; pos < endPos; ++pos) {
        struct flock flock = doSetFileByteLock(filePath, F_GETLK, F_WRLCK, pos);

        if (flock.l_len == -1) {
            fprintf(stderr, "Error listing locks\n");
            exit(EXIT_FAILURE);
        }

        if (flock.l_type == F_UNLCK) continue;

        char *lockType = (flock.l_type == F_RDLCK) ? "READ" : "WRITE";
        off_t lockStartByte = lseek(fd, flock.l_start, flock.l_whence);
        off_t lockEndByte = lockStartByte + flock.l_len;
        printf("%-8s%-14ld%-14ld%d\n", lockType, lockStartByte, lockEndByte, flock.l_pid);

        pos = lockEndByte;
    }
    printf("\n");
}


int main(int argc, char **argv) {
    if (argc == 3 && strcmp(argv[0], argv[1]) == 0) {
        listLocks(argv[2]);
        return 0;
    }

    ActionType input = (ActionType) -1;

    do {
        printActions();

        if (scanf("%d", (int *) &input) == EOF) {
            if (ferror(stdin) != 0) {
                perror("Error getting input");
                clearerr(stdin);
            }
            input = (ActionType) -1;
        }
        discardLine(stdin);

        switch (input) {
            case NON_BLOCKING_READ_LOCK:
                nonBlockingReadLock();
                break;
            case BLOCKING_READ_LOCK:
                blockingReadLock();
                break;
            case NON_BLOCKING_WRITE_LOCK:
                nonBlockingWriteLock();
                break;
            case BLOCKING_WRITE_LOCK:
                blockingWriteLock();
                break;
            case LIST_LOCKS:
                listLocksInvoker(argv[0]);
                break;
            case FREE_LOCK:
                freeLock();
                break;
            case READ_CHAR:
                readChar();
                break;
            case WRITE_CHAR:
                writeChar();
                break;
            case QUIT:
                break;
            default:
                fprintf(stderr, "Invalid command\n");
                break;
        }
    } while (input != QUIT);

    if (filePathBuffer != NULL) {
        free(filePathBuffer);
    }

    return 0;
}