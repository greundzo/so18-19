//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

int generate_random_integer(int minNum, int maxNum, pid_t pid)
{
    int span = maxNum - minNum + 1;
    int snNum = maxNum - maxNum % span;
    int a;
    srand(pid);
    do a = rand(); while (a >= snNum);
    return minNum + a % span;
}

int create_memory(int student_number)
{
    return shmget(MEM, student_number * sizeof(struct student_data), IPC_CREAT|0600);
}

void * connect(int id)
{
    return shmat(id, (void *)0, 0);
}

int create_sem()
{
    ops = malloc(sizeof(struct sembuf));
    return semget(SEM, SEM, IPC_CREAT);
}

int take_sem(int s_id)
{
    ops->sem_num = 1;
    ops->sem_op = -1;
    return semop(s_id, ops, 1);
}

int release_sem(int s_id)
{
    ops->sem_num = 1;
    ops->sem_op = 1;
    return semop(s_id, ops, 1);
}
