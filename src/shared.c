//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

// Generates a casual number between minNum and maxNum 
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

// Generates the register number
int generate_regnum(pid_t pid) 
{
    srand(pid);
    return (rand() % 900000) + 100000;
}

// Opens the configuration file and returns a random value 
int read_conf(char *str) 
{
    FILE *opt = fopen("../opt.conf", "r");
    TEST_ERROR
    int val;
    char *read_string = malloc(sizeof(char)*20);	

    while(fscanf(opt, "%s %i", read_string, &val) != EOF) {     	
        if(strcmp(str, read_string) == 0) {
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

int get_pref() 
{
    int num = generate_random_integer(1, 100, getpid());
    
    if (num <= read_conf("pref_2")) {
        return 2;
    }

    if (num <= (read_conf("pref_2") + read_conf("pref_3")) && num > read_conf("pref_2")) {
        return 3;
    }

    if(num > read_conf("pref_2") + read_conf("pref_3")) {
        return 4;
    }

    return 0;
}

int get_turn(int matricule)
{
    if (matricule % 2 == 0) {
        return 2;
    } 
    return 1;
}

void printinfo(int index){
    printf("%3i: Register Number = %5i   CA Mark = %2i   OS Mark = %2i\n",
            index, pStudentData->stdata[index].registration_number,
            pStudentData->stdata[index].mark_ca, pStudentData->stdata[index].mark_os);
}

void signalhandler(int signum)
{

    switch(signum){
        case SIGALRM:
            take_sem(semid, 0);
            printf("End simulation.\n");            
            for (int i = 0; i < POP_SIZE; i++) {
                pids[i] = pStudentData->stdata[i].student_pid;
                kill(pids[i], SIGUSR1);
            }
            release_sem(semid, 0);            
            break;

        case SIGUSR1:
            /* here child processes send data to parent*/
            exit(EXIT_SUCCESS);

        case SIGINT:
            for(int i = 0; i < POP_SIZE; i++){
                kill(pStudentData->stdata[i].student_pid, SIGTERM);
            }
            semctl(semid, 2, IPC_RMID);
            shmdt(pStudentData);
            shmctl(memid, IPC_RMID, NULL);
            remove_queue(create_queue());
            //msq_rm(msqvote_id);
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

void ready(int semid) {
    ops.sem_num = 1;
    ops.sem_op = 0;
    if (semop(semid, &ops, 1) == -1) {
        TEST_ERROR
    }
}

void take_sem(int semid, int num)
{      
    ops.sem_num = num;
    ops.sem_op = -1;
    if ( (semop(semid, &ops, (size_t)1)) == -1) {
        TEST_ERROR
    }
}

void release_sem(int semid, int num)
{   
    ops.sem_num = num;
    ops.sem_op = 1;
    if (semop(semid, &ops, (size_t)1) == -1) {
        TEST_ERROR
    }
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
    if (( rm_queue = msgctl(id, IPC_RMID, NULL)) == -1) {
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

int receive_msg_nowait (int id) //receive a message in the queue, no wait
{
    return msgrcv(id, &invitation, (sizeof(invitation)-sizeof(long)), 0, IPC_NOWAIT);
}

int invite(int position, struct message msg, int mark)
{    
    msg.type = msg.sender_pid; 
    msg.invited = 1;
    msg.accept = 0;
    msg.sender_pid = getpid();
    msg.sender_index = position;
    msg.max_mark = mark;

    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), IPC_NOWAIT) == -1) {
        if (errno != EAGAIN) {
            TEST_ERROR
        }
        return 0;
    } else {
        pStudentData->stdata[position].nof_invites--;
        return 1;
    }
}

void accept(int position, struct message msg)
{
    msg.type = msg.sender_pid; 
    msg.invited = 0;
    msg.accept = 1;
    msg.sender_pid = getpid();
    msg.sender_index = position;
    
    if (pStudentData->stdata[position].mark_ca > msg.max_mark) {
        msg.max_mark = pStudentData->stdata[position].mark_ca;
    }    

    if ( msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        TEST_ERROR
    } else {
        pStudentData->stdata[position].team = 1;
    }
}

void decline(int position, struct message msg)
{
    msg.type = msg.sender_pid; 
    msg.invited = 0;
    msg.accept = 0;
    msg.sender_pid = getpid();
    msg.sender_index = position;

    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        TEST_ERROR
    }
}
	/* TEAM FUNCTIONS */
pid_t find_team_mate(int position)
{
    pid_t pid = -1;
    for(int i = 0; (i < POP_SIZE && pid == -1); i++){
        if(i != position){
            if(pStudentData->stdata[position].team == 0 &&
               pStudentData->stdata[position].registration_number == pStudentData->stdata[position].class &&
               pStudentData->stdata[position].nof_elems == pStudentData->stdata[i].nof_elems){
                pid = pStudentData->stdata[i].student_pid;
            }
        }
    }
    return pid;
}
//chiude il gruppo
void lock_group(int *group_members, int nelem_group, int max_mark ){
   for(int i = 0; i < nelem_group; i++){
     pStudentData->stdata[group_members[i]].closed = 1;
     pStudentData->stdata[group_members[i]].nelem_group = nelem_group;
     pStudentData->stdata[group_members[i]].max_mark_ca = max_mark;
   
   }
}
pid_t search_4_mate(int position){
  int i = 0;
  pid_t pid = -1;
  int mark1 = pStudentData->stdata[position].mark_ca;
  int group1 = pStudentData->stdata[position].team;
  int tlab1 = pStudentData->stdata[position].turn;	
  int nof_1 =pStudentData->stdata[position].nof_elems;
  int mark2 = pStudentData->stdata[i].mark_ca;
  int group2 = pStudentData->stdata[i].team;
  int tlab2 = pStudentData->stdata[i].turn;
  int nof2 = pStudentData->stdata[i].nof_elems;
  while(i < POP_SIZE && i != position){
    if(group2 == 0 && mark2 > mark1 && tlab1 == tlab2 && nof1 == nof2){
      mark1 = mark2;
      pid = pStudentData->stdata[i].matricule;
    }
    else if(group2 == 0 && mark2 > mark1 && mark1 > mark2 + 3 && )	
  
	
	
	
	
	
  
  }

}
