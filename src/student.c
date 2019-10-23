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
    pst = (shared *)connect(mem_id);
    max_reject = read_conf("max_reject");
    sem_id = create_sem();
    // CRITICAL AREA
        
    take_sem(sem_id, 0);    
    position = pst->pc;
    pst->stdata[position].student_pid = pid;
    pst->stdata[position].registration_number = generate_regnum(pid);
    pst->stdata[position].class = get_turn(pst->stdata[position].registration_number);
    pst->stdata[position].mark_os = 0;
    pst->stdata[position].mark_ca = generate_random_integer(18, 30, pid);
    pst->stdata[position].max_mark_ca = 0;
    pst->stdata[position].team = 0;
    pst->stdata[position].leader = 0;
    pst->stdata[position].closed = 0;
    pst->stdata[position].nof_elems = get_pref();
    pst->stdata[position].nof_invites = read_conf("nof_invites");
    pst->stdata[position].nof_members = 0;
    pst->pc ++;    
    release_sem(sem_id, 0);

    //exit(0);
    //pause();
    msgid = create_queue();

    ready(sem_id);
    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pst->stdata[position].team == 0 
        || (pst->stdata[position].leader == 1 
        && pst->stdata[position].closed == 0)) 
    {
        take_sem(sem_id, 0);
        while (receive_msg_nowait(msgid) != -1)
        {
            if (pst->stdata[position].team == 0) {
                if (max_reject > 0) {
                    if (pst->stdata[position].mark_ca > 26) {
                        if (pst->stdata[position].nof_elems == pst->stdata[invitation.sender_index].nof_elems) {
                            accept(position, invitation);

                        } else if (find_team_mate(position) == -1) {
                            accept(position, invitation);
                        } else {
                            decline(position, invitation);
                            max_reject--;
                        }
                    } else {
		          		if (pst->stdata[position].nof_elems == pst->stdata[invitation.sender_index].nof_elems) {
							if (invitation.max_mark > pst->stdata[position].mark_ca || pst->stdata[position].nof_invites == 0) {
								accept(position, invitation);
							} else {
								decline(position, invitation);
								max_reject --;
							}
						} else {
							if ((invitation.max_mark - 3) > pst->stdata[position].mark_ca || pst->stdata[position].nof_invites == 0) {
                                accept(position, invitation);
                	        } else {
								decline(position, invitation);
				      			max_reject--;
           		        	}
			  			}	    
		     		}
				} else {
		    		accept(position, invitation);
                }
            } else {
				decline(position, invitation);
	    	}
		}//while
        release_sem(sem_id, 0);
	
      /*  if(errno){//il processo potrebbe non trovare messaggi nella coda
            if(errno != ENOMSG){
                printf("Errore anomalo nella lettura della coda di messaggi:\n");
                TEST_ERROR;
            }
          }*/
	
    }//while
    pause();
}