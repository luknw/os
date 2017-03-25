#include <stdio.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdbool.h>

#include "main.h"
#include "libsafe/safe.h"


static const char *USAGE = "scripe SCRIPET_PATH\n"
        "\tInterprets the scripet at SCRIPET_PATH.\n";

static const size_t INITIAL_ARGV_LEN = 4;
static const char *WHITESPACE = " \t\n";


static ssize_t lineBufferSize = 0;
static char *lineBuffer;

static unsigned int lineCount = 1;


enum CmdLimitIndices {
    MEMORY,
    CPU
};

static struct rlimit cmdLimits[] = {{RLIMIT_AS},
                                    {RLIMIT_CPU}};


EnvCmd parseEnvCmd(char *cmdLine) {
    EnvCmd cmd;
    cmd.varName = strtok(cmdLine + 1, WHITESPACE);
    cmd.varValue = strtok(NULL, WHITESPACE);

    cmd.action = (cmd.varValue != NULL) ? SET : UNSET;

    return cmd;
}

ExecCmd parseExecCmd(char *cmdLine) {
    ExecCmd cmd;
    cmd.cmd = strtok(cmdLine, WHITESPACE);

    size_t argvLen = INITIAL_ARGV_LEN;
    cmd.argv = safe_calloc(argvLen, sizeof(char *));
    cmd.argv[0] = cmd.cmd;

    char *token;
    int tokenCount = 1;
    while ((token = strtok(NULL, WHITESPACE)) != NULL) {
        if (tokenCount + 1 >= argvLen) {
            argvLen *= 2;
            cmd.argv = safe_realloc(cmd.argv, argvLen * sizeof(char *));
        }

        if (token[0] == '$') token = getenv(token + 1);

        cmd.argv[tokenCount++] = token;
    }

    return cmd;
}


void performEnvCmd(EnvCmd cmd) {
    if (cmd.varName == NULL) return;

    switch (cmd.action) {
        case SET:
            if (setenv(cmd.varName, cmd.varValue, true) == -1) {
                perror("Error setting environment variable");
                exit(EXIT_FAILURE);
            };
            break;
        case UNSET:
            if (unsetenv(cmd.varName) == -1) {
                perror("Error unsetting environment variable");
                exit(EXIT_FAILURE);
            };
            break;
    }
}

static void setResourceLimits() {
    if (setrlimit(RLIMIT_AS, &cmdLimits[MEMORY]) == -1) {
        perror("Error setting memory limit");
        exit(EXIT_FAILURE);
    }
    if (setrlimit(RLIMIT_CPU, &cmdLimits[CPU]) == -1) {
        perror("Error setting cpu time limit");
        exit(EXIT_FAILURE);
    }
}

void performExecCmd(ExecCmd cmd) {
    int status = 0;
    struct rusage rusage;

    pid_t pid = fork();
    if (pid == 0) {
        setResourceLimits();
        if (execvp(cmd.cmd, cmd.argv) == -1) {
            fprintf(stderr, "Cannot execute %s: %s\n", cmd.cmd, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        pid = wait3(&status, 0, &rusage);
        fprintf(stderr, "%s%2s%d%-5s", "Child terminated", "(", WEXITSTATUS(status), ")");

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "\nError executing command at line: %u\n", lineCount);
            exit(WEXITSTATUS(status));
        }

        fprintf(stderr, "%-8s%-10d%20s%9li%-10s%-15s%4li%s%li%-5s%-15s%4li%s%li%-5s\n",
                "PID:", pid,
                "Max resident memory:", rusage.ru_maxrss, " kB",
                "User time:", rusage.ru_utime.tv_sec, ".", rusage.ru_utime.tv_usec, " s",
                "System time:", rusage.ru_stime.tv_sec, ".", rusage.ru_stime.tv_usec, " s");

        safe_free(cmd.argv);
    } else {
        perror("Error while forking");
        exit(EXIT_FAILURE);
    }
}


void interpretLine(char *line, ssize_t lineLen) {
    if (lineLen == 0) return;

    if (line[0] == '#') {
        performEnvCmd(parseEnvCmd(line));
    } else {
        performExecCmd(parseExecCmd(line));
    }
}

void interpret(char *filePath) {
    FILE *sScript = safe_fopen(filePath, "r");

    while (safe_getline_content(&lineBuffer, &lineBufferSize, sScript) != EOF) {
        interpretLine(lineBuffer, lineBufferSize);
        lineCount++;
    }

    safe_fclose(sScript);
}


void findResourceLimits(int argc, char **argv) {
    if (argc >= 4) {
        cmdLimits[MEMORY].rlim_cur = cmdLimits[MEMORY].rlim_max = (rlim_t) atoll(argv[2]);
        cmdLimits[CPU].rlim_cur = cmdLimits[CPU].rlim_max = (rlim_t) atoll(argv[3]);
        return;
    }

    if (getrlimit(RLIMIT_AS, &cmdLimits[MEMORY]) == -1) {
        perror("Error getting resource limit");
        exit(EXIT_FAILURE);
    }
    if (getrlimit(RLIMIT_CPU, &cmdLimits[CPU]) == -1) {
        perror("Error getting resource limit");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("%s\n", USAGE);
        return 0;
    }

    findResourceLimits(argc, argv);
    interpret(argv[1]);

    return 0;
}
