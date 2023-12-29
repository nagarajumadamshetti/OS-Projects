/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
	update_syscall_stats_count(currpid, "sys_signal", 16);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_signal", 16, exec_time);
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	exec_time = ctr1000 - start_time;
	// kprintf("\n\n\n\nSIGNAL: %d abc", currpid);
	update_syscall_stats(currpid, "sys_signal", 16, exec_time);
	return(OK);
}
