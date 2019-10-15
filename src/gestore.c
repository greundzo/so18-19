#include "shared.h"

int sim_time, id_memory, sem_id;
student_data * pStudentData;
struct sigaction end_handler;

void end_simulation()
{
    //int average = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        pid_t child = pStudentData[i].student_pid;
        kill(child, SIGKILL);
        wait(&child);
    }
    puts("Exiting main");
    semctl(sem_id, SEM, IPC_RMID);
    free(pStudentData);
    exit(0);
}

void spawn(int size)
{
    char * argv = malloc(sizeof(arguments));
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        argv[1] = i;
        if(process == -1) {
            strerror(errno);
        }else if(process==0) {
            if (execve("./student", NULL, NULL) == -1) {
                strerror(errno);
            }
        }
    }
    puts("Students created. Simulation started.");
    kill(GROUP, SIGBUS);
}

int main(int argc, char ** argv)
{
    end_handler.sa_handler = end_simulation;
    sigaction(SIGALRM, &end_handler, NULL);


    puts("Insert the students' number.");
    scanf("%d", &POP_SIZE);
    puts("Insert the simulation time (minutes).");
    scanf("%d", &sim_time);

    sim_time = sim_time * 60;

    if((id_memory = create_memory(POP_SIZE)) == -1) {
        strerror(errno);
    }

    pStudentData = (student_data *)connect(id_memory);

    if ((sem_id = create_sem()) == -1) {
        strerror(errno);
    }

    semctl(sem_id, 1, SETVAL, 1);

    puts("Creating students...");

    spawn(POP_SIZE);
    puts("**********");
    puts("");
    alarm(sim_time);
    wait(0);
}
