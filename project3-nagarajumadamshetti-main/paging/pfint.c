#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/* Function prototypes */
void initialize_page_table_entry(pt_t *page_table_entry);
void handle_page_directory_entry_not_present(pd_t *page_directory_entry, unsigned long pdbr, int *new_frame);
void handle_page_table_entry_not_present(pt_t *page_table_entry, unsigned long vaddr, pd_t *page_directory_entry);
void update_page_table_entry(pt_t *page_table_entry, int new_frame_pt);
void read_from_backing_store(unsigned long vaddr, int new_frame_pt);

/* pfint - paging fault ISR */
SYSCALL pfint() {
    STATWORD ps;
    disable(ps); // Disable interrupts

    unsigned long vaddr = read_cr2(); // Read the faulting virtual address
    unsigned long pdbr = proctab[currpid].pdbr; // Get the Page Directory Base Register for current process
    virt_addr_t *virtual_add = (virt_addr_t*)&vaddr; // Cast the address to a virtual address structure
    unsigned int vpd_offset = virtual_add->pd_offset; // Extract page directory offset
    unsigned int vpt_offset = virtual_add->pt_offset; // Extract page table offset
    pd_t *page_directory_entry = pdbr + vpd_offset * sizeof(pd_t); // Get the page directory entry
    int new_frame;

    // Check if the page directory entry is present
    if (!page_directory_entry->pd_pres) {
        handle_page_directory_entry_not_present(page_directory_entry, pdbr, &new_frame);
    }

    // Get the page table entry
    pt_t *page_table_entry = (pt_t*) (page_directory_entry->pd_base * NBPG + vpt_offset * sizeof(pt_t));
    // Check if the page table entry is present
    if (!page_table_entry->pt_pres) {
        handle_page_table_entry_not_present(page_table_entry, vaddr, page_directory_entry);
    }

    write_cr3(pdbr); // Write to control register to update page tables
    restore(ps); // Restore interrupts
    return OK;
}

void initialize_page_table_entry(pt_t *page_table_entry) {
    // Initialize each page table entry
    int i;
    for (i = 0; i < NFRAMES; i++) {
        page_table_entry[i] = (pt_t){0}; // Set all bits to 0
    }
}

void handle_page_directory_entry_not_present(pd_t *page_directory_entry, unsigned long pdbr, int *new_frame) {
    // Handle the scenario where a page directory entry is not present
    get_frm(new_frame); // Allocate a new frame
    pt_t *page_table_entry = (pt_t*)((FRAME0 + *new_frame) * NBPG); // Get the page table entry
    initialize_page_table_entry(page_table_entry); // Initialize the page table

    // Update the frame table
    frm_tab[*new_frame] = (fr_map_t){.fr_status = FRM_MAPPED, .fr_type = FR_TBL, .fr_pid = currpid};

    // Update the page directory entry
    *page_directory_entry = (pd_t) {1,1, 0, 0 , 0,0,0,0,0,0, FRAME0 + *new_frame};
}

void handle_page_table_entry_not_present(pt_t *page_table_entry, unsigned long vaddr, pd_t *page_directory_entry) {
    // Handle the scenario where a page table entry is not present
    int new_frame_pt;
    get_frm(&new_frame_pt); // Allocate a new frame for the page table
    update_page_replacement(new_frame_pt); // Update the page replacement information

    update_page_table_entry(page_table_entry, new_frame_pt); // Update the page table entry
    frm_tab[new_frame_pt] = (fr_map_t){ .fr_status = FRM_MAPPED, .fr_type = FR_PAGE, .fr_pid = currpid, .fr_vpno = vaddr/NBPG};

    // Increment reference count of the page directory entry
    frm_tab[page_directory_entry->pd_base - FRAME0].fr_refcnt++;

    // Read the required page from the backing store
    int bs_id, pageth;
    bsm_lookup(currpid, vaddr, &bs_id, &pageth);
    read_from_backing_store(vaddr, new_frame_pt);
}

void update_page_table_entry(pt_t *page_table_entry, int new_frame_pt) {
    // Update the properties of a page table entry
    *page_table_entry = (pt_t){.pt_pres =1, .pt_write=1, .pt_base = FRAME0 + new_frame_pt};
}

void read_from_backing_store(unsigned long vaddr, int new_frame_pt) {
    // Read data from the backing store and load it into a frame
    int bs_id, pageth;
    bsm_lookup(currpid, vaddr, &bs_id, &pageth); // Lookup the backing store and page
    read_bs((char*)((FRAME0 + new_frame_pt) * NBPG), bs_id, pageth); // Read from the backing store into the frame
}
