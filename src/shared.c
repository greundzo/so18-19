//
// Created by greundzo on 09/10/19.
//
#include "shared.h"

// Generates a casual number between minNum and maxNum 
int generate_random_integer(pid_t pid) 
{
    srand(pid);
    return rand()%13+18;
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
    srand(getpid());
    int num = rand()%100+1;
    
    if (num <= 35) {
        return 2;
    }

    if (num <= 75 && num > 35) {
        return 3;
    }

    if(num > 75) {
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

void printinfo(int index, int reg, int mark) 
{
    printf("%3i: Register Number = %5i   CA Mark = %2i   OS Mark = %2i\n",
            index, reg,
            mark, max_mark);
}

void masksig()
{
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void signalhandler(int signum)
{

    switch(signum){
        case SIGALRM:      
            for (int i = 0; i < POP_SIZE; i++) {
                kill(pst->stdata[i].student_pid, SIGUSR1);
            }
            break;

        case SIGINT:
            for(int i = 0; i < POP_SIZE; i++){
                kill(pst->stdata[i].student_pid, SIGTERM);
            }
            semctl(semid, 2, IPC_RMID);
            shmdt(pst);
            shmctl(memid, IPC_RMID, NULL);
            remove_queue(msgid);
            remove_queue(lastid);
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
	if (( sem = semget(SEM, 4, 0666|IPC_CREAT)) == -1) {
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

void ready(int semid, int num) {
    ops.sem_num = num;
    ops.sem_op = 0;
    if (semop(semid, &ops, 1) == -1) {
        TEST_ERROR
    }
}

void take_sem(int semid, int num)
{      
    ops.sem_num = num;
    ops.sem_op = -1;
    if ( (semop(semid, &ops, 1)) == -1) {
        TEST_ERROR
    }
}

void release_sem(int semid, int num)
{   
    ops.sem_num = num;
    ops.sem_op = 1;
    if (semop(semid, &ops, 1) == -1) {
        TEST_ERROR
    }
}

/* GESTIONE DEI MESSAGGI */
int create_queue() //creates queue and returns id
{
    int queue;
    if (( queue = msgget(MSG, 0666|IPC_CREAT)) == -1) {
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

int info_queue(int id) //get the status of the queue
{
    int infos;
    if (( infos = msgctl(id, IPC_STAT, &buffer)) == -1) {
	    TEST_ERROR
    }
    return infos;
}

int receive_msg_nowait(int id) //receive a message in the queue, no wait
{
    return msgrcv(id, &invitation, sizeof(invitation), getpid(), IPC_NOWAIT);
}

int invite(int ind, int pid, int mark)
{    
    invitation.mtype = pid; 
    invitation.invited = 1;
    invitation.accept = 0;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;
    invitation.max_mark = mark;

    if (msgsnd(msgid, &invitation, sizeof(invitation), IPC_NOWAIT) == -1) {
        if (errno != EAGAIN) {
            TEST_ERROR
        }
        return 0;
    } else {
        pst->stdata[ind].nof_invites--;
        return 1;
    }
}

void accept(int ind)
{
    invitation.mtype = invitation.sender_pid; 
    invitation.invited = 0;
    invitation.accept = 1;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;
    
    if (pst->stdata[ind].mark_ca > invitation.max_mark) {
        invitation.max_mark = pst->stdata[ind].mark_ca;
    }    

    if (msgsnd(msgid, &invitation, sizeof(invitation), 0) == -1) {
        TEST_ERROR
    } else {
        pst->stdata[ind].team = 1;
    }
}

void decline(int ind)
{
    invitation.mtype = invitation.sender_pid; 
    invitation.invited = 0;
    invitation.accept = 0;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;

    if (msgsnd(msgid, &invitation, sizeof(invitation), 0) == -1) {
        TEST_ERROR
    }
}
/* TEAM FUNCTIONS */
pid_t find_team_mate(int ind)
{
    pid_t pid = -1;
    for(int i = 0; (i < POP_SIZE && pid == -1); i++) {
        if(i != ind){
            if(pst->stdata[ind].team == 0 
                && pst->stdata[ind].class == pst->stdata[i].class 
                && pst->stdata[ind].nof_elems == pst->stdata[i].nof_elems) {
                pid = pst->stdata[i].student_pid;
            } else if (pst->stdata[ind].team == 0 
                && pst->stdata[ind].class == pst->stdata[i].class) {
                pid = pst->stdata[i].student_pid;
            }
        }
    }
    return pid;
}

pid_t find_inviting_mate(int ind) 
{
    for(int i = 0; i < POP_SIZE; i++){
        if(i != ind) {
            if(pst->stdata[i].team == 0 
                && pst->stdata[ind].class == pst->stdata[i].class 
                && pst->stdata[i].nof_invites != 0) {
                return pst->stdata[i].student_pid;
            }

            if(pst->stdata[i].team == 1 && pst->stdata[i].closed == 0 
                && pst->stdata[ind].class == pst->stdata[i].class 
                && pst->stdata[i].leader == 1 &&  pst->stdata[i].nof_invites != 0) {
                return pst->stdata[i].student_pid;
            }
        }
    }
    return -1;
}

//chiude il gruppo
void lock_group(int *team_members, int nelem_team, int max_mark ) {
    for(int i = 0; i < nelem_team; i++) {
        pst->stdata[team_members[i]].closed = 1;
        pst->stdata[team_members[i]].nelem_team = nelem_team;
        pst->stdata[team_members[i]].max_mark_ca = max_mark;   
    }
}

pid_t search_team_mate(int ind) {
	int i = 0; //per distinguere i casi
	pid_t pid = -1;

	int search_mark = pst->stdata[ind].mark_ca;
	int search_class = pst->stdata[ind].turn;	
	int search_elem = pst->stdata[ind].nof_elems;

	int other_mark = pst->stdata[i].mark_ca;
	int other_team = pst->stdata[i].team;
	int other_class = pst->stdata[i].turn;
	int other_elem = pst->stdata[i].nof_elems;
	
	while (i < POP_SIZE && i != ind) {	
		if (other_team == 0 && other_mark > search_mark 
            && search_class == other_class && search_elem == other_elem) { // best mate
			pid = pst->stdata[i].student_pid;	
		} else if (other_team == 0 && other_mark < search_mark 
            && search_class == other_class && search_elem == other_elem) { // worst mate 
			pid = pst->stdata[i].student_pid;
		} else if (other_team == 0 && search_class == other_class 
            && search_elem == other_elem) { // random classmate with same nof_elems
			pid = pst->stdata[i].student_pid;
		}else {	
			if (pid == -1) {
				if(other_team == 0 && other_mark > search_mark + 3 
                    && search_class == other_class && search_elem == other_elem) {
					pid = pst->stdata[i].student_pid;
				} else if (other_team == 0 && other_mark < search_mark 
                    && search_class == other_class && search_elem == other_elem) {
					pid = pst->stdata[i].student_pid;
				} else if(other_team == 0 && search_class == other_class) { // totally random classmate
					pid = pst->stdata[i].student_pid;
				}				
			}		
		}	
	}
	return pid;
}
