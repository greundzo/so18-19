#include "shared.h"

int sim_time;
shared * pStudentData;
struct sigaction end_handler;
struct sigaction compulsory;
union semun uni;

/*
* Emergency handler to remove IPCs and terminate execution
* in case of trouble. Set to handle SIGINT caused by user
* pressing Ctrl + C
*/
void emergency(int signal) 
{
    for (int i = 0; i < POP_SIZE; i++) {
        pid_t child = pStudentData->stdata->student_pid[&i];
        kill(child, SIGKILL);
        wait(&child);
    }
    semctl(semid, SEM, IPC_RMID);
    free(pStudentData);
    shmctl(memid, IPC_RMID, NULL);
}

/*
* End simulation signal handler. Kills and waits for
* all children processes, frees the IPCs and then
* terminates the execution.
*/
void end_simulation(int signal)
{
    //int average = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        pid_t child = pStudentData->stdata->student_pid[&i];
        kill(child, SIGKILL);
        wait(&child);
    }
    puts("Exiting main");
    semctl(semid, SEM, IPC_RMID);
    free(pStudentData);
    exit(0);
}
/*
* Creates POP_SIZE child processes
* and make them specialize using the execve syscall
*/
void spawn(int size)
{
    char * argv = malloc(sizeof(char));
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        sprintf(argv, "%d", i);
        if(process == -1) {
            strerror(errno);
        } else if(process==0) {
            if (execve("./student", NULL, NULL) == -1) { //here we could pass argument to our child
                strerror(errno);                        //to set his own shm cell
            }
        }
    }
    puts("Students created. Simulation started.");
    kill(GROUP, SIGBUS);
}

int main(int argc, char ** argv)
{
    // Program termination handler
    end_handler.sa_handler = end_simulation;
    sigaction(SIGALRM, &end_handler, NULL);

    // Program troubles handler
    compulsory.sa_handler = emergency;
    sigaction(SIGINT, &compulsory, NULL);

    #ifdef TEST
        printf("%s", "Insert the students' number: ");
        scanf("%d", &nums);
    #endif    
    printf("%s", "Insert the simulation time (minutes): ");
    scanf("%d", &sim_time);
    puts("");

    //sim_time = sim_time * 60; //Conversion in minutes

    if((memid = create_memory(POP_SIZE)) == -1) {
        strerror(errno);
    }

    // Pointer to shm segment allocated at the beginning of the execution
    pStudentData = (shared *)connect(memid);

    if ((semid = create_sem()) == -1) {
        strerror(errno);
    }

    uni.val = 1;
    sem_init_val(0, uni);

    puts("Creating students...");

    spawn(POP_SIZE);
    puts("**********");
    puts("");
    alarm(sim_time);
    pause();
}
