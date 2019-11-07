//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int mark_os, max_reject, nelem_team, wait_answer, st_ind, reg_num, 
    st_class, st_mark_ca, st_nof_el, st_inv, last_minute;
pid_t pod;

void sthandler(int signal)
{
    /* here child processes send data to parent*/
    if (msgrcv(lastid, &lastmsg, sizeof(lastmsg), getpid(), 0) == -1) {
        TEST_ERROR
    } else {
        max_mark = lastmsg.mark;
        take_sem(semid, 3);
        pst->stdata[st_ind].mark_os = max_mark;
        release_sem(semid, 3);
        printinfo(st_ind, reg_num, st_mark_ca, st_nof_el);
    }

    if (pst->stdata[st_ind].leader == 1) {
        free(member_indexes);
    }

    shmdt(pst);
    exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv)
{
    pid_t pid = getpid();
    reg_num = generate_regnum(pid);
    st_class = get_turn(reg_num);
    st_mark_ca = generate_random_integer(pid);
    st_nof_el = get_pref();
    st_inv = read_conf("nof_invites");
    last_minute = 0;

    sthandle.sa_handler = sthandler;
    sigemptyset(&mask);
    sthandle.sa_mask = mask;
    sthandle.sa_flags = 0;

    if (sigaction(SIGUSR1, &sthandle, NULL) == -1) {
        TEST_ERROR
    }

    memid = create_memory();
    pst = (shared *)connect(memid);
    max_reject = read_conf("max_reject");
    semid = create_sem();
    // CRITICAL AREA
        
    take_sem(semid, 0);    
    st_ind = pst->pc;
    pst->stdata[st_ind].student_pid = pid;
    pst->stdata[st_ind].class = st_class;
    pst->stdata[st_ind].mark_os = 0;
    pst->stdata[st_ind].mark_ca = st_mark_ca;
    pst->stdata[st_ind].max_mark_ca = st_mark_ca;
    pst->stdata[st_ind].team = 0;
    pst->stdata[st_ind].leader = 0;
    pst->stdata[st_ind].closed = 0;
    pst->stdata[st_ind].nof_elems = st_nof_el;
    pst->stdata[st_ind].nof_invites = st_inv;
    pst->pc ++; 
    take_sem(semid, 2);   
    release_sem(semid, 0);

    msgid = create_queue();
    lastid = msgget(LMS, 0666 | IPC_CREAT);

    ready(semid, 1);
    // General condition to access invitation code
    // 1) I'm not in a team
    // 2) I'm the leader but I've not yet closed the team
    while (pst->stdata[st_ind].team == 0 
        || (pst->stdata[st_ind].leader == 1 
        && pst->stdata[st_ind].closed == 0)) 
    {
        take_sem(semid, 0);
        while (receive_msg_nowait(msgid) != -1)
        {
            if (pst->stdata[st_ind].team == 0) {
                if (max_reject > 0) {
                    if (st_mark_ca > 26) {
                        if (st_nof_el == pst->stdata[invitation.sender_index].nof_elems) {
                            accept(st_ind);
                        } else if (find_team_mate(st_ind) == -1) {
                            accept(st_ind);
                        } else {
                            decline(st_ind);
                            max_reject--;
                        }
                    } else {
		                if (st_nof_el == pst->stdata[invitation.sender_index].nof_elems) {
                            if (invitation.max_mark >= st_mark_ca || st_nof_el == 0) {
                                accept(st_ind);
                            } else {
                                decline(st_ind);
                                max_reject--;
                            }
                        } else {
                            if ((invitation.max_mark - 3) >= st_mark_ca || st_nof_el == 0) {
                                accept(st_ind);
                            } else {
                                decline(st_ind);
                                max_reject--;
                            }
                        }	    
                    }
	            } else {
	                accept(st_ind);
                }
            } else {
                decline(st_ind);
			}
		}//while
        
        // Student may not find messages in the queue so we check if a particular error occurred
        if (errno) {
            if (errno != ENOMSG) {
                TEST_ERROR
            }
        }

        // I'm not in a team, I search for team mates
        if (((pst->stdata[st_ind].leader && st_nof_el != nelem_team) || 
            pst->stdata[st_ind].team == 0) && pst->stdata[st_ind].nof_invites > 0) 
        {
            if (pst->stdata[st_ind].leader) {
                if ((pod = find_team_mate(st_ind)) != -1) { 
                    wait_answer = invite(st_ind, pod, st_mark_ca);
                }
            } else {
                if ((pod = find_team_mate(st_ind)) != -1) { 
                    wait_answer = invite(st_ind, pod, st_mark_ca);
                } else {
                    if ((pod = find_random_mate(st_ind)) != -1) {
                        wait_answer = invite(st_ind, pod, st_mark_ca);
                    }
                }
            }
        } //search mates

        // I'm leader, I've found all necessaries team mates, I close the team
        if (pst->stdata[st_ind].leader && st_nof_el == nelem_team) {
            lock_group(member_indexes, nelem_team, max_mark);
        }

        // No more to invite, I'm leader, I close the team
        if (pst->stdata[st_ind].leader && pst->stdata[st_ind].nof_invites == 0 && wait_answer == 0) {
            lock_group(member_indexes, nelem_team, max_mark);
        }

        // I'm alone, I can't invite and the others are no more inviting, so I close the team
        if (pst->stdata[st_ind].team == 0 && wait_answer == 0) {
            if (pst->stdata[st_ind].nof_invites == 0) {
                if((pod = find_inviting_mate(st_ind)) == -1) {
                    pst->stdata[st_ind].leader = 1;
                    pst->stdata[st_ind].team = 1;
                    nelem_team = 1;
                    member_indexes = (int *)calloc(nelem_team,sizeof(int));
                    member_indexes[0] = st_ind;
                    lock_group(member_indexes, nelem_team, st_mark_ca);
                }
            } else { // If I can invite I search for someone
                if ((pod = find_inviting_mate(st_ind)) != -1) { 
                    if ((pod = find_random_mate(st_ind)) != -1) {
                        wait_answer = invite(st_ind, pod, st_mark_ca);
                        st_inv--;
                    }
                }
            }
        }

        release_sem(semid, 0);

        // If the process invited someone it executes this part of code
        while(wait_answer) {
            if (msgrcv(msgid, &invitation, sizeof(invitation), getpid(), 0) != -1) {
                if (invitation.invited) {
                    decline(st_ind);
                } else {
                    if (invitation.accept) {
                        if (pst->stdata[st_ind].leader == 0) {
                            pst->stdata[st_ind].leader = 1;
                            pst->stdata[st_ind].team = 1;
                            nelem_team = 2;
                            member_indexes = (int *)calloc(nelem_team, sizeof(int));
                            member_indexes[0] = st_ind;
                            member_indexes[nelem_team - 1] = invitation.sender_index;
                            if (invitation.max_mark > max_mark) {
                                max_mark = invitation.max_mark;
                            }
                        } else {
                            nelem_team++;
                            member_indexes = (int *)realloc(member_indexes, nelem_team * sizeof(int));
                            member_indexes[nelem_team - 1] = invitation.sender_index;
                            if (invitation.max_mark > max_mark) {
                                max_mark = invitation.max_mark;
                            }
                        }
                    }
                    pst->stdata[st_ind].max_mark_ca = max_mark;
                    wait_answer = 0;
                }                
            }
        }	    
    }//while
    
    masksig();

    // Waiting for os mark..
    if (msgrcv(lastid, &lastmsg, sizeof(lastmsg), getpid(), 0) == -1) {
        TEST_ERROR
    } else {
        max_mark = lastmsg.mark;
        take_sem(semid, 3);
        pst->stdata[st_ind].mark_os = max_mark;
        release_sem(semid, 3);
        printinfo(st_ind, reg_num, st_mark_ca, st_nof_el);
    }

    if (pst->stdata[st_ind].leader == 1) {
        free(member_indexes);
    }

    shmdt(pst);
    exit(EXIT_SUCCESS);
}
