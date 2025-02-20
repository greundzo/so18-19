/*
** Created by greundzo on 08/10/19.
*/
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
#endif 

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

#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#define MEM 300
#define SEM 555
#define MSG 777
#define LMS 888

typedef struct student_data {
    pid_t student_pid;
    int fclass;
    int nof_elems;
    int nof_invites;
    int nelem_team;
    int team;
    int leader;
    int closed;
    int mark_os;
    int mark_ca;
    int max_mark_ca;
}student_data;

typedef struct shared {
    struct student_data stdata[POP_SIZE];
    int pc;
}shared;

struct sigaction handle, sthandle;
sigset_t mask;
void signalhandler(int signal);
void sthandler(int signal);

/* Shared global variables and functions */

struct shared *pst;
int *ca_count, *os_count, *member_indexes;
int memid, semid, max_mark;
float average_ca, average_os;
int generate_random_integer(pid_t pid);
int generate_regnum(pid_t pid);
int read_conf(char *str);
int get_pref();
int get_turn(int matricule);
void printinfo(int index, int reg, int mark, int nof);
void masksig();

/* Shared Memory functions declaration */

int create_memory();
void *connect(int id);

/* Semaphore functions declaration */

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

struct sembuf ops;
int create_sem();
void sem_init_val(int index, int value);
void ready(int semid, int num);
void take_sem(int semid, int num);
void release_sem(int semid, int num);

/* Message Queue functions declaration */ 

struct msgbuf {
    long mtype;
    pid_t sender_pid;
    int sender_index;
    int invited;
    int accept;
    int max_mark;  
};

struct markmsg {
    long mtype;
    int mark;
};

struct msqid_ds buffer;
struct msgbuf invitation;
struct markmsg lastmsg;
int msgid, lastid;
int create_queue();
int remove_queue(int id);
int modify_queue(int id);
int receive_msg_nowait (int id);
int invite (int id, int pid, int mark);
pid_t search_team_mate(int ind);
void accept (int id);
void decline(int id);
pid_t find_team_mate(int ind);
pid_t find_inviting_mate(int ind);
pid_t find_random_mate(int ind);
void lock_group(int *team_members, int nelem_team, int max_mark);
