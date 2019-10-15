//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;
student_data * pStudentData;

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

    pStudentData = (student_data *)connect(memid);
    if (setpgid(getpid(), GROUP) == -1) {
        strerror(errno);
    }

    int st_pos = atoi(argv[1]);
    voto_AdE = generate_random_integer(18,30, getpid());

    take_sem(semid, 0);

    printf("%d", voto_AdE);

    take_sem(semid, 0);
    pStudentData[st_pos].student_pid = getpid();
    release_sem(semid);

    puts("Wrote");
    pause();
}
