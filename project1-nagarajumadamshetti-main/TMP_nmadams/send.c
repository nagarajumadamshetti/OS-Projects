/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(int pid, WORD msg)
{
	update_syscall_stats_count(currpid, "sys_send", 12);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);

	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg != 0) {
		restore(ps);
	    exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_send", 12, exec_time);
		return(SYSERR);
	}

	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV)	/* if receiver waits, start it	*/
		ready(pid, RESCHYES);
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);

	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_send", 12, exec_time);

	return(OK);
}
