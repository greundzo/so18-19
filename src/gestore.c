#include "shared.h"

int POP_SIZE, sim_time;

void spawn(int size)
{
  for(int i = 0; i < size; i++) {
    pid_t process = fork();

    if(process == -1) {
      perror("Fork failed.");
    }else if(process==0) {
      execve("./student", NULL, NULL);
    }
  }
}

int main(int argc, char** argv) {
  puts("Insert the students' number.");
  scanf("%d", &POP_SIZE);
  puts("Insert the simulation time (minutes).");
  scanf("%d", &sim_time);

  spawn(POP_SIZE);

  wait(0);
}
