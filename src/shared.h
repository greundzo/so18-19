//
// Created by greundzo on 08/10/19.
//
#define TEST_ERROR		if (errno) {fprintf(stderr, \
						"%s:%d: PID=%5d: Error %d (%s)\n",\
					   	__FILE__,\
					   	__LINE__,\
					   	getpid(),\
					   	errno,\
					   	strerror(errno));}

#define _XOPEN_SOURCE
#ifndef TEST
#define POP_SIZE 100
#define SIM_TIME 1
#endif //test

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#define GROUP 3
#define MEM 30
#define SEM 5
#define MSG 7

int * group_members;


typedef struct student_data {
    pid_t student_pid;
    int class;
    int registration_number;
    int nof_elems;
    int nof_invites;
    int nof_members;
    int team;
    int leader;
    int closed;
    int mark_os;
    int mark_ca;
    int max_mark_ca;
    int nelem_group;	
}student_data;

typedef struct shared {
    struct student_data stdata[POP_SIZE];
    int pc;
}shared;

struct sigaction handle;
sigset_t mask;
void signalhandler(int signal);

// Shared global variables and functions
struct shared *pStudentData;
int *pids, *ca_count, *os_count;
int memid, semid;
float average_ca, average_os;
int generate_random_integer(int minNum, int maxNum, pid_t pid);
int generate_regnum(pid_t pid);
int read_conf(char *str);
int get_pref();
int get_turn(int matricule);
void printinfo(int index);

// Shared Memory functions declaration
int create_memory();
void *connect(int id);

// Semaphore functions declaration 
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};
struct sembuf ops;
int create_sem();
void sem_init_val(int index, int value);
void ready(int semid);
void take_sem(int semid, int num);
void release_sem(int semid, int num);

// Message Queue functions declaration 
struct msqid_ds buffer;

struct message {
    long type;
    pid_t sender_pid;
    int sender_index;
    int invited;
    int accept;
    int max_mark;    
};

struct message invitation;
int msgid;
int create_queue();
int remove_queue(int id);
int modify_queue(int id);
int receive_msg_nowait (int id);
int invite (int id, struct message msg, int mark);
void accept (int id, struct message msg);
void decline(int id, struct message msg);
pid_t find_team_mate(int position);
void lock_group(int *group_members, int nelem_group, int max_mark);
