#include "shared.h"

int sim_time;
union semun uni;

/*
* Creates POP_SIZE child processes
* and make them specialize using the execve syscall
*/
void spawn(int size)
{
    char * args [] = {NULL, NULL};
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        //sprintf(args[0], "%d", i);
        TEST_ERROR;
        if(process == -1) {
            TEST_ERROR;
        } else if(process==0) {
            if (execve("./student", args, NULL) == -1) { //here we could pass argument to our child
                TEST_ERROR;                        //to set his own shm cell
            }
        }
    }
    puts("Students created. Simulation started.");
    kill(GROUP, SIGBUS);
}

int main(int argc, char ** argv)
{
    // Program termination handler
    handle.sa_handler = signalhandler;
    sigemptyset(&mask);
    handle.sa_mask = mask;
    handle.sa_flags = 0;
    
    if (sigaction(SIGALRM, &handle, NULL) == -1) {
        TEST_ERROR;
    }
    
    if (sigaction(SIGINT, &handle, NULL) == -1) {
        TEST_ERROR;
    }

    #ifdef TEST
        printf("%s", "Insert the students' number: ");
        scanf("%d", &nums);
    #endif    
    printf("%s", "Insert the simulation time (minutes): ");
    scanf("%d", &sim_time);
    puts("");

    //sim_time = sim_time * 60; //Conversion in minutes

    memid = create_memory();

    // Pointer to shm segment allocated at the beginning of the execution
    pStudentData = (shared *)connect(memid);
    pStudentData->pc = 0;

    semid = create_sem();
    sem_init_val(0, 1);

    puts("Creating students...");

    spawn(POP_SIZE);
    puts("**********");
    puts("");
    alarm(sim_time);
    pause();
}
