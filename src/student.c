//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;
shared * pStudentData;

struct sigaction start_handler; 
struct sigaction st_end_handler;

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
    start_handler.sa_handler = start_simulation;
    st_end_handler.sa_handler = end_sim;
    sigaction(SIGBUS, &start_handler, NULL);
    sigaction(SIGKILL, &st_end_handler, NULL);

    pStudentData = (shared *)connect(memid);
    

    //int st_pos = atoi(argv[0]);
    TEST_ERROR;
    voto_AdE = generate_random_integer(18,30, getpid());

    take_sem(1);

    printf("%d", voto_AdE);
    //pStudentData->stdata->student_pid[&st_pos] = getpid();
    TEST_ERROR;
    release_sem(1);

    pause();
}
