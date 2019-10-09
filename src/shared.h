//
// Created by greundzo on 08/10/19.
//

#ifndef SO1920_SHARED_H
#define SO1920_SHARED_H

#endif //SO1920_SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <semaphore.h>

// Generates and prints 'count' random
// numbers in range [lower, upper].
void printRandoms(int lower, int upper,
                  int count)
{
    int i;
    for (i = 0; i < count; i++) {
        int num = (rand() %
                   (upper - lower + 1)) + lower;
    }
}