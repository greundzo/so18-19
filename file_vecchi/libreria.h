#define _GNU_SOURCE
#define __USE_POSIX
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

#define KEY_MSG 666
#define KEY_SEM 2810
#define KEY_MEM 777
#define MSG_MAX_SIZE 20
#define PEOPLE 100

//SHM Prototype Functions
int shmid;
int get_mem(int pop);
void write_data(void * to_write, int index, pid_t pid);
void remove_mem(int memid);

//Semaphores Prototype Functions
int semid;
int get_semid();
int init_sem_one(int semid, int semnum);
int init_sem_zero(int semid, int semnum);
int set_sem(int semid, int semnum, int value, short flag);
int take_sem(int semid, int semnum, short flag);
int release_sem(int semid, int semnum, short flag);
int wait_sem(int semid, int semnum, short flag);
void remove_sem(int semid);

//MsgQueue Prototype Functions
int get_queue();
int send_mex(int msqid, const void *msgp, size_t msgsz, int msgflg);
int receive_mex(int msqid, struct msgbuf *msgp, int msgsz, long mtype, int msgflag);
int remove_queue(int msqid);

typedef struct student_data {
  pid_t student;
  int matricule;
  int grade;
}student_data;

//Used with semctl() calls
union semun {
     int                 val;
     struct semid_ds *   buf;
     unsigned short *    array;
 #if defined(__linux__)
     struct seminfo *    __buf;
 #endif
 };

//SHM Functions
int get_mem(int pop) {
	if( (shmid = shmget(KEY_MEM, pop * sizeof(struct student_data), IPC_CREAT|0666) == -1 ))
		return -1;
	return shmid;
}

void remove_mem(int memid) {
	shmctl(memid, IPC_RMID, 0);
}

//Semaphores Functions
int get_semid() {
	if( (semid = semget(KEY_SEM, 4, IPC_CREAT|0666) == -1) )
		return -1;
	return semid;
}

void remove_sem(int semid) {
	semctl(semid, IPC_RMID, 0);
}

//Initialize semaphore to 1 (i.e., "available")
int init_sem_one(int semid, int semnum) {
  union semun arg;
  arg.val = 1;
  return semctl(semid, semnum, SETVAL, arg);
}
//Initialize semaphore to 0 (i.e., "in use")
int init_sem_zero(int semid, int semnum) {
  union semun arg;
  arg.val = 0;
  return semctl(semid, semnum, SETVAL, arg);
}
//Set a semaphore to a value (adds the value)
int set_sem(int semid, int semnum, int value, short flag) {
  struct sembuf sops;
  sops.sem_num = semnum;
  sops.sem_op = value;
  sops.sem_flg = flag;
  return semop(semid, &sops, 1);
}
// Reserve semaphore (decrement it by 1)
int take_sem(int semid, int semnum, short flag) {
  struct sembuf sops;
  sops.sem_num = semnum;
  sops.sem_op = -1;
  sops.sem_flg = flag;
  return semop(semid, &sops, 1);
}
// Release semaphore (increment it by 1)
int release_sem(int semid, int semnum, short flag) {
  struct sembuf sops;
  sops.sem_num = semnum;
  sops.sem_op = 1;
  sops.sem_flg = flag;
  return semop(semid, &sops, 1);
}
//Wait for semaphore to be 0
int wait_sem(int semid, int semnum, short flag) {
  struct sembuf sops;
  sops.sem_num = semnum;
  sops.sem_op = 0;
  sops.sem_flg = flag;
  return semop(semid, &sops, 1);
}

/*Message Queue Functions*/
struct lettera {
	long mtype;
	char mtext[MSG_MAX_SIZE];
};

int get_queue() {
	return msgget(KEY_MSG, IPC_CREAT);
}

int send_mex(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	return msgsnd(msqid, msgp, msgsz, msgflg);
}

int receive_mex(int msqid, struct msgbuf *msgp, int msgsz, long mtype, int msgflag) {
	return msgrcv(msqid, msgp, msgsz, mtype, msgflag);
}

int remove_queue(int msqid) {
	return msgctl(msqid, IPC_RMID, NULL);
}
