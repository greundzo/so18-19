//
// Created by greundzo on 08/10/19.
//

#ifndef SO1920_SHARED_H
#define SO1920_SHARED_H

#endif //SO1920_SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#define MEM 30

struct student_data;
int generate_random_integer(int minNum, int maxNum, pid_t pid);
int create_memory(int size);
void * connect(int id);