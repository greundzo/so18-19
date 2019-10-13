//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

typedef struct student_data
{
    pid_t student_pid;
    int matricule;
    int vote_So;
    struct sembuf * ops;
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

struct student_data * connect(int id)
{
    return (struct student_data *)shmat(id,NULL,0);
}

int create_sem()
{
    return semget(SEM, SEM, IPC_CREAT);
}

int take_sem(int s_id, struct student_data * st_id, int index)
{
    st_id[index].ops->sem_num = 1;
    st_id[index].ops->sem_op = -1;
    return semop(s_id, st_id[index].ops, 1);
}

int release_sem(int s_id, struct student_data * st_id, int index)
{
    st_id[index].ops->sem_num = 1;
    st_id[index].ops->sem_op = 1;
    return semop(s_id, st_id[index].ops, 1);
}