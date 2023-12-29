/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep100(int n)
{
	update_syscall_stats_count(currpid, "sys_sleep100", 20);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    

	if (n < 0  || clkruns==0){
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_sleep100", 20, exec_time);
		return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
	restore(ps);
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_sleep100", 20, exec_time);
	return(OK);
}
