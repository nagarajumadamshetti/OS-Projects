/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>


// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{
	update_syscall_stats_count(currpid, "sys_receive", 6);
	unsigned long start_time = ctr1000;
	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

    unsigned long exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_receive", 6, exec_time);

	return(msg);
}
