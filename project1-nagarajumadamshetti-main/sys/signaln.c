/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
	update_syscall_stats_count(currpid, "sys_signaln", 17);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_signaln", 17, exec_time);
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_signaln", 17, exec_time);
	return(OK);
}
