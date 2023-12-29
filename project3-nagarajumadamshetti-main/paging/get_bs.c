#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {
    // Request a new mapping of npages with backing store ID (bs_id)

    STATWORD ps;
    disable(ps); // Disable interrupts for atomic operations

    // Check for validity of the request: npages should be within range and bs_id should be valid
    if (!npages || npages > 256 || bs_id < 0 || bs_id > 7) {
        restore(ps); // Restore interrupts
        return SYSERR; // Return error if conditions not met
    } else if (bsm_tab[bs_id].bs_status == BSM_UNMAPPED) {
        // If the backing store is currently unmapped, map it to the current process
        bsm_tab[bs_id] = (bs_map_t){BSM_MAPPED, currpid, 0, npages, 0, 0};
        restore(ps); // Restore interrupts
        return npages; // Return the number of pages requested
    } 
    // Code commented out - potentially to check if the store is already mapped to the current process
    // else if (bsm_tab[bs_id].bs_status == BSM_MAPPED && bsm_tab[bs_id].bs_pid == currpid) {
    //   restore(ps);
    //   return bsm_tab[bs_id].bs_npages;
    // } 
    else if (bsm_tab[bs_id].bs_status == BSM_MAPPED) {
        // If the store is already mapped (but not to the current process)
        restore(ps); // Restore interrupts
        // Return the number of pages if heap is not set, otherwise return error
        int ret = bsm_tab[bs_id].has_heap == 0 ? bsm_tab[bs_id].bs_npages : SYSERR;
        return ret;
    }

    restore(ps); // Restore interrupts
    return npages; // Return the number of pages requested
}
