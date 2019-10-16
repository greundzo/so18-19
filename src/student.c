//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;

int main(int argc, char ** argv)
{
    handle.sa_handler = signalhandler;
    sigemptyset(&mask);
    handle.sa_mask = mask;
    handle.sa_flags = 0;

    if (sigaction(SIGUSR1, &handle, NULL) == -1) {
        TEST_ERROR;
    }
    
    if (sigaction(SIGKILL, &handle, NULL) == -1) {
        TEST_ERROR;
    }
    
    pStudentData = (shared *)connect(memid);
    voto_AdE = generate_random_integer(18, 30, getpid());

    // CRITICAL AREA
    ops.sem_num = 1;
    ops.sem_op = 0;
    if (semop(semid, &ops, (size_t)1) == -1) {
        TEST_ERROR
    }

    take_sem(0);    
    printf("%d", voto_AdE);
    int index = pStudentData->pc;
    pStudentData->stdata[index].student_pid = getpid();
    pStudentData->pc ++;    
    release_sem(0);

    pause();
}
