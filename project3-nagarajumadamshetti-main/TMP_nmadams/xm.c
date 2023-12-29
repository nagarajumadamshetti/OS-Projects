#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
    STATWORD ps;
    disable(ps); // Disable interrupts

    // Validate the parameters: source ID, virtual page number, and number of pages
    if (source < 0 || source > 7 || virtpage < 4096 || npages < 1 || npages > 256) {
        restore(ps); // Restore interrupts
        return SYSERR; // Return error if parameters are invalid
    } else {
        // Map the virtual pages to the given backing store
        int ret = bsm_map(currpid, virtpage, source, npages);
        if (ret != SYSERR) {
            restore(ps); // Restore interrupts
            return OK; // Return OK if mapping is successful
        }       
    }
    restore(ps); // Restore interrupts
    return SYSERR; // Return error if mapping fails
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    STATWORD ps;
    disable(ps); // Disable interrupts

    // Validate the virtual page number
    if (virtpage < 4096) {
        restore(ps); // Restore interrupts
        return SYSERR; // Return error if virtual page number is invalid
    }

    // Unmap the virtual page from the backing store
    bsm_unmap(currpid, virtpage, 0);
    restore(ps); // Restore interrupts
    return OK; // Return OK after unmapping
}
