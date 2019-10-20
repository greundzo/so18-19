//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int reg_num, class, mark_AdE, mark_So, nof_elements, nof_invites, max_reject;
int mem_id, sem_id, position;

int main(int argc, char ** argv)
{
    pid_t pid = getpid();

    handle.sa_handler = signalhandler;
    sigemptyset(&mask);
    handle.sa_mask = mask;
    handle.sa_flags = 0;

    if (sigaction(SIGUSR1, &handle, NULL) == -1) {
        TEST_ERROR
    }


    mem_id = create_memory();
    pStudentData = (shared *)connect(mem_id);
    mark_AdE = generate_random_integer(18, 30, pid);
    reg_num = generate_regnum(pid);
    class = get_turn(reg_num);
    sem_id = create_sem();
    // CRITICAL AREA
    ops.sem_num = 1;
    ops.sem_op = 0;
    if (semop(sem_id, &ops, 1) == -1) {
        TEST_ERROR
    }
    
    take_sem(sem_id, 0);    
    position = pStudentData->pc;
    pStudentData->stdata[position].student_pid = pid;
    pStudentData->stdata[position].registration_number = reg_num;
    pStudentData->stdata[position].class = class;
    pStudentData->stdata[position].mark_os = 0;
    pStudentData->stdata[position].mark_ca = mark_AdE;
    pStudentData->stdata[position].group = 0;
    pStudentData->stdata[position].leader = 0;
    pStudentData->stdata[position].closed = 0;
    pStudentData->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    pause();
    int msgid = create_queue();
    struct message mymsg;
    
    while (receive_msg_nowait (msgid, mymsg) != - 1) {
        if (pStudentData->stdata[position].group == 0){ //il gruppo è vuoto: divento il leader
            take_sem(sem_id, 0);
	    pStudentData->stdata[position].leader = 1;
	    pStudentData->stdata[position].group = 1;
	    release_sem(sem_id, 0);
	}
	if (class == mymsg.class) { //stesso turno
            int posl = mymsg.posleader;
            take_sem(sem_id, 0);
            pStudentData->stdata[posl].group++;
            /*if (studentData->stdata[position].group == nof_elements){ //chiudo il gruppo 
                pStudentData->stdata[posl].closed = 1;
                release_sem(sem_id, 0);
            }
	    */
            if (pStudentData->stdata[position].leader == 0){//se non sono il leader aspetto
		pause();
	    }
        }
	    if (pStudentData->stdata[position].leader == 1) { //il leader cerca nuovi partecipanti al gruppo
		mymsg.posleader = position;
                mymsg.class = class;
		send_msg (msgid, mymsg);
	    }
        }  
    } 
}
