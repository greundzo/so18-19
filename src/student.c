//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;

int main(int argc, char ** argv)
{
    if (sigaction(SIGUSR1, &handle, NULL) == -1) {
        TEST_ERROR;
    }
    
    if (sigaction(SIGKILL, &handle, NULL) == -1) {
        TEST_ERROR;
    }
    
    pStudentData = (shared *)connect(memid);
    voto_AdE = generate_random_integer(18, 30, getpid());

    // CRITICAL AREA
    if (take_sem(0) < 0) {
        TEST_ERROR;
    }    
    printf("%d", voto_AdE);
    int index = pStudentData->pc;
    pStudentData->stdata[index].student_pid = getpid();
    pStudentData->pc ++;    
    if(release_sem(0) < 0) {
        TEST_ERROR;
    }

    pause();
}
