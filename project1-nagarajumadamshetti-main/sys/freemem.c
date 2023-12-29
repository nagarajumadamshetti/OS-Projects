/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include <proc.h>
// #include "printsyscallsummary.c"
#include <lab0.h>


// unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL	freemem(struct mblock *block, unsigned size)
{
	update_syscall_stats_count(currpid, "sys_freemem", 0);
	unsigned long start_time = ctr1000;
	unsigned long exec_time;
	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end)){
			exec_time = ctr1000 - start_time;
			update_syscall_stats(currpid, "sys_freemem", 0, exec_time);
			return(SYSERR);
	}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		exec_time = ctr1000 - start_time;
		update_syscall_stats(currpid, "sys_freemem", 0, exec_time);
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);

    exec_time = ctr1000 - start_time;
    update_syscall_stats(currpid, "sys_freemem", 0, exec_time);

	return(OK);
}
