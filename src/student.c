//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;

void start_simulation()
{

}

void end_sim()
{
    puts("exit");
    free(pStudentData);
    exit(0);
}

int main(int argc, char ** argv)
{
    /*start_handler.sa_handler = start_simulation;
    st_end_handler.sa_handler = end_sim;
    sigaction(SIGBUS, &start_handler, NULL);
    sigaction(SIGKILL, &st_end_handler, NULL);*/
    TEST_ERROR;
    //pStudentData = (shared *)connect(memid);
    

    //int st_pos = atoi(argv[0]);
    voto_AdE = generate_random_integer(18, 30, getpid());

    // CRITICAL AREA
    if (take_sem(1) < 0) {
        TEST_ERROR;
    }    
    printf("%d", voto_AdE);
    //pStudentData->stdata->student_pid[&st_pos] = getpid();
    
    if(release_sem(1) < 0) {
        TEST_ERROR;
    }

    pause();
}
