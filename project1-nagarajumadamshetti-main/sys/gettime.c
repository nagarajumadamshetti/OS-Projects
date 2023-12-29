/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <lab0.h>


extern int getutim(unsigned long *);
// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    update_syscall_stats_count(currpid, "sys_getutim", 4);
    unsigned long start_time = ctr1000;
    /* long	now; */

	/* FIXME -- no getutim */

    unsigned long exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_getutim", 4, exec_time);
    return OK;
}
