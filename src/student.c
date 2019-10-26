//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int mark_os, max_reject, nelem_team, wait_answer;
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

    memid = create_memory();
    pst = (shared *)connect(memid);
    max_reject = read_conf("max_reject");
    semid = create_sem();
    // CRITICAL AREA
        
    take_sem(semid, 0);    
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
    release_sem(semid, 0);

    //msgid = create_queue();

    ready(semid);
    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pst->stdata[ind].team == 0 
        || (pst->stdata[ind].leader == 1 
        && pst->stdata[ind].closed == 0)) 
    {
        take_sem(semid, 0);
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
            pod = find_team_mate(ind);
            wait_answer = invite(ind, pod, pst->stdata[ind].max_mark_ca);
        }

        if (pst->stdata[ind].leader == 1 && pst->stdata[ind].nof_elems == nelem_team) {
            lock_group(member_indexes, nelem_team, max_mark);
        }

        // No more to invite, I'm leader, I close the team
        if (pst->stdata[ind].leader == 1 && pst->stdata[ind].nof_invites == 0 && wait_answer == 0) {
            lock_group(member_indexes, nelem_team, pst->stdata[ind].max_mark_ca);
        }

        release_sem(semid, 0);

        // If the process invited someone it executes this part of code
        while(wait_answer) {
            if (msgrcv(msgid, &invitation, sizeof(invitation)-sizeof(long), getpid(), 0) != -1) {
                if (invitation.invited) {
                    decline(ind);
                } else {
                    if (invitation.accept) {
                        if (pst->stdata[ind].leader == 0) {
                            pst->stdata[ind].leader = 1;
                            pst->stdata[ind].team = 1;
                            nelem_team = 2;
                            member_indexes = (int *)calloc(nelem_team, sizeof(int));
                            member_indexes[0] = ind;
                            member_indexes[nelem_team - 1] = invitation.sender_index;
                            max_mark = invitation.max_mark;
                        }
                    } else {
                        nelem_team++;
                        member_indexes = (int *)realloc(member_indexes, nelem_team * sizeof(int));
                        member_indexes[nelem_team - 1] = invitation.sender_index;
                        max_mark = invitation.max_mark;
                    }
                    pst->stdata[ind].max_mark_ca = max_mark;
                    wait_answer = 0;
                }                
            }
        }	    
    }//while
    
    masksig();

    // Waiting for os mark..
    if (msgrcv(msgid, &lastmsg, sizeof(lastmsg)-sizeof(long), getpid(), 0) == -1) {
        TEST_ERROR
    } else {
        max_mark = lastmsg.mark;
        printinfo(ind);
    }

    if (pst->stdata[ind].leader == 1) {
        free(member_indexes);
    }

    //shmdt(pst);
    exit(EXIT_SUCCESS);
}