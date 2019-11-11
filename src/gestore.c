#include "shared.h"

int sim_time, status;

/*
* Create POP_SIZE child processes
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
    /* Program termination handler */
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

    /* Pointer to shm segment allocated at the beginning of the execution */
    pst = (shared *)connect(memid);
    pst->pc = 0;

    /* Semaphore creation and initialization */
    semid = create_sem();
    sem_init_val(0, 1);
    sem_init_val(1, 1);
    sem_init_val(2, POP_SIZE);
    sem_init_val(3, POP_SIZE);

    /* Msgqueue creation */
    msgid = create_queue();
    lastid = msgget(LMS, 0666|IPC_CREAT);

    puts("Creating students...");

    /* Spawn students */
    spawn(POP_SIZE);
    puts("**********");
    puts("");

    ready(semid, 2);
    
    take_sem(semid, 1);

    /* Set the alarm */
    alarm(SIM_TIME); 
    
    /* EINTR is the Interrupted Signal */
    if (pause() == -1) {
        if(errno != EINTR)
            TEST_ERROR
    }

    printf("End simulation.\n");   
    
    /* Send to every student his final mark */
    for(int i = 0; i < POP_SIZE; i++) {

        lastmsg.mtype = pst->stdata[i].student_pid;

        if (pst->stdata[i].closed == 0) {
            lastmsg.mark = 0;
            if (msgsnd(lastid, &lastmsg, sizeof(lastmsg), IPC_NOWAIT) == -1) {
                TEST_ERROR
            }
        } else {
            if (pst->stdata[i].nelem_team == pst->stdata[i].nof_elems) {
                lastmsg.mark = pst->stdata[i].max_mark_ca;
            }
            else{
                lastmsg.mark = (pst->stdata[i].max_mark_ca - 3);
            }

            if (msgsnd(lastid, &lastmsg, sizeof(lastmsg), IPC_NOWAIT) == -1) {
                TEST_ERROR
            }
        }

        if (waitpid(pst->stdata[i].student_pid, &status, 0) == -1) {
            TEST_ERROR
        }
    }
    
    /* Allocate memory to check execution statistics */
    ca_count = (int *)calloc(13, sizeof(int));
    os_count = (int *)calloc(16, sizeof(int));
    average_ca = 0;
    average_os = 0;
    
    for(int i = 0; i < POP_SIZE; i++) {
        average_ca += pst->stdata[i].mark_ca;
        ca_count[pst->stdata[i].mark_ca - 18] += 1;
    }

    printf("\nComputer Architecture marks distribution:\n");
    printf("   18   19   20   21   22   23   24   25   26   27   28   29   30\n");
    for(int i = 0; i < 13; i++) {
        printf("  %3d", ca_count[i]);
    }
    printf("\n\n");
    free(ca_count);    
    
    for(int i = 0; i < POP_SIZE; i++) {
        average_os += pst->stdata[i].mark_os;
        os_count[pst->stdata[i].mark_os - 15] += 1;         
    }

    printf("Operating Systems marks distribution:\n");
    printf("   15   16   17   18   19   20   21   22   23   24   25   26   27  "
           " 28   29   30    \n");
    for(int i = 0; i < 16; i++) {
        printf("  %3d", os_count[i]);
    }
    printf("\n\n");
    free(os_count);

    average_ca /= POP_SIZE;
    average_os /= POP_SIZE;
    
    /* Free all IPCs */
    semctl(semid, 2, IPC_RMID);
    shmdt(pst);
    shmctl(memid, IPC_RMID, NULL);
    remove_queue(msgid);
    remove_queue(lastid);

    printf("\n\n");
    double average = average_os-average_ca;
    if (average < 0) {
        average *= -1;
    }
    printf("Average CA = %2.2f   Average OS = %2.2f   Difference = %2.2f\n\n",
            average_ca, average_os, average);

    exit(EXIT_SUCCESS);
}
