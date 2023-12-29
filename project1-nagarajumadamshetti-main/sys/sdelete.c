/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(int sem)
{
	update_syscall_stats_count(currpid, "sys_delete", 11);
	unsigned long start_time = ctr1000;

	unsigned long exec_time;
	STATWORD ps;    
	int	pid;
	struct	sentry	*sptr;

	disable(ps);

	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_delete", 11, exec_time);
		return(SYSERR);
	}

	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}

	restore(ps);

	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_delete", 11, exec_time);

	return(OK);
}
