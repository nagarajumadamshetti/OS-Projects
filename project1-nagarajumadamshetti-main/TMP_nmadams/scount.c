/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include<proc.h>
#include <lab0.h>


// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
	update_syscall_stats_count(currpid, "sys_scount", 10);
extern	struct	sentry	semaph[];
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
	    exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_scount", 10, exec_time);
		return(SYSERR);
	}
    exec_time = ctr1000 - start_time;

	// int pid = currpid;
    // Update statistics for this system call
    update_syscall_stats(currpid, "sys_scount", 10, exec_time);

	return(semaph[sem].semcnt);
}
