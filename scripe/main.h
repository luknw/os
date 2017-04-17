//
// Created by luknw on 3/24/17.
//

#ifndef SCRIPE_MAIN_H
#define SCRIPE_MAIN_H


typedef enum EnvCmdAction {
    SET,
    UNSET
} EnvCmdAction;

typedef struct EnvCmd {
    EnvCmdAction action;
    char *varName;
    char *varValue;
} EnvCmd;

typedef struct ExecCmd {
    char *cmd;
    char **argv;
    pid_t pid;
} ExecCmd;

typedef struct Pipeline {
    ExecCmd *cmds;
    int cmdCount;
} Pipeline;

typedef struct Pipe {
    int sink;
    int tap;
} Pipe;


#endif //SCRIPE_MAIN_H
