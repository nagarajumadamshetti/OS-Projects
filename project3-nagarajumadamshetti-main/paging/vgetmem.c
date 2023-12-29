#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD *vgetmem(nbytes)
    unsigned nbytes; // The number of bytes to allocate
{
    STATWORD ps;    
    struct mblock *p, *q, *leftover;

    disable(ps); // Disable interrupts
    // Check for invalid allocation size or empty virtual memory list
    if (nbytes == 0 || proctab[currpid].vmemlist->mnext == (struct mblock *) NULL) {
        restore(ps); // Restore interrupts
        return (WORD *)SYSERR; // Return error
    }

    nbytes = (unsigned int) roundmb(nbytes); // Round the number of bytes to allocate

    // Traverse the virtual memory list to find a suitable block
    for (q = proctab[currpid].vmemlist, p = proctab[currpid].vmemlist->mnext;
         p != (struct mblock *) NULL;
         q = p, p = p->mnext) {
        if (p->mlen == nbytes) {
            // Exact fit found, allocate and remove from list
            q->mnext = p->mnext;
            restore(ps); // Restore interrupts
            return (WORD *)p; // Return the allocated block
        } else if (p->mlen > nbytes) {
            // Allocate from a larger block, splitting it
            leftover = (struct mblock *)((unsigned)p + nbytes);
            q->mnext = leftover;
            leftover->mnext = p->mnext;
            leftover->mlen = p->mlen - nbytes;
            restore(ps); // Restore interrupts
            return (WORD *)p; // Return the allocated block
        }
    }
    restore(ps); // Restore interrupts
    return (WORD *)SYSERR; // Return error if no suitable block found
}
