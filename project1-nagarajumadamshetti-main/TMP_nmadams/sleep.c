/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	update_syscall_stats_count(currpid, "sys_sleep", 18);
	// kprintf("\nSLEEP: ,PID:  %d, time: %lu",currpid, ctr1000);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	if (n<0 || clkruns==0){
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_sleep", 18, exec_time);
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_sleep", 18, exec_time);
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_sleep", 18, exec_time);
	return(OK);
}
