#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr, ssize, hsize, priority, name, nargs, args)
    int *procaddr;       /* procedure address        */
    int ssize;           /* stack size in words      */
    int hsize;           /* virtual heap size in pages   */
    int priority;        /* process priority > 0     */
    char *name;          /* name (for debugging)     */
    int nargs;           /* number of args that follow   */
    long args;           /* arguments (treated like an   */
                        /* array in the code)       */
{
    STATWORD ps;
    disable(ps); // Disable interrupts

    int backingStoreId, newProcId;
    struct mblock *heapStartAddress;

    // Create a new process
    newProcId = create(procaddr, ssize, priority, name, nargs, args);
    if (newProcId == SYSERR || !allocateBackingStore(&backingStoreId, hsize, newProcId)) {
        kill(newProcId); // Clean up if allocation fails
        restore(ps);
        return SYSERR;
    }

    // Setup virtual heap for the process
    setupProcessVirtualHeap(backingStoreId, hsize, newProcId);
    restore(ps); // Restore interrupts
    return newProcId; // Return the new process ID
}

int allocateBackingStore(int *backingStoreId, int heapSize, int procId) {
    // Allocate a backing store for the process's virtual heap
    if (get_bsm(backingStoreId) != OK) {
        return 0; // Failure
    }

    if (bsm_map(procId, 4096, *backingStoreId, heapSize) != OK) {
        return 0; // Failure
    }

    bsm_tab[*backingStoreId].has_heap = 1;
    return 1; // Success
}

void setupProcessVirtualHeap(int backingStoreId, int heapSize, int procId) {
    // Configure the virtual heap in the process's memory
    if(backingStoreId >= 0){
        struct mblock *bsAddress = BACKING_STORE_BASE + (backingStoreId * BACKING_STORE_UNIT_SIZE);
        bsAddress->mlen = heapSize * NBPG;
        bsAddress->mnext = NULL;
        if(procId >= -1){
            proctab[procId].vhpnpages = heapSize;
            proctab[procId].vmemlist->mnext = 4096 * NBPG;
        }
    }
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL newpid()
{
    int pid;            /* process id to return      */
    int i;

    for (i = 0; i < NPROC; i++) {    /* check all NPROC slots    */
        if ( (pid = nextproc--) <= 0)
            nextproc = NPROC - 1;
        if (proctab[pid].pstate == PRFREE)
            return(pid);
    }
    return(SYSERR);
}
