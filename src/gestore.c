#include "shared.h"

int sim_time, id_memory, sem_id;
student_data * localmemory;

void end_handler(int signal)
{
    int average = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        pid_t child = localmemory[i].student_pid;
        kill(child, SIGKILL);
        wait(&child);
    }
    puts("Exiting main");
    semctl(sem_id, SEM, IPC_RMID);
    free(localmemory);
    exit(0);
}

void spawn(int size)
{
    int * arguments = malloc(sizeof(int));
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        arguments[1] = i;

        if(process == -1) {
            strerror(errno);
        }else if(process==0) {
            execve("./student", (char *const *) arguments, NULL);
        }
    }
}

int main()
{
    #ifndef TEST
        puts("Insert the students' number.");
        scanf("%d", &POP_SIZE);
    #endif

    puts("Insert the simulation time (minutes).");
    scanf("%d", &sim_time);

    signal(SIGALRM, end_handler);

    if((id_memory = create_memory(POP_SIZE)) == -1) {
        strerror(errno);
    }

    localmemory = (student_data *)connect(id_memory);

    sem_id = create_sem();
    semctl(sem_id, 1, SETVAL, 1);

    spawn(POP_SIZE);
    alarm(sim_time);
    wait(0);
}
