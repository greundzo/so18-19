#include "shared.h"

int sim_time, id_memory, sem_id;
struct student_data * p_memory;

void spawn(int size)
{
    int * arguments = malloc(2*sizeof(int));
    arguments[0] = id_memory;
    for(int i = 0; i < size; i++) {
        pid_t process = fork();
        arguments[1] = i;

        if(process == -1) {
            perror("Fork failed.");
        }else if(process==0) {
            execve("./student", (char *const *) arguments, NULL);
        }
    }
}

int main() {
    #ifndef test
        puts("Insert the students' number.");
        scanf("%d", &POP_SIZE);
    #endif

    puts("Insert the simulation time (minutes).");
    scanf("%d", &sim_time);

    id_memory = create_memory(POP_SIZE);
    p_memory = connect(id_memory);

    sem_id = create_sem();
    semctl(sem_id, 1, SETVAL, 1);

    spawn(POP_SIZE);

    wait(0);
}
