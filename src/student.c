//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int reg_num, class, mark_AdE, mark_So, nof_elements, nof_invites, max_reject;
int mem_id, sem_id, position;
struct sigaction killed;

void killhandler (int signal) {
    //printinfo(position);
    shmdt(pStudentData);
    exit(EXIT_SUCCESS);
}            

int main(int argc, char ** argv)
{
    pid_t pid = getpid();

    killed.sa_handler = killhandler;
    sigemptyset(&mask);
    killed.sa_mask = mask;
    killed.sa_flags = 0;

    if (sigaction(SIGUSR1, &killed, NULL) == -1) {
        TEST_ERROR
    }


    mem_id = create_memory();
    pStudentData = (shared *)connect(mem_id);
    mark_AdE = generate_random_integer(18, 30, pid);
    reg_num = generate_matr(pid);
    class = getturn(reg_num);
    sem_id = create_sem();
    // CRITICAL AREA
    ops.sem_num = 1;
    ops.sem_op = 0;
    if (semop(sem_id, &ops, 1) == -1) {
        TEST_ERROR
    }
    
    take_sem(sem_id, 0);    
    position = pStudentData->pc;
    pStudentData->stdata[position].student_pid = pid;
    pStudentData->stdata[position].registration_number = reg_num;
    pStudentData->stdata[position].class = class;
    pStudentData->stdata[position].mark_os = 0;
    pStudentData->stdata[position].mark_ca = mark_AdE;
    pStudentData->stdata[position].group = 0;
    pStudentData->stdata[position].leader = 0;
    pStudentData->stdata[position].closed = 0;
    pStudentData->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    pause();
}
