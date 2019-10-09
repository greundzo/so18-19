#include "libreria.h"
#define TEST_ERROR    if (errno) {fprintf(stderr,                       \
                                          "%s:%d: PID=%5d: Error %d (%s)\n", \
                                          __FILE__,                     \
                                          __LINE__,                     \
                                          getpid(),                     \
                                          errno,                        \
                                          strerror(errno));}

static void end_time(int signum);
static void emergency(int signum);
struct data* st_log;

/**
 *Handler di fine simulazione
 */
static void end_time(int signum)
{
  //remove_mem();
  printf("%d, giorno dell'appello.\n", getpid());/*first_time_plz_be_gentle*/
  exit(0);
}

/**
 *Handler del segnale di emergenza
 */
static void emergency(int signum)
{
  //remove_mem();
  printf("Emergency exit %d\n", getpid());
  exit(0);
}

int main(int argc, char** argv) {
  pid_t pid, father;
  int status, matricola, voto_AdE, nof_elems, nof_invites, max_reject, elems, rejects, invites;

  struct sigaction end;		//handler del segnale di fine
	end.sa_handler = end_time;	//simulazione
	sigemptyset(&end.sa_mask);
	TEST_ERROR;
	end.sa_flags = 0;

  if(sigaction(SIGALRM, &end, NULL)==-1) {
		perror("Failed.");	//installo l'hanlder
	}

  struct sigaction quit;		//handler del segnale di emergenza
	quit.sa_handler = emergency;
	sigemptyset(&end.sa_mask);
	TEST_ERROR;
	end.sa_flags = 0;

  if(sigaction(SIGINT, &quit, NULL)==-1) {
		perror("Fallito");	//installo l'hanlder
	}
  /*
  st_log = shmat(argv[0],0,0);
  sem = (int)argv[1];*/
  pid = getpid();
  father = getppid();
  srand(pid);
  matricola = (rand()+100000)%900000;

  printf("Student %d, matricule %d\n", pid, matricola);
  exit(0);

}
