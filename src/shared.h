//
// Created by greundzo on 08/10/19.
//
#define _XOPEN_SOURCE
#ifdef TEST
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

typedef struct student_data
{
    pid_t student_pid;
    int class;
    int matricule;
    int vote_So;
}student_data;

// Shared global variables
int POP_SIZE, memid, semid;
int main(int argc, char ** argv);
char arguments[2];
int generate_random_integer(int minNum, int maxNum, pid_t pid);

// Shared Memory functions declaration
int create_memory(int size);
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



