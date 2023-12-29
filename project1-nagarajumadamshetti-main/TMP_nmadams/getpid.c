/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
    update_syscall_stats_count(currpid, "sys_getpid", 2);
	unsigned long start_time = ctr1000;

    unsigned long exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_getpid", 2, exec_time);

	return(currpid);
}
