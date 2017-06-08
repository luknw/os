//
// Created by luknw on 2017-06-08
//

#define _BSD_SOURCE

#include <endian.h>
#include <cstdint>
#include "Metadata.h"


Metadata::Metadata() {}

Metadata::Metadata(int dataType, ssize_t dataSize) {
    this->dataType_ = htobe32((unsigned int) dataType);
    this->dataSize_ = htobe64((uint64_t) dataSize);
}

int Metadata::dataType() {
    return be32toh((unsigned int) dataType_);
}

ssize_t Metadata::dataSize() {
    return be64toh((uint64_t) dataSize_);
}
