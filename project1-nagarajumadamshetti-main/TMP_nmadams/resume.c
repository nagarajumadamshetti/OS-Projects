/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */

SYSCALL resume(int pid)
{
	update_syscall_stats_count(currpid, "sys_resume", 9);
	// kprintf("\nRESUME: ,PID:  %d, time: %lu",currpid, ctr1000);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_resume", 9, exec_time);
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
    exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_resume", 9, exec_time);

	return(prio);
}
