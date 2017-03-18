//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_MAIN_H
#define IOBENCHMARK_MAIN_H

typedef struct argp argp;
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
    size_t recordCount;
    size_t recordSize;
    char *filePath;
};


void generate(size_t recordCount, size_t recordSize, char *filePath);

void shuffleLib(size_t recordCount, size_t recordSize, char *filePath);

void shuffleSys(size_t recordCount, size_t recordSize, char *filePath);

void sortLib(size_t recordCount, size_t recordSize, char *filePath);

void sortSys(size_t recordCount, size_t recordSize, char *filePath);


#endif //IOBENCHMARK_MAIN_H_H
