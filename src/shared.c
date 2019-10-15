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
    do a = rand(); while (a >= snNum);
    return minNum + a % span;
}
/* GESTIONE DELLA SHM */
int create_memory(int student_number)
{
    return shmget(MEM, student_number * sizeof(struct student_data), IPC_CREAT|0600);
}

void * connect(int id)
{
    return shmat(id, (void *)0, 0);
}
/* GESTIONE DEI SEMAFORI */
int create_sem(key_t key, int nsems){
    
      int s = semget(key, nsems, IPC_CREAT);
      
      if(s == -1){
      	printf("Creation of semaphore failed : \n"); 
        exit(EXIT_FAILURE);
      }
      return s;
}
void sem_init_val(int s_id , int s_index , int value){
     if(semctl(s_id,s_index,value) == -1){
     	printf("Errore in sempahore init:\n");
     exit(-1);
     } 	
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
