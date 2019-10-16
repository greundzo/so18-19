//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, class, voto_AdE, nof_elements, nof_invites, max_reject;
int mem_id, sem_id;
struct sigaction killed;

void killhandler (int signal) {
    shmdt(pStudentData);
    exit(EXIT_FAILURE);
}            

int main(int argc, char ** argv)
{
    pid_t pid = getpid();

    handle.sa_handler = signalhandler;
    sigemptyset(&mask);
    handle.sa_mask = mask;
    handle.sa_flags = 0;

    killed.sa_handler = signalhandler;
    sigemptyset(&mask);
    killed.sa_mask = mask;
    killed.sa_flags = 0;

    if (sigaction(SIGUSR1, &handle, NULL) == -1) {
        TEST_ERROR
    }

    mem_id = create_memory();
    pStudentData = (shared *)connect(mem_id);
    voto_AdE = generate_random_integer(18, 30, pid);
    matricola = generate_matr(pid)
;    sem_id = create_sem();
    // CRITICAL AREA
    ops.sem_num = 1;
    ops.sem_op = 0;
    if (semop(sem_id, &ops, 1) == -1) {
        TEST_ERROR
    }

    
    take_sem(sem_id, 0);    
    printf("%d\n", voto_AdE);
    int index = pStudentData->pc;
    pStudentData->stdata[index].student_pid = pid;
    pStudentData->stdata[index].matricule = matricola;
    pStudentData->stdata[index].vote_So = 0;
    pStudentData->stdata[index].vote_AdE = voto_AdE;
    pStudentData->stdata[index].group = 0;
    pStudentData->stdata[index].leader = 0;
    pStudentData->stdata[index].closed = 0;
    pStudentData->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    pause();
}
