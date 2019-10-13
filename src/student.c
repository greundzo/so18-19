//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;
struct student_data * st_memory;

void end_handler(int signal)
{
    puts("exit");
    exit(0);
}

int main(int argc, char * argv[])
{
    st_memory = connect(atoi(argv[0]));
    int st_pos = atoi(argv[1]);
    int st_sem_id = create_sem();
    voto_AdE = generate_random_integer(18,30, getpid());

    signal(SIGTERM, end_handler);

    take_sem(st_sem_id, st_memory, st_pos);
    printf("%d", voto_AdE);
    release_sem(st_sem_id, st_memory, st_pos);

    take_sem(st_sem_id, st_memory, st_pos);
    st_memory[st_pos].student_pid = getpid();
    release_sem(st_sem_id, st_memory, st_pos);

    puts("");
    pause();
}
