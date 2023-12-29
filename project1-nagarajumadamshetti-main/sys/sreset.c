/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(int sem, int count)
{
	update_syscall_stats_count(currpid, "sys_sreset", 22);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_sreset", 22, exec_time);
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_sreset", 22, exec_time);
	return(OK);
}
