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
            index, pst->stdata[index].registration_number,
            pst->stdata[index].mark_ca, pst->stdata[index].mark_os);
}

void signalhandler(int signum)
{

    switch(signum){
        case SIGALRM:
            take_sem(semid, 0);
            printf("End simulation.\n");            
            for (int i = 0; i < POP_SIZE; i++) {
                pids[i] = pst->stdata[i].student_pid;
                kill(pids[i], SIGUSR1);
            }
            release_sem(semid, 0);            
            break;

        case SIGUSR1:
            /* here child processes send data to parent*/
            exit(EXIT_SUCCESS);

        case SIGINT:
            for(int i = 0; i < POP_SIZE; i++){
                kill(pst->stdata[i].student_pid, SIGTERM);
            }
            semctl(semid, 2, IPC_RMID);
            shmdt(pst);
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

int invite(int ind, int pid, int mark)
{    
    invitation.type = pid; 
    invitation.invited = 1;
    invitation.accept = 0;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;
    invitation.max_mark = mark;

    if (msgsnd(msgid, &invitation, sizeof(invitation) - sizeof(long), IPC_NOWAIT) == -1) {
        if (errno != EAGAIN) {
            TEST_ERROR
        }
        return 0;
    } else {
        pst->stdata[ind].nof_invites--;
		pst->stdata[ind].leader = 1;
        return 1;
    }
}

void accept(int ind)
{
    invitation.type = invitation.sender_pid; 
    invitation.invited = 0;
    invitation.accept = 1;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;
    
    if (pst->stdata[ind].mark_ca > invitation.max_mark) {
        invitation.max_mark = pst->stdata[ind].mark_ca;
    }    

    if (msgsnd(msgid, &invitation, sizeof(invitation) - sizeof(long), 0) == -1) {
        TEST_ERROR
    } else {
        pst->stdata[ind].team = 1;
    }
}

void decline(int ind)
{
    invitation.type = invitation.sender_pid; 
    invitation.invited = 0;
    invitation.accept = 0;
    invitation.sender_pid = getpid();
    invitation.sender_index = ind;

    if (msgsnd(msgid, &invitation, sizeof(invitation) - sizeof(long), 0) == -1) {
        TEST_ERROR
    }
}
/* TEAM FUNCTIONS */
pid_t find_team_mate(int ind)
{
    pid_t pid = -1;
    for(int i = 0; (i < POP_SIZE && pid == -1); i++) {
        if(i != ind){
            if(pst->stdata[ind].team == 0 &&
               pst->stdata[ind].class == pst->stdata[i].class &&
               pst->stdata[ind].nof_elems == pst->stdata[i].nof_elems) {
                pid = pst->stdata[i].student_pid;
            }
        }
    }
    return pid;
}
//chiude il gruppo
void lock_group(int *team_members, int nelem_team, int max_mark ) {
    for(int i = 0; i < nelem_team; i++) {
        pst->stdata[team_members[i]].closed = 1;
        pst->stdata[team_members[i]].nelem_group = nelem_team;
        pst->stdata[team_members[i]].max_mark_ca = max_mark;   
    }
}

pid_t search_4_mate(int ind) {
	int check, i = 0; //per distinguere i casi
	pid_t pid = -1;

	int search_mark = pst->stdata[ind].mark_ca;
	//int search_team = pst->stdata[ind].team;
	int search_class = pst->stdata[ind].turn;	
	int search_elem = pst->stdata[ind].nof_elems;

	int other_mark = pst->stdata[i].mark_ca;
	int other_team = pst->stdata[i].team;
	int other_class = pst->stdata[i].turn;
	int other_elem = pst->stdata[i].nof_elems;
	
	while (i < POP_SIZE && i != ind) {
	
		if (other_team == 0 && other_mark > search_mark && search_class == other_class && search_elem == other_elem) { //compagno migliore
			pid = pst->stdata[i].student_pid;
			check = 1;	
		} else if (other_team == 0 && other_mark < search_mark && search_class == other_class && search_elem == other_elem) { //compagno peggiore 
			pid = pst->stdata[i].student_pid;
			check = 2;   
		} else if (other_team == 0 && search_class == other_class && search_elem == other_elem) { //compgano qualsiasi(non si tiene conto del voto)
			pid = pst->stdata[i].student_pid;
			check = 3;
		}else {	
			if (pid == -1 && check != 2 && check != 3) {
				if(other_team == 0 && other_mark > search_mark + 3 && search_class == other_class && search_elem == other_elem){
					pid = pst->stdata[i].student_pid;
				}
			} else if (pid == -1 && check != 1 && check != 3) {
				if (other_team == 0 && other_mark < search_mark && search_class == other_class && search_elem == other_elem) {
					pid = pst->stdata[i].student_pid;
				} 		
			} else if (pid == -1 && check != 1 && check != 2) {
				if(other_team == 0 && search_class == other_class) {
					pid = pst->stdata[i].student_pid;
				}				
			}		
		}	
	}
	return pid;
}
