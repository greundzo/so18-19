//
// Created by greundzo on 08/10/19.
//
#define TEST_ERROR		if (errno) {fprintf(stderr, \
						"%s:%d: PID=%5d: Error %d (%s)\n",\
					   	__FILE__,\
					   	__LINE__,\
					   	getpid(),\
					   	errno,\
					   	strerror(errno));}

#define _XOPEN_SOURCE
#ifndef TEST
#define POP_SIZE 10
#endif //test

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

#define GROUP 3
#define MEM 30
#define SEM 5

typedef struct student_data {
    pid_t student_pid;
    int class;
    int matricule;
    int vote_So;
}student_data;

typedef struct shared {
    struct student_data stdata[POP_SIZE];
    int pc;
}shared;

// Shared global variables and functions
int memid, semid;
char arguments[2];
int generate_random_integer(int minNum, int maxNum, pid_t pid);

// Shared Memory functions declaration
int create_memory();
void * connect(int id);

// Semaphore functions declaration 
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};
int create_sem();
void sem_init_val(int index, union semun value);
int take_sem(int num);
int release_sem(int num);



