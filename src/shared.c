//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

struct sembuf semwait = { 0, -1, SEM_UNDO}; // semwait
struct sembuf semsignal = { 0, +1, SEM_UNDO}; // semsignal

//genera un intero casuale
int generate_random_integer(int minNum, int maxNum, pid_t pid)
{
    int span = maxNum - minNum + 1;
    int snNum = maxNum - maxNum % span;
    int a;
    srand(pid);
    do a = rand(); while (a >= snNum);
    return minNum + a % span;
}

/* GESTIONE DELLA SHM */
int create_memory()
{
    return shmget(MEM, sizeof(struct shared), IPC_CREAT|0666);
}

void * connect(int id)
{
    return shmat(id, (void *)0, 0);
}

/* GESTIONE DEI SEMAFORI */
int create_sem()
{
	return semget(SEM, 5, IPC_CREAT|0666); 
}

void sem_init_val(int index , union semun value)
{
     if(semctl(semid, index, SETVAL, value) == -1) {
     	strerror(errno);
     } 	
}

int take_sem(int num)
{
    return semop(semid, &semwait, 1);
}

int release_sem(int num)
{
    return semop(semid, &semsignal, 1);
}
