//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

typedef struct student_data {
    pid_t student_pid;
    int matricule;
    int voto_So;
}student_data;

int generate_random_integer(int minNum, int maxNum, pid_t pid)
{
    int span = maxNum - minNum + 1;
    int snNum = maxNum - maxNum % span;
    int a;
    srand(pid);
    do a = rand(); while (a >= snNum);
    return minNum + a % span;
}

int create_memory(int size)
{
    return shmget(MEM, size*sizeof(struct student_data), IPC_CREAT|IPC_EXCL|0666);
}

void * connect(int id)
{
    return (struct student_data*)shmat(id,NULL,0);
}