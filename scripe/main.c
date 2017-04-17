#define _BSD_SOURCE

#include <stdio.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>

#include "main.h"
#include "libsafe/safe.h"
#include "libhashmap/hashmap.h"


static const char *USAGE = "scripe SCRIPET_PATH\n"
        "\tInterprets the scripet at SCRIPET_PATH.\n";

static const size_t INITIAL_CMD_ARGV_LEN = 4;
static const size_t INITIAL_CMD_PIPELINE_LEN = 4;
static const char *WHITESPACE = " \t\n";
static const char *PIPELINE_DELIMITERS = "|";


static ssize_t lineBufferSize = 0;
static char *lineBuffer;

static unsigned int lineCount = 1;


enum CmdLimitIndices {
    MEMORY,
    CPU
};

static struct rlimit cmdLimits[] = {{RLIMIT_AS},
                                    {RLIMIT_CPU}};


EnvCmd parseEnvCmd(char *cmdString) {
    assert(cmdString != NULL);

    EnvCmd cmd;
    cmd.varName = strtok(cmdString + 1, WHITESPACE);
    cmd.varValue = strtok(NULL, WHITESPACE);

    cmd.action = (cmd.varValue != NULL) ? SET : UNSET;

    return cmd;
}

int parseArray(char *parsed, char ***pArray, size_t arraySize, size_t offset, const char *delimiters) {
    assert(arraySize >= 2);
    assert(offset <= arraySize - 1);

    char *token;
    int tokenCount = 0;
    while ((token = strsep(&parsed, delimiters)) != NULL) {
        if (strcmp("", token) == 0) continue;

        if (offset + tokenCount >= arraySize - 1) {
            arraySize *= 2;
            *pArray = safe_realloc(*pArray, arraySize * sizeof(char *));
        }

        (*pArray)[offset + tokenCount++] = token;
    }
    (*pArray)[offset + tokenCount] = NULL;

    return tokenCount;
}

ExecCmd parseExecCmd(char *cmdString) {
    assert(cmdString != NULL);

    cmdString += strspn(cmdString, WHITESPACE);

    ExecCmd cmd;
    cmd.cmd = strsep(&cmdString, WHITESPACE);
    cmd.pid = 0;

    cmd.argv = safe_calloc(INITIAL_CMD_ARGV_LEN, sizeof(char *));
    cmd.argv[0] = cmd.cmd;

    parseArray(cmdString, &cmd.argv, INITIAL_CMD_ARGV_LEN, 1, WHITESPACE);

    for (char **token = cmd.argv + 1; *token != NULL; ++token) {
        if (**token == '$') *token = getenv(*token + 1);
    }

    return cmd;
}

Pipeline parsePipeline(char *line) {
    assert(line != NULL);

    Pipeline pipeline;
    pipeline.cmdCount = 0;

    size_t cmdsLen = INITIAL_CMD_PIPELINE_LEN;
    pipeline.cmds = safe_calloc(cmdsLen, sizeof(ExecCmd));

    char *execCmdToken;
    while ((execCmdToken = strsep(&line, PIPELINE_DELIMITERS)) != NULL) {
        if (pipeline.cmdCount >= cmdsLen) {
            cmdsLen *= 2;
            pipeline.cmds = safe_realloc(pipeline.cmds, cmdsLen * sizeof(ExecCmd));
        }

        pipeline.cmds[pipeline.cmdCount++] = parseExecCmd(execCmdToken);
    }

    return pipeline;
}

void runEnvCmd(EnvCmd cmd) {
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

void setResourceLimits(void) {
    if (setrlimit(RLIMIT_AS, &cmdLimits[MEMORY]) == -1) {
        perror("Error setting memory limit");
        exit(EXIT_FAILURE);
    }
    if (setrlimit(RLIMIT_CPU, &cmdLimits[CPU]) == -1) {
        perror("Error setting cpu time limit");
        exit(EXIT_FAILURE);
    }
}

Pipe Pipe_new(void) {
    int fds[2];
    if (pipe(fds) == -1) {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    return (Pipe) {fds[1], fds[0]};
}

bool isStandard(int fd) {
    return fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO;
}

pid_t runExecCmd(ExecCmd *cmd, int inputFd, int outputFd) {
    assert(cmd != NULL);

    pid_t pid = fork();
    if (pid == 0) {
        safe_dup2(inputFd, STDIN_FILENO);
        safe_dup2(outputFd, STDOUT_FILENO);

        setResourceLimits();

        if (execvp(cmd->cmd, cmd->argv) == -1) {
            fprintf(stderr, "Cannot execute %s: %s\n", cmd->cmd, strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        if (!isStandard(inputFd)) safe_close(inputFd);
        if (!isStandard(outputFd)) safe_close(outputFd);

        return cmd->pid = pid;
    }

    perror("Error while forking");
    exit(EXIT_FAILURE);
}

static size_t hashExecCmdByPid(void *pid) {
    long long pidValue = (long long) pid;
    size_t hash = (size_t) pidValue;

    if (sizeof(size_t) >= sizeof(void *)) return hash;

    while (pidValue != 0) {
        pidValue >>= sizeof(size_t) * 8;
        hash ^= (size_t) pid;
    }

    return hash;
}

void *fitVoidPointer(void *pValue, size_t valueSize) {
    long long value = 0;
    memcpy(&value, pValue, valueSize);

    if (sizeof(void *) >= valueSize) return (void *) value;

    long long pointerValue = value;
    while (value != 0) {
        value >>= sizeof(void *) * 8;
        pointerValue ^= value;
    }

    return (void *) pointerValue;
}

HashMap *runPipeline(Pipeline pipeline) {
    HashMap *cmdsByPid = HashMap_new(hashExecCmdByPid);

    int inputFd = STDIN_FILENO, outputFd;
    int i;
    for (i = 0; i < pipeline.cmdCount - 1; ++i) {
        Pipe p = Pipe_new();
        outputFd = p.sink;

        runExecCmd(&pipeline.cmds[i], inputFd, outputFd);
        HashMap_add(cmdsByPid, fitVoidPointer(&pipeline.cmds[i].pid, sizeof(pid_t)), &pipeline.cmds[i]);

        inputFd = p.tap;
    }
    outputFd = STDOUT_FILENO;
    runExecCmd(&pipeline.cmds[i], inputFd, outputFd);
    HashMap_add(cmdsByPid, fitVoidPointer(&pipeline.cmds[i].pid, sizeof(pid_t)), &pipeline.cmds[i]);

    return cmdsByPid;
}

void waitForPipeline(HashMap *cmdsByPid) {
    int status = 0;
    struct rusage rusage;

    while (!HashMap_isEmpty(cmdsByPid)) {
        pid_t pid = wait3(&status, 0, &rusage);
        ExecCmd *cmd = HashMap_remove(cmdsByPid, fitVoidPointer(&pid, sizeof(pid)));

        fprintf(stderr, "%s%s%2s%d%-5s", cmd->cmd, " terminated", "(", WEXITSTATUS(status), ")");

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "\nError executing command at line %u\n", lineCount);
            exit(WEXITSTATUS(status));
        }

        fprintf(stderr, "%-8s%-10d%20s%9li%-10s%-15s%4li%s%li%-5s%-15s%4li%s%li%-5s\n",
                "PID:", pid,
                "Max resident memory:", rusage.ru_maxrss, " kB",
                "User time:", rusage.ru_utime.tv_sec, ".", rusage.ru_utime.tv_usec, " s",
                "System time:", rusage.ru_stime.tv_sec, ".", rusage.ru_stime.tv_usec, " s");

        cmd->pid = 1;
        safe_free(cmd->argv);
    }
}

void interpretLine(char *line, ssize_t lineLen) {
    size_t whiteSpaceLen = strspn(line, WHITESPACE);
    lineLen -= whiteSpaceLen;
    if (lineLen == 0) return;
    line += whiteSpaceLen;

    if (line[0] == '#') {
        runEnvCmd(parseEnvCmd(line));
    } else {
        Pipeline pipeline = parsePipeline(line);

        HashMap *cmdsByPid = runPipeline(pipeline);
        waitForPipeline(cmdsByPid);

        HashMap_delete(cmdsByPid);
        safe_free(pipeline.cmds);
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
