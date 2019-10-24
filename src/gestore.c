#include "shared.h"

int sim_time;

/*
* Creates POP_SIZE child processes
* and make them specialize using the execve syscall
*/
void spawn(int size)
{
    char * args [] = {"student", NULL};
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        TEST_ERROR
        if(process == -1) {
            TEST_ERROR
        } else if(process == 0) {
            if (execve("./student", args, NULL) == -1) { 
                TEST_ERROR                     
            }
        }
    }
    puts("Students created. Simulation started.");
}

int main(int argc, char ** argv)
{
    // Program termination handler
    handle.sa_handler = signalhandler;
    sigemptyset(&mask);
    handle.sa_mask = mask;
    handle.sa_flags = 0;
    
    if (sigaction(SIGALRM, &handle, NULL) == -1) {
        TEST_ERROR
    }
    
    if (sigaction(SIGINT, &handle, NULL) == -1) {
        TEST_ERROR
    }

    #ifdef TEST
    printf("%s", "Insert the students' number: ");
    scanf("%d", &POP_SIZE);
    #endif
    #ifdef TEST    
    printf("%s", "Insert the simulation time (minutes): ");
    scanf("%d", &SIM_TIME);
    #endif
    puts("");

    memid = create_memory();

    // Pointer to shm segment allocated at the beginning of the execution
    pst = (shared *)connect(memid);
    pst->pc = 0;

    // Semaphore creation and initialization
    semid = create_sem();
    sem_init_val(0, 1);
    sem_init_val(1, 1);

    msgid = create_queue();

    puts("Creating students...");

    spawn(POP_SIZE);
    puts("**********");
    puts("");
    
    ops.sem_num = 1;
    ops.sem_op = -1;
    semop(semid, &ops, 1);

    masksig();

    alarm(SIM_TIME); 
    
    // EINTR is the Interrupted Signal
    if (pause() == -1) {
        if(errno != EINTR)
            TEST_ERROR
    }

    printf("End simulation.\n");   
    
    for(int i = 0; i < POP_SIZE; i++) {

        lastmsg.mtype = pst->stdata[i].student_pid;

        if (pst->stdata[i].closed == 0) {
            lastmsg.mark = 0;
            if (msgsnd(msgmid, &lastmsg, sizeof(lastmsg) - sizeof(long), IPC_NOWAIT) == -1) {
                TEST_ERROR
            }
        } else {
            if (pst->stdata[i].nelem_team == pst->stdata[i].nof_elems) {
                lastmsg.mark = pst->stdata[i].max_mark_ca;
            }
            else{
                lastmsg.mark = (pst->stdata[i].max_mark_ca - 3);
            }

            if(msgsnd(msgmid, &lastmsg, sizeof(lastmsg) - sizeof(long), IPC_NOWAIT) == -1) {
                TEST_ERROR
            }
        }

        if(waitpid(pst->stdata[i].student_pid, &stat, 0) == -1) {
            TEST_ERROR
        }
    }
    
    ca_count = (int *)calloc(13, sizeof(int));
    os_count = (int *)calloc(16, sizeof(int));
    average_ca = 0;
    average_os = 0;
    for(int i = 0; i < POP_SIZE; i++) {
        printinfo(i);
        average_ca += pst->stdata[i].mark_ca;
        ca_count[pst->stdata[i].mark_ca - 18] += 1;

        average_os += pst->stdata[i].mark_os;
        os_count[pst->stdata[i].mark_os - 15] += 1;         
    }
    ca_count[1] -= POP_SIZE;
    average_ca /= POP_SIZE;
    average_os /= POP_SIZE;
    semctl(semid, 2, IPC_RMID);
    shmdt(pst);
    shmctl(memid, IPC_RMID, NULL);
    remove_queue(msgid);

    printf("\nComputer Architecture marks distribution:\n");
    printf("   18   19   20   21   22   23   24   25   26   27   28   29   30\n");
    for(int i = 0; i < 13; i++) {
        printf("  %3d", ca_count[i]);
    }
    printf("\n\n");

    printf("Operating Systems marks distribution:\n");
    printf("   15   16   17   18   19   20   21   22   23   24   25   26   27  "
           " 28   29   30    \n");
    for(int i = 0; i < 16; i++){
        printf("  %3d", os_count[i]);
    }
    printf("\n\n");
    printf("Average CA = %2.2f   Average OS = %2.2f   Difference = %2.2f\n\n",
            average_ca, average_os, average_os-average_ca);

    free(ca_count);
    free(os_count);

    exit(EXIT_SUCCESS);
}
