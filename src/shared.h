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
#include <sys/wait.h>
#include <semaphore.h>

int generate_random_integer(int minNum, int maxNum, pid_t pid) {
    int span = maxNum - minNum + 1;
    int snNum = maxNum - maxNum % span;
    int a;
    srand(pid);
    do a = rand(); while (a >= snNum);
    return minNum + a % span;
}