//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int mark_os, max_reject;
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
    max_reject = read_conf("max_reject");
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
    pStudentData->stdata[position].registration_number = generate_regnum(pid);
    pStudentData->stdata[position].class = get_turn(pStudentData->stdata[position].registration_number);
    pStudentData->stdata[position].mark_os = 0;
    pStudentData->stdata[position].mark_ca = generate_random_integer(18, 30, pid);
    pStudentData->stdata[position].max_mark_ca = 0;
    pStudentData->stdata[position].team = 0;
    pStudentData->stdata[position].leader = 0;
    pStudentData->stdata[position].closed = 0;
    pStudentData->stdata[position].nof_elems = get_pref();
    pStudentData->stdata[position].nof_invites = read_conf("nof_invites");
    pStudentData->stdata[position].nof_members = 0;
    pStudentData->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    //pause();
    msgid = create_queue();

    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pStudentData->stdata[position].team == 0 
        || (pStudentData->stdata[position].leader == 1 
        && pStudentData->stdata[position].closed == 0)) 
    {
        take_sem(sem_id, 0);
        while (receive_msg_nowait(msgid, my_msg) != -1)
        {
            if (pStudentData->stdata[position].team == 0) {
                if (max_reject > 0) {
                    if (pStudentData->stdata[position].mark_ca > 26) {
                        if (pStudentData->stdata[position].nof_elems == pStudentData->stdata[my_msg.sender_pid].nof_elems) {
                            accept(position, my_msg);
                        }
                    }
                }
            }
        }
        release_sem(sem_id, 0);
    }
    
    /* PEZZO DA SISTEMARE
    
	if (class == my_msg.class) { //stesso turno
            int posl = my_msg.posleader;
            take_sem(sem_id, 0);
            pStudentData->stdata[posl].team++;
            /*if (studentData->stdata[position].group == nof_elements){ //chiudo il gruppo 
                pStudentData->stdata[posl].closed = 1;
                release_sem(sem_id, 0);
            }
	    
            if (pStudentData->stdata[position].leader == 0){//se non sono il leader aspetto
		pause();
	    }
        }
	    if (pStudentData->stdata[position].leader == 1 && pStudentData->stdata[position].closed == 0) { //il leader cerca nuovi partecipanti al gruppo
		my_msg.posleader = position;
                my_msg.class = class;
		send_msg (msgid, my_msg);
	    }
          
    } */
}
