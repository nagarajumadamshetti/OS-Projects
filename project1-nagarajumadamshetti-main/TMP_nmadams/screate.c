/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

LOCAL int newsem();
// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL screate(int count)
{
	update_syscall_stats_count(currpid, "sys_screate", 15);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;

	STATWORD ps;    
	int	sem;

	disable(ps);
	if ( count<0 || (sem=newsem())==SYSERR ) {
		restore(ps);
		
		exec_time = ctr1000 - start_time;
	    update_syscall_stats(currpid, "sys_screate", 15, exec_time);
		
		return(SYSERR);
	}
	semaph[sem].semcnt = count;
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);

	exec_time = ctr1000 - start_time;
	update_syscall_stats(currpid, "sys_screate", 15, exec_time);
		
	return(sem);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newsem()
{
	int	sem;
	int	i;

	for (i=0 ; i<NSEM ; i++) {
		sem=nextsem--;
		if (nextsem < 0)
			nextsem = NSEM-1;
		if (semaph[sem].sstate==SFREE) {
			semaph[sem].sstate = SUSED;
			return(sem);
		}
	}
	return(SYSERR);
}
