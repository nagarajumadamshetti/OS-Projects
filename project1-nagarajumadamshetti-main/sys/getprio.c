/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	update_syscall_stats_count(currpid, "sys_getprio", 3);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_getprio", 3, exec_time);
		return(SYSERR);
	}
	restore(ps);

    exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_getprio", 3, exec_time);

	return(pptr->pprio);
}
