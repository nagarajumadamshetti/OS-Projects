/* unsleep.c - unsleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * unsleep  --  remove  process from the sleep queue prematurely
 *------------------------------------------------------------------------
 */
SYSCALL	unsleep(int pid)
{
	update_syscall_stats_count(currpid, "sys_unsleep", 25);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	pentry	*pptr;
	struct	qent	*qptr;
	int	remain;
	int	next;

    disable(ps);
	if (isbadpid(pid) ||
	    ( (pptr = &proctab[pid])->pstate != PRSLEEP &&
	     pptr->pstate != PRTRECV) ) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_unsleep", 25, exec_time);
		return(SYSERR);
	}
	qptr = &q[pid];
	remain = qptr->qkey;
	if ( (next=qptr->qnext) < NPROC)
		q[next].qkey += remain;
	dequeue(pid);
	if ( (next=q[clockq].qnext) < NPROC)
		sltop = (int *) & q[next].qkey;
	else
		slnempty = FALSE;
        restore(ps);

	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_unsleep", 25, exec_time);
	return(OK);
}
