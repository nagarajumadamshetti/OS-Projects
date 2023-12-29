#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL vfreemem(block, size)
    struct mblock  *block; // The starting address of the block to free
    unsigned size;         // Size of the block to free
{
    STATWORD ps;    
    struct mblock *currBlock, *prevBlock;
    unsigned top;

    // Check if size is non-zero and block address is valid
    if (size != 0 && (unsigned)block >= 4096 * NBPG) {
        size = (unsigned)roundmb(size); // Round the size to nearest memory block
        disable(ps); // Disable interrupts

        // Iterate through the virtual memory list to find the position of the block
        for(currBlock = proctab[currpid].vmemlist->mnext, prevBlock = proctab[currpid].vmemlist;
            !(currBlock == (struct mblock *) NULL || currBlock >= block);
            prevBlock = currBlock, currBlock = currBlock->mnext);

        // Check if the block can be merged with adjacent blocks
        if (!((top = prevBlock->mlen + (unsigned)prevBlock) <= (unsigned)block && prevBlock != proctab[currpid].vmemlist) &&
            !(currBlock != NULL && (size + (unsigned)block) <= (unsigned)currBlock)) {

            // Merge with previous block if adjacent
            if (prevBlock != proctab[currpid].vmemlist && top == (unsigned)block) {
                prevBlock->mlen += size;
            } else {
                // Otherwise, insert the block into the list
                block->mlen = size;
                block->mnext = currBlock;
                prevBlock->mnext = block;
                prevBlock = block;
            }

            // Merge with next block if adjacent
            if ((unsigned)(prevBlock->mlen + (unsigned)prevBlock) != (unsigned)currBlock) {
                restore(ps); // Restore interrupts
                return OK;
            } else {
                prevBlock->mlen += currBlock->mlen;
                prevBlock->mnext = currBlock->mnext;
            }
        }
    }

    restore(ps); // Restore interrupts
    return SYSERR; // Return error if size is zero or block address is invalid
}
