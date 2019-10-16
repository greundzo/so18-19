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

int generate_matr(pid_t pid)
{
    srand(pid);
    return (rand() % 900000) + 100000;
}

void signalhandler(int signum){

    switch(signum){
        case SIGALRM:
            printf("End simulation.\n");
            int * votesAdE = malloc(POP_SIZE * sizeof(int)); 
            int average_AdE = 0;
            for(int i = 0; i < POP_SIZE; i++){
                //kill(pStudentData->stdata[i].student_pid, SIGUSR1);
                votesAdE[i] = pStudentData->stdata[i].vote_AdE;
                average_AdE += votesAdE[i];
                kill(pStudentData->stdata[i].student_pid, SIGKILL);
            }
            printf("%d\n", pStudentData->pc);
            printf("%s %d \n", "AdE grades average", average_AdE/POP_SIZE);

            semctl(semid, 2, IPC_RMID);
            shmdt(pStudentData);
            shmctl(memid, IPC_RMID, NULL);
            free(votesAdE);
            exit(EXIT_SUCCESS);
            break;
        /*
        case SIGUSR1:
            if(msgrcv(msqvote_id, &msg_prof, sizeof(msg_prof)- sizeof(long), getpid(), 0) == -1){
                printf("%d: Errore nella ricezione del voto", getpid());
                TEST_ERROR;
            }
            voto_prj = msg_prof.voto;
            print_info(indice);

            if(sh_stud->stud[indice].leader == 1)
                free(indexOfMembs);

            if((shmdt(sh_stud)) == -1){
                printf("Errore nella detaching dalla shmem:\n");
                TEST_ERROR;
            }
            else{
                //printf("Mi sono staccato con successo\n");
            }

            if(errno != ENOMSG)
                TEST_ERROR;
            exit(EXIT_SUCCESS);
        */
        case SIGINT:
            for(int i = 0; i < POP_SIZE; i++){
                kill(pStudentData->stdata[i].student_pid, SIGTERM);
            }
            semctl(semid, 2, IPC_RMID);
            shmdt(pStudentData);
            shmctl(memid, IPC_RMID, NULL);
            //msq_rm(msq_id);
            //msq_rm(msqvote_id);
            //system("make rm");
            exit(EXIT_FAILURE);
    }
}

/* GESTIONE DELLA SHM */
int create_memory()
{
    int sh;
    if ( (sh =shmget(MEM, sizeof(struct shared), 0666|IPC_CREAT)) == -1) {
        TEST_ERROR
    }
    return sh;
}

void * connect(int id)
{
    return shmat(id, 0, 0);
}

/* GESTIONE DEI SEMAFORI */

int create_sem()
{
    int sm;
	if (( sm = semget(SEM, 2, 0666|IPC_CREAT)) == -1) {
        TEST_ERROR
    } 
    return sm;
}

void sem_init_val(int index, int value)
{
    if(semctl(semid, index, SETVAL, value) == -1) {
        TEST_ERROR
    } 	
}

int take_sem(int semid, int num)
{   
    int took;   
    ops.sem_num = num;
    ops.sem_op = -1;
    if ((took = semop(semid, &ops, (size_t)1) == -1)) {
        TEST_ERROR
    }
    return took;
}

int release_sem(int semid, int num)
{   
    int released;
    ops.sem_num = num;
    ops.sem_op = +1;
    if ((released = semop(semid, &ops, (size_t)1) == -1)) {
        TEST_ERROR;
    }
    return released;
}

/* GESTIONE DEI MESSAGGI */
int create_queue () //creates queue and returns id
{
    int q;
    if (( q = msgget(MSG, IPC_CREAT | 0666)) == -1) {
        TEST_ERROR;
    }
    return q;
}

int remove_queue (int id) //removes queue and returns id
{
    int rm;
    if (( rm =msgctl(id, IPC_RMID, NULL)) == -1) {
        TEST_ERROR
    }
    return rm;
}

int info_queue (int id) //get the status of the queue
{
    int c;
    if (( c = msgctl(id, IPC_STAT, buffer)) == -1)
	TEST_ERROR
    return c;
}

int send_msg (int id) //send a message in the queue
{
    int s;
    if (( s = msgsnd(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0)) == - 1)
	TEST_ERROR
    return s;
}

int receive_msg (int id) //receive a message in the queue
{
    int r;
    if (( r = msgrcv(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0, 0)) == - 1)
	TEST_ERROR
    return r;
}













