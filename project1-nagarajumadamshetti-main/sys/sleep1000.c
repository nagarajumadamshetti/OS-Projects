/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep1000(int n)
{
	update_syscall_stats_count(currpid, "sys_sleep1000", 21);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;
	STATWORD ps;    

	if (n < 0  || clkruns==0){
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_sleep1000", 21, exec_time);
		return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
	restore(ps);
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_sleep1000", 21, exec_time);
	return(OK);
}
