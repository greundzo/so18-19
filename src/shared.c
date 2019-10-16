//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

//genera un intero casuale
int generate_random_integer(int minNum, int maxNum, pid_t pid)
{
    int span = maxNum - minNum + 1;
    int snNum = maxNum - maxNum % span;
    int a;
    srand(pid);
    do 
    a = rand(); 
    while (a >= snNum);
    return minNum + a % span;
}

/* GESTIONE DELLA SHM */
int create_memory()
{
    return shmget(MEM, sizeof(struct shared), 0666|IPC_CREAT);
}

void * connect(int id)
{
    return shmat(id, 0, 0);
}

/* GESTIONE DEI SEMAFORI */

int create_sem()
{
	return semget(SEM, 5, 0666|IPC_CREAT); 
}

void sem_init_val(int index)
{
    union semun arg = {1};
    if(semctl(semid, index, SETVAL, arg) == -1) {
        TEST_ERROR;
    } 	
}

int take_sem(int num)
{      
    semwait.sem_num = num;
    semwait.sem_op = 1;
    semwait.sem_flg = SEM_UNDO; 
    TEST_ERROR;
    return semop(semid, &semwait, 1);
}

int release_sem(int num)
{   
    semsignal.sem_num = num;
    semsignal.sem_op = 1;
    semsignal.sem_flg = SEM_UNDO;
    TEST_ERROR;
    return semop(semid, &semsignal, 1);
}

/* GESTIONE DEI MESSAGGI */
int create_queue () //creates queue and returns id
{
    return msgget(MSG, IPC_CREAT | 0666);
}

int remove_queue (int id) //removes queue and returns id
{
    return msgctl(id, IPC_RMID, NULL);
}

int info_queue (int id) //get the status of the queue
{
    return msgctl(id, IPC_STAT, buffer);
}

int send_msg (int id) //send a message in the queue
{
    return msgsnd(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0);
}

int receive_msg (int id) //receive a message in the queue
{
    return msgrcv(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0, 0);
}













