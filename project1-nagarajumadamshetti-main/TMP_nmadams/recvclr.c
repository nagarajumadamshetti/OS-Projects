/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>


// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{
	update_syscall_stats_count(currpid, "sys_recvlr", 7);
	unsigned long start_time = ctr1000;
	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
   	unsigned long exec_time = ctr1000 - start_time;

	// int pid = getpid();
    // Update statistics for this system call
    update_syscall_stats(currpid, "sys_recvlr", 7, exec_time);

	return(msg);
}
