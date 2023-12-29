/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
SYSCALL	recvtim(int maxwait)
{
	update_syscall_stats_count(currpid, "sys_recvtim", 8);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;
	STATWORD ps;    
	struct	pentry	*pptr;
	int	msg;

	if (maxwait<0 || clkruns == 0){
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_recvtim", 8, exec_time);
		return(SYSERR);
	}
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait*1000);
		slnempty = TRUE;
		sltop = (int *)&q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);

    exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_recvtim", 8, exec_time);

	return(msg);
}
