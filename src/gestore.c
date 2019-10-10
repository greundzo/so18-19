#include "shared.h"

int POP_SIZE, sim_time, id_memory;
void * p_memory;

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

int main(int argc, char ** argv) {
  puts("Insert the students' number.");
  scanf("%d", &POP_SIZE);
  puts("Insert the simulation time (minutes).");
  scanf("%d", &sim_time);

  id_memory = create_memory(POP_SIZE);
  p_memory = connect(id_memory);

  spawn(POP_SIZE);

  wait(0);
}
