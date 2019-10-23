//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int mark_os, max_reject, max_mark;
int mem_id, sem_id, ind, nelem_team;
pid_t pod;

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
    pst = (shared *)connect(mem_id);
    max_reject = read_conf("max_reject");
    sem_id = create_sem();
    // CRITICAL AREA
        
    take_sem(sem_id, 0);    
    ind = pst->pc;
    pst->stdata[ind].student_pid = pid;
    pst->stdata[ind].registration_number = generate_regnum(pid);
    pst->stdata[ind].class = get_turn(pst->stdata[ind].registration_number);
    pst->stdata[ind].mark_os = 0;
    pst->stdata[ind].mark_ca = generate_random_integer(18, 30, pid);
    pst->stdata[ind].max_mark_ca = pst->stdata[ind].mark_ca;
    pst->stdata[ind].team = 0;
    pst->stdata[ind].leader = 0;
    pst->stdata[ind].closed = 0;
    pst->stdata[ind].nof_elems = get_pref();
    pst->stdata[ind].nof_invites = read_conf("nof_invites");
    pst->stdata[ind].nof_members = 0;
    pst->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    //pause();
    msgid = create_queue();

    ready(sem_id);
    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pst->stdata[ind].team == 0 
        || (pst->stdata[ind].leader == 1 
        && pst->stdata[ind].closed == 0)) 
    {
        take_sem(sem_id, 0);
        while (receive_msg_nowait(msgid) != -1)
        {
            if (pst->stdata[ind].team == 0) {
                if (max_reject > 0) {
                    if (pst->stdata[ind].mark_ca > 26) {
                        if (pst->stdata[ind].nof_elems == pst->stdata[invitation.sender_index].nof_elems) {
                            accept(ind);
                        } else if (find_team_mate(ind) == -1) {
                            accept(ind);
                        } else {
                            decline(ind);
                            max_reject--;
                        }
                    } else {
		          		if (pst->stdata[ind].nof_elems == pst->stdata[invitation.sender_index].nof_elems) {
							if (invitation.max_mark > pst->stdata[ind].mark_ca || pst->stdata[ind].nof_invites == 0) {
								accept(ind);
							} else {
								decline(ind);
								max_reject --;
							}
						} else {
							if ((invitation.max_mark - 3) > pst->stdata[ind].mark_ca || pst->stdata[ind].nof_invites == 0) {
                                accept(ind);
                	        } else {
								decline(ind);
				      			max_reject--;
           		        	}
			  			}	    
		     		}
				} else {
		    		accept(ind);
                }
            } else {
				decline(ind);
	    	}
		}//while
        
        // Student may not find messages in the queue so we check if a particular error occurred
        if (errno) {
            if (errno != ENOMSG) {
                TEST_ERROR
            }
        }

        if (((pst->stdata[ind].leader == 1 && pst->stdata[ind].nof_elems != nelem_team) || 
            pst->stdata[ind].team == 0) && pst->stdata[ind].nof_invites > 0) 
        {
            puts("I'm in");
            pod = find_team_mate(ind);
            invite(ind, pod, pst->stdata[ind].max_mark_ca);
        }

	    release_sem(sem_id, 0);
    }//while


    pause();
}