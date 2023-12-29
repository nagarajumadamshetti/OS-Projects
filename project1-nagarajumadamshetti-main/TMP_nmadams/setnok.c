/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(int nok, int pid)
{
	update_syscall_stats_count(currpid, "sys_setnok", 14);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_setnok", 14, exec_time);
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_setnok", 14, exec_time);
		
	return(OK);
}
