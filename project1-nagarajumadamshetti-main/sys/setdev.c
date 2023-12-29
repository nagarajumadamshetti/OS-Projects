/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	update_syscall_stats_count(currpid, "sys_setdev", 13);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	short	*nxtdev;

	if (isbadpid(pid)){
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_setdev", 13, exec_time);
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_setdev", 13, exec_time);
	return(OK);
}
