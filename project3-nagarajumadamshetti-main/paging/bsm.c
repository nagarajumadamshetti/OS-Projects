/* bsm.c - manage the backing store mapping */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*--------------------------------------------------------------------------
 * init_bsm - initialize bsm_tab
 * Initializes each entry in the Backing Store Map (bsm_tab) to default values.
 *--------------------------------------------------------------------------
 */
SYSCALL init_bsm() {
    STATWORD 	ps;
	disable(ps);
	int i = 0;
	while(i < 8){
		bsm_tab[i] = (bs_map_t) {BSM_UNMAPPED, -1, 4096, 0, 0, 0 };
		i+=1;
	}

	restore(ps);
	return(OK);
}

/*--------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 * Searches for a free (unmapped) entry in the Backing Store Map (bsm_tab) and 
 * returns its index.
 *--------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail) {
	STATWORD ps;
	disable(ps);
	int i = 0;
	while(i<8){
		if (bsm_tab[i].bs_status != BSM_UNMAPPED) {
			i+=1;
			continue;
		}else{
			*avail = i;
			restore(ps);
			return(OK);
		}
		i+=1;
	}
	restore(ps);
	return SYSERR;
}

/*--------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 * Frees up a specified entry in the Backing Store Map, resetting it to default values.
 *--------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i) {
	STATWORD ps;
	disable(ps);
    if(i<0 || i > 7 ||  bsm_tab[i].bs_status != BSM_MAPPED) {
        restore(ps);	
        return SYSERR;
    }
	bsm_tab[i] = (bs_map_t) {BSM_UNMAPPED, -1, 4096, 0, 0, 0 };

    restore(ps);		
    return(OK);
}

/*--------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 * Looks up the Backing Store Map for a given process ID and virtual address, 
 * returning the store and page threshold.
 *--------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {
	STATWORD ps;
	disable(ps);
	int i = 0;
	while(i<8){
		if(bsm_tab[i].bs_pid != pid || bsm_tab[i].bs_status != BSM_MAPPED) {
			i+=1;
			continue;
		}else if (bsm_tab[i].bs_pid == pid && bsm_tab[i].bs_status == BSM_MAPPED) {
			int reference = (vaddr/NBPG);
			*pageth = reference - bsm_tab[i].bs_vpno;
			*store = i;
			restore(ps);
			return(OK);	
		}
		i+=1;
	}
	restore(ps);
	return SYSERR;
}

/*--------------------------------------------------------------------------
 * bsm_map - add a mapping into bsm_tab 
 * Maps a given virtual page number (vpno) to a source in the Backing Store Map,
 * for a particular process ID.
 *--------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages) {
	STATWORD ps;
	disable(ps);
    
	// validate condition
    if(bsm_tab[source].has_heap == 1 && bsm_tab[source].bs_pid != pid) {
        restore(ps);
        return SYSERR;
    }

	if ((bsm_tab[source].has_heap == 1 && bsm_tab[source].bs_pid == pid) || bsm_tab[source].has_heap == 0) {
		proctab[currpid].vhpno = vpno;
		proctab[currpid].store = source;
		bsm_tab[source] = (bs_map_t) {BSM_MAPPED, pid, vpno, npages, bsm_tab[source].bs_sem, bsm_tab[source].has_heap };
		restore(ps);
		return(OK);
	}
	restore(ps);
	return SYSERR;	
}

/*--------------------------------------------------------------------------
 * bsm_unmap - delete a mapping from bsm_tab
 * Removes a mapping from the Backing Store Map for a given process ID and virtual page number.
 *--------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag) {
    STATWORD ps;
    disable(ps);
    
    int bs_id, pageth;

    if (bsm_lookup(pid, vpno, &bs_id, &pageth) == SYSERR) {
        restore(ps);
        return SYSERR;
    }
    
    clearBackingStoreMapping(bs_id);

    if (!writeBackFramePage(pid, bs_id, pageth)) {
        restore(ps);
        return SYSERR;
    }

    restore(ps);
    return OK;
}

/* Helper function to clear a backing store mapping */
void clearBackingStoreMapping(int bs_id) {
	bsm_tab[bs_id] = (bs_map_t) {BSM_UNMAPPED, -1, 4096, 0, 0, 0 };
}

/* Helper function to write back a frame page */
int writeBackFramePage(int pid, int bs_id, int pageth) {
    int i = 0;
    for (; i < NFRAMES; i++) {
        if (frm_tab[i].fr_status != FRM_MAPPED || frm_tab[i].fr_pid != pid || frm_tab[i].fr_type != FR_PAGE) {
            continue;
        }

        write_bs((i + NFRAMES) * NBPG, bs_id, pageth);
        return 1;  // Return true indicating success
    }
    return 0;  // Return false indicating failure
}

