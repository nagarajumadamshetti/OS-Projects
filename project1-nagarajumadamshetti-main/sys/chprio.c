/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	update_syscall_stats_count(currpid, "sys_chprio", 1);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_chprio", 1, exec_time);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);

    exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_chprio", 1, exec_time);

	return(newprio);
}
