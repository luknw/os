//
// Created by luknw on 2017-05-03
//

#ifndef SAFE_EXIT_H
#define SAFE_EXIT_H


#include <stdint.h>


#define EXIT_FREE(ptr) safe_on_exit(exit_freeWrapper, ptr)

void exit_freeWrapper(int status, void *ptr);


#define EXIT_CLOSE(fd) safe_on_exit(exit_closeWrapper, (int) (uintptr_t) (fd))

void exit_closeWraspper(int status, void *fd);


void safe_on_exit(void (*exitHandler)(int, void *), void *exitHandlerArgument);


#endif //SAFE_EXIT_H
