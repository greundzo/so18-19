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

int read_opt_conf(char *string){
  FILE * opt = fopen("opt.conf", "r");
  TEST_ERROR;
  int val;
  char * read_string = malloc(sizeof(char)*20);	
  //	for(read_string = getc(opt); read_string != EOF ; read_string = getc(opt)){
  	while(fscanf(opt, "%s %i", read_string, &val)!=EOF){     	
             if(strcmp(string, read_string) == 0){
		  fclose(opt);
		  free(read_string);	
		  return val;
		}		
  	 	
	     }  
	
	fclose(opt);
	free(read_string);
	printf("String not in file \n");
	return -1;
}


int getturn(int matricule)
{
    if (matricule % 2 == 0) {
        return 2;
    } 
    return 1;
}

void printinfo(int index){
    printf("%3i: matricola = %5i   voto_AdE = %2i   voto_prj = %2i\n",
            index, pStudentData->stdata[index].registration_number,
            pStudentData->stdata[index].mark_ca, pStudentData->stdata[index].mark_os);
}

void signalhandler(int signum){

    switch(signum){
        case SIGALRM:
            printf("End simulation.\n");
            marks_os = malloc(POP_SIZE * sizeof(int)); 
            ca_count = calloc(13, sizeof(int));
            os_count = calloc(16, sizeof(int));
            average_ca = 0;
            average_os = 0;
            int val = 0;
            for(int i = 0; i < POP_SIZE; i++) {
                average_ca += pStudentData->stdata[i].mark_ca;
                val = pStudentData->stdata[i].mark_ca - 18;
                //printf("%d", val);
                
                average_os += pStudentData->stdata[i].mark_os;
                os_count[marks_os[i] - 15] += 1;     

                printinfo(i);
                kill(pStudentData->stdata[i].student_pid, SIGUSR1);
            }
            average_ca = average_ca / POP_SIZE;
            average_os = average_os / POP_SIZE;
            semctl(semid, 2, IPC_RMID);
            shmdt(pStudentData);
            shmctl(memid, IPC_RMID, NULL);
            break;

        case SIGUSR1:
            //shmdt(pStudentData);
            exit(EXIT_SUCCESS);

        case SIGINT:
            for(int i = 0; i < POP_SIZE; i++){
                kill(pStudentData->stdata[i].student_pid, SIGTERM);
            }
            semctl(semid, 2, IPC_RMID);
            shmdt(pStudentData);
            shmctl(memid, IPC_RMID, NULL);
            //remove_queue(create_queue());
            //msq_rm(msqvote_id);
            //system("make rm");
            exit(EXIT_FAILURE);
    }
}

/* GESTIONE DELLA SHM */
int create_memory()
{
    int shm;
    if ( (shm =shmget(MEM, sizeof(struct shared), 0666|IPC_CREAT)) == -1) {
        TEST_ERROR
    }
    return shm;
}

void *connect(int id)
{
    return shmat(id, 0, 0);
}

/* GESTIONE DEI SEMAFORI */

int create_sem()
{
    int sem;
	if (( sem = semget(SEM, 2, 0666|IPC_CREAT)) == -1) {
        TEST_ERROR
    } 
    return sem;
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
        TEST_ERROR
    }
    return released;
}

/* GESTIONE DEI MESSAGGI */
int create_queue () //creates queue and returns id
{
    int queue;
    if (( queue = msgget(MSG, IPC_CREAT | 0666)) == -1) {
        TEST_ERROR
    }
    return queue;
}

int remove_queue (int id) //removes queue and returns id
{
    int rm_queue;
    if (( rm_queue =msgctl(id, IPC_RMID, NULL)) == -1) {
        TEST_ERROR
    }
    return rm_queue;
}

int info_queue (int id) //get the status of the queue
{
    int infos;
    if (( infos = msgctl(id, IPC_STAT, &buffer)) == -1) {
	    TEST_ERROR
    }
    return infos;
}

int send_msg (int id, struct message mymsg) //send a message in the queue
{
    int sent;
    if (( sent = msgsnd(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0)) == - 1) {
	    TEST_ERROR
    }
    return sent;
}

int receive_msg (int id, struct message mymsg) //receive a message in the queue
{
    int received;
    if (( received = msgrcv(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0, 0)) == - 1) {
	    TEST_ERROR
    }
    return received;
}

int receive_msg_nowait (int id, struct message mymsg) //receive a message in the queue, no wait
{
    int receivednw;
    if (( receivednw = msgrcv(id, &mymsg, (sizeof(mymsg)-sizeof(long)), 0, IPC_NOWAIT)) == - 1)
	TEST_ERROR
    return receivednw;
}













