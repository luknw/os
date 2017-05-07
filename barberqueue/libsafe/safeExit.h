//
// Created by luknw on 2017-05-03
//

#ifndef SAFE_EXIT_H
#define SAFE_EXIT_H


void safe_on_exit(void (*exitHandler)(int, void *), void *exitHandlerArgument);


#endif //SAFE_EXIT_H
