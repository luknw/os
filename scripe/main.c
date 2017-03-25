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

void performExecCmd(ExecCmd cmd) {
    int status = 0;
    struct rusage rusage;

    pid_t pid = fork();
    if (pid == 0) {
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

        fprintf(stderr, "%-8s%-10d%-15s%4li%s%-10li%-15s%4li%s%-10li\n",
                "PID:", pid,
                "User time:", rusage.ru_utime.tv_sec, ".", rusage.ru_utime.tv_usec,
                "System time:", rusage.ru_stime.tv_sec, ".", rusage.ru_stime.tv_usec);

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

void interpret(FILE *script) {
    while (safe_getline_content(&lineBuffer, &lineBufferSize, script) != EOF) {
        interpretLine(lineBuffer, lineBufferSize);
        lineCount++;
    }
}


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("%s\n", USAGE);
        return 0;
    }

    FILE *sScript = safe_fopen(argv[1], "r");

    interpret(sScript);

    safe_fclose(sScript);

    return 0;
}
