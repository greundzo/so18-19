//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;
student_data * localmemory;

void end_handler(int signal)
{
    puts("exit");
    free(localmemory);
    exit(0);
}

int main(int argc, char * argv[])
{
    int memory_id = create_memory(POP_SIZE);
    localmemory = (student_data *)connect(memory_id);

    int st_pos = atoi(argv[1]);
    int st_sem_id = create_sem();
    voto_AdE = generate_random_integer(18,30, getpid());

    signal(SIGKILL, end_handler);

    printf("%d", voto_AdE);

    take_sem(st_sem_id);
    localmemory[st_pos].student_pid = getpid();
    release_sem(st_sem_id);

    puts("");
    pause();
}
