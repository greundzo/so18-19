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

    ready(sem_id);
    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pStudentData->stdata[position].team == 0 
        || (pStudentData->stdata[position].leader == 1 
        && pStudentData->stdata[position].closed == 0)) 
    {
        take_sem(sem_id, 0);
        while (receive_msg_nowait(msgid) != -1)
        {
            if (pStudentData->stdata[position].team == 0) {
                if (max_reject > 0) {
                    if (pStudentData->stdata[position].mark_ca > 26) {
                        if (pStudentData->stdata[position].nof_elems == pStudentData->stdata[invitation.sender_pid].nof_elems) {
                            accept(position, invitation);
                        /*} else if (find_team_mate() != -1) {
                            accept(position, my_msg);*/
                        } else {
                            decline(position, invitation);
                            max_reject--;
                        }
                    } else {

                    }
                } else {
                    accept(position, invitation); // max_reject = 0
                }
            } else {
                decline(position, invitation);
            }
        }
        release_sem(sem_id, 0);
    }
    pause();
}
