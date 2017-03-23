//#define USE_NFTW

#ifdef USE_NFTW

#define _XOPEN_SOURCE 500

#include <ftw.h>

#endif

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>


#define WD_BUFFER_SIZE PATH_MAX * sizeof(char)
static char workingDirBuffer[WD_BUFFER_SIZE];


static void mode_t_toString(mode_t mode, char *str) {
    str[0] = (char) (mode & S_IRUSR ? 'r' : '-');
    str[1] = (char) (mode & S_IWUSR ? 'w' : '-');
    str[2] = (char) (mode & S_ISUID
                     ? (mode & S_IXUSR ? 's' : 'S')
                     : (mode & S_IXUSR ? 'x' : '-'));
    str[3] = (char) (mode & S_IRGRP ? 'r' : '-');
    str[4] = (char) (mode & S_IWGRP ? 'w' : '-');
    str[5] = (char) (mode & S_ISGID
                     ? (mode & S_IXGRP ? 's' : 'S')
                     : (mode & S_IXGRP ? 'x' : '-'));
    str[6] = (char) (mode & S_IROTH ? 'r' : '-');
    str[7] = (char) (mode & S_IWOTH ? 'w' : '-');
    str[8] = (char) (mode & S_ISVTX
                     ? (mode & S_IXOTH ? 't' : 'T')
                     : (mode & S_IXOTH ? 'x' : '-'));
    str[9] = '\0';
}


#ifdef USE_NFTW

static void printFileStat(const char *filePath, const struct stat *sb) {
    char permissions[10];
    mode_t_toString(sb->st_mode, permissions);
    char *mTime = ctime(&sb->st_mtime);

    printf("%s\n%10li\t%9s\t%25s", filePath, sb->st_size, permissions, mTime);
}

static size_t maxFileSize;


static int processFile(const char *filePath, const struct stat *sb, int typeFlag, struct FTW *ftwBuf) {
    if (!typeFlag == FTW_F || (maxFileSize != -1 && sb->st_size > maxFileSize)) return 0;

    printFileStat(filePath, sb);
    return 0;
}


void fileInfo(char *rootDirectory, size_t maxSize) {
    if (realpath(rootDirectory, workingDirBuffer) == NULL) {
        perror("Error resolving absolute path");
        exit(EXIT_FAILURE);
    }

    maxFileSize = maxSize;

    nftw(workingDirBuffer, processFile, 256, FTW_PHYS);
}

#else

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>


static struct stat fileStatBuffer;


static void printFileStat(struct dirent *fileDirectoryEntry) {
    char permissions[10];
    mode_t_toString(fileStatBuffer.st_mode, permissions);
    char *mTime = ctime(&fileStatBuffer.st_mtime);

    if (strcmp("/", workingDirBuffer) != 0) printf("%s", workingDirBuffer);
    printf("/%s\n%10li\t%9s\t%25s", fileDirectoryEntry->d_name, fileStatBuffer.st_size, permissions, mTime);
}


static int openDirectory(int fdParentDir, char *dirName, int *fdDir, DIR **dirStream) {
    *fdDir = openat(fdParentDir, dirName, O_DIRECTORY);
    if (*fdDir == -1) {
        if (errno == EACCES) return -1;
        perror("Error opening directory file");
        exit(EXIT_FAILURE);
    }

    *dirStream = fdopendir(*fdDir);
    if (*dirStream == NULL) {
        if (errno == EACCES) return -1;
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    return 0;
}


static void changeCwd(int fdNewCwd) {
    if (fchdir(fdNewCwd) == -1) {
        perror("Error changing working directory");
        exit(EXIT_FAILURE);
    }

    if (getcwd(workingDirBuffer, WD_BUFFER_SIZE) == NULL) {
        perror("Error resolving absolute working path");
        exit(EXIT_FAILURE);
    }
}


static void fileInfoAt(int fdParentDir, char *dirName, long maxSize) {
    int fdDir;
    DIR *dir;

    if (openDirectory(fdParentDir, dirName, &fdDir, &dir) == -1) return;
    changeCwd(fdDir);

    struct dirent *directoryEntry;

    errno = 0;
    while ((directoryEntry = readdir(dir)) != NULL) {
        if (fstatat(fdDir, directoryEntry->d_name, &fileStatBuffer, AT_SYMLINK_NOFOLLOW) == -1) {
            perror("Error getting info about file");
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(fileStatBuffer.st_mode) && (maxSize == -1 || fileStatBuffer.st_size <= maxSize)) {
            printFileStat(directoryEntry);
        } else if (S_ISDIR(fileStatBuffer.st_mode)
                   && strcmp(directoryEntry->d_name, ".") != 0
                   && strcmp(directoryEntry->d_name, "..") != 0) {

            fileInfoAt(fdDir, directoryEntry->d_name, maxSize);
            changeCwd(fdDir);
        }

        errno = 0;
    }
    if (errno != 0) {
        perror("Error reading directory");
        exit(EXIT_FAILURE);
    }

    if (closedir(dir) == -1) {
        perror("Error closing directory");
        exit(EXIT_FAILURE);
    }
}


void fileInfo(char *rootDirectory, long maxSize) {
    fileInfoAt(AT_FDCWD, rootDirectory, maxSize);
}

#endif


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("USAGE: fileInfo DIRECTORY [MAX_FILE_SIZE]\n"
                       "Prints info about files of size not larger than MAX_FILE_SIZE,\n"
                       "that reside in DIRECTORY or any of its direct or indirect subdirectories.\n"
                       "Considers only files and directories, that the invoking user can access.\n"
                       "If MAX_FILE_SIZE is not provided, there is no file size limit.\n");
        return 0;
    }

    char *rootDirectory = argv[1];
    size_t maxSize = (size_t) ((argc > 2) ? atoll(argv[2]) : -1);

    fileInfo(rootDirectory, maxSize);

    return 0;
}
