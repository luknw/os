//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_MAIN_H
#define IOBENCHMARK_MAIN_H

//todo resolve typedef issue
//typedef struct argp argp;
typedef struct argp_option argp_option;
typedef struct argp_state argp_state;

typedef enum Action Action;
typedef enum ActionProvider ActionProvider;
typedef struct Arguments Arguments;


enum Action {
    GENERATE,
    SHUFFLE,
    SORT
};

enum ActionProvider {
    LIBRARY,
    SYSTEM
};

struct Arguments {
    Action action;
    ActionProvider provider;
    int recordCount;
    int recordSize;
    char *filePath;
};


#endif //IOBENCHMARK_MAIN_H_H
