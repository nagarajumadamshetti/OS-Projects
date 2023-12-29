/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(int sem)
{
	update_syscall_stats_count(currpid, "sys_wait", 26);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_wait", 26, exec_time);
		return(SYSERR);
	}

	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_wait", 26, exec_time);
		return pptr->pwaitret;
	}
	restore(ps);

	exec_time = ctr1000 - start_time;
	// kprintf("\n\n\n\nWAIT: %d abc", currpid);
	update_syscall_stats(currpid, "sys_wait", 26, exec_time);

	return(OK);
}
