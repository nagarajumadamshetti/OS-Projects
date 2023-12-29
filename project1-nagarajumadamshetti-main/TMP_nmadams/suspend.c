/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
SYSCALL	suspend(int pid)
{
	update_syscall_stats_count(currpid, "sys_suspend", 24);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority returned		*/

	disable(ps);
	if (isbadpid(pid) || pid==NULLPROC ||
	 ((pptr= &proctab[pid])->pstate!=PRCURR && pptr->pstate!=PRREADY)) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_suspend", 24, exec_time);
		return(SYSERR);
	}
	if (pptr->pstate == PRREADY) {
		pptr->pstate = PRSUSP;
		dequeue(pid);
	}
	else {
		pptr->pstate = PRSUSP;
		resched();
	}
	prio = pptr->pprio;
	restore(ps);
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_suspend", 24, exec_time);
	return(prio);
}
