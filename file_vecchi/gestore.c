#include"libreria.h"

int POP_SIZE, sim_time;
struct student_data * pmem;

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

  if (get_mem(POP_SIZE) == -1) {
      strerror(errno);
  }

  pmem = (struct student_data *)shmat(shmid,NULL,0);
  //pmem[0].student = getpid();

  spawn(POP_SIZE);
  remove_mem(shmid);

  wait(0);
}
