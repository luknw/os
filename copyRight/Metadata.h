//
// Created by luknw on 2017-06-08
//

#ifndef COPYRIGHT_MESSAGE_H
#define COPYRIGHT_MESSAGE_H


#include <cstdio>

class Metadata {
public:
    Metadata();

    Metadata(int dataType, ssize_t dataSize);

    int dataType();

    ssize_t dataSize();

private:
    int dataType_;
    ssize_t dataSize_;
};


#endif //COPYRIGHT_MESSAGE_H
