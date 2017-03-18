//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_SAFEFILE_H
#define IOBENCHMARK_SAFEFILE_H

#include <stdio.h>
#include <stdlib.h>


FILE *safe_fopen(const char *__restrict __filename, const char *__restrict __modes);


#endif //IOBENCHMARK_SAFEFILE_H
