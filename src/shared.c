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
            remove_queue(msgid);
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

int receive_msg_nowait (int id, struct message invitation) //receive a message in the queue, no wait
{
    int receivednw;
    if (( receivednw = msgrcv(id, &invitation, (sizeof(invitation)-sizeof(long)), 0, IPC_NOWAIT)) == -1) {
	    TEST_ERROR
    }
    return receivednw;
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