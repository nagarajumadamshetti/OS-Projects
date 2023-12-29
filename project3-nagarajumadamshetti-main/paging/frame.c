/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

extern int debugging;
extern int page_replace_policy;

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
    STATWORD ps;
    disable(ps); // Disable interrupts to ensure atomic operation

    // Iterate over all frames and initialize them
    int i = 0;
    for (; i < NFRAMES; i++)
    {
        // Initialize each frame with default values:
        // Status: FRM_UNMAPPED (not currently mapped to any virtual page)
        // Type: FR_PAGE (type of the frame, here it's a page)
        // Remaining values set to default (-1 or 0)
        frm_tab[i] = (fr_map_t){FRM_UNMAPPED, FR_PAGE, -1, 0, 0, 0};        
    }

    restore(ps); // Restore interrupts
    return(OK);
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according to page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
    STATWORD ps;
    disable(ps); // Disable interrupts

    // Try to find an unmapped (free) frame
    int frame_id = find_unmapped_frame();
    if (frame_id != -1) {
        *avail = frame_id; // If found, set the available frame
        restore(ps);
        return OK;
    }

    // If no unmapped frame, use page replacement policy to get a frame
    frame_id = get_frame_from_policy();

    // If unable to get a frame using the policy, return error
    if(frame_id < 0){
        restore(ps);
        return SYSERR;
    }

    // Free the frame obtained from the policy and set it as available
    free_frm(frame_id);
    *avail = frame_id;
    
    restore(ps); // Restore interrupts
    return OK;
}
// ... previous functions ...

int find_unmapped_frame() {
    // Iterate over all frames to find an unmapped one
    int i = 0;
    for (; i < NFRAMES; i++) {
        // Check if the current frame is unmapped
        if (frm_tab[i].fr_status == FRM_UNMAPPED) {
            return i; // Return the index of the unmapped frame
        }
    }
    return -1; // Return -1 if no unmapped frame is found
}

void updatePageAccess(pt_t *page_table_entry) {
    // Reset the access flag of a page table entry
    page_table_entry->pt_acc = 0;
}

pt_t *getPageTableEntry_sc(unsigned long pdbr, virt_addr_t *virtual_add) {
    // Calculate the page table entry address using directory base register and virtual address
    pd_t *page_directory_entry = pdbr + virtual_add->pd_offset * sizeof(pd_t);
    return (pt_t*) (page_directory_entry->pd_base * NBPG + virtual_add->pt_offset * sizeof(pt_t));
}

int removeFromQueue(int frameIdToRemove, int prev) {
    // Remove a frame from the replacement queue
    if (prev == -1) {
        // If it's the first frame in the queue
        page_replacement_head = page_replacement_tab[frameIdToRemove].frame_next;
    } else {
        // If it's not the first frame, adjust the previous frame's next pointer
        page_replacement_tab[prev].frame_next = page_replacement_tab[frameIdToRemove].frame_next;
    }
    // Mark the removed frame's next pointer as -1 (indicating it's no longer in the queue)
    page_replacement_tab[frameIdToRemove].frame_next = -1;
    return frameIdToRemove;
}

int getFrameIdToReplace_SC() {
    // Implement Second-Chance algorithm to find a frame to replace
    int current = page_replacement_head;
    int prev = -1;
    while (current != -1) {
        // Get the page table entry for the current frame
        unsigned long pdbr = proctab[currpid].pdbr;
        unsigned long vaddr = frm_tab[current].fr_vpno * NBPG;
        virt_addr_t *virtual_add = (virt_addr_t*)&vaddr;
        pt_t *page_table_entry = getPageTableEntry_sc(pdbr, virtual_add);

        // Check access bit; if set, reset it and move to next frame
        if (page_table_entry->pt_acc) {
            updatePageAccess(page_table_entry);
        } else {
            // If access bit not set, this is the frame to replace
            return removeFromQueue(current, prev);
        }
        prev = current;
        current = page_replacement_tab[current].frame_next;
    }
    // If all frames had the access bit set, remove the first frame in the queue
    return removeFromQueue(page_replacement_head, -1);
}

int get_frame_from_policy() {
    STATWORD ps;
    disable(ps); // Disable interrupts

    // Obtain a frame to replace based on the second-chance algorithm
    int frame_id = getFrameIdToReplace_SC();

    // Optional debugging message
    if (debugging == 1) {
        kprintf("\nRemove frame-  %d\n", frame_id);
    }

    restore(ps); // Restore interrupts
    return frame_id;
}

typedef struct {
    pd_t *page_directory_entry;
    pt_t *page_table_entry;
    int backing_store_id;
    int page_offset;
} FrameInfo;

FrameInfo getFrameInfo(int frame_number) {
    // Function to get detailed information about a specific frame

    // Calculating virtual address
    unsigned long virtual_address = frm_tab[frame_number].fr_vpno * NBPG;
    virt_addr_t *virtual_addr_struct = (virt_addr_t *)&virtual_address;

    // Calculating page directory and table offsets
    unsigned int page_directory_offset = virtual_addr_struct->pd_offset;
    unsigned int page_table_offset = virtual_addr_struct->pt_offset;

    // Retrieve page directory and table entries
    unsigned long pdbr = proctab[frm_tab[frame_number].fr_pid].pdbr;
    pd_t *page_directory_entry = pdbr + page_directory_offset * sizeof(pd_t);
    pt_t *page_table_entry = (pt_t *)(page_directory_entry->pd_base * NBPG + page_table_offset * sizeof(pt_t));

    // Get backing store ID and page offset
    int backing_store_id = proctab[frm_tab[frame_number].fr_pid].store;
    int page_offset = frm_tab[frame_number].fr_vpno - proctab[frm_tab[frame_number].fr_pid].vhpno;

    // Return all collected frame information
    FrameInfo frameInfo;
    frameInfo.page_directory_entry = page_directory_entry;
    frameInfo.page_table_entry = page_table_entry;
    frameInfo.backing_store_id = backing_store_id;
    frameInfo.page_offset = page_offset;

    return frameInfo;
}

SYSCALL free_frm(int frame_number)
{
    STATWORD ps;
    disable(ps); // Disable interrupts for atomic operation

    // Check if the frame number is valid
    if (!isValidFrame2(frame_number)) {
        restore(ps);
        return SYSERR;
    }

    // Check if the frame is a page frame
    if (!isPageFrame2(frame_number)) {
        restore(ps);
        return SYSERR;
    }

    // Get detailed information about the frame
    FrameInfo frame_info = getFrameInfo(frame_number);

    // Write the contents of the frame to the backing store
    write_to_backing_store((frame_number + FRAME0) * NBPG, frame_info.backing_store_id, frame_info.page_offset);

    // Reset the corresponding page table entry
    reset_page_table_entry(frame_info.page_table_entry);

    // Check if the page table is no longer referenced and unmap if so
    if (decrement_and_check_page_table_refcount(frame_info.page_directory_entry)) {
        unmap_page_directory_entry(frame_info.page_directory_entry);
    }

    restore(ps); // Restore interrupts
    return OK;
}

int isValidFrame2(int frame_number) {
    // Check if frame number is within valid range
    return (frame_number >= 0 && frame_number < NFRAMES);
}

int isPageFrame2(int frame_number) {
    // Check if the frame is of type 'FR_PAGE'
    return (frm_tab[frame_number].fr_type == FR_PAGE);
}

void reset_page_table_entry(pt_t *page_table_entry)
{
    // Reset the presence flag of the page table entry
    page_table_entry->pt_pres = 0;
}

int decrement_and_check_page_table_refcount(pd_t *page_directory_entry)
{
    // Decrement the reference count of the page table and check if it reaches zero
    int frame_index = page_directory_entry->pd_base - FRAME0;
    frm_tab[frame_index].fr_refcnt--;

    return frm_tab[frame_index].fr_refcnt == 0;
}

void unmap_page_directory_entry(pd_t *page_directory_entry)
{
    // Unmap the page directory entry and reset its frame entry in the frame table
    int frame_index = page_directory_entry->pd_base - FRAME0;
    frm_tab[frame_index] = (fr_map_t){FRM_UNMAPPED, FR_PAGE, -1, 4096, frm_tab[frame_index].fr_refcnt, frm_tab[frame_index].fr_dirty};  
}

void write_to_backing_store(unsigned long address, int store_id, int page_offset)
{
    // Write the contents of a frame to the backing store
    write_bs(address, store_id, page_offset);
}

void initialize_page_replacement()
{
    // Initialize the page replacement queue
    int i = 0;
    while (i < NFRAMES)
    {
        page_replacement_tab[i] = (page_replacement_queue){i,-1,0};
        i+=1; // Increment the counter
    }
}

void initializePageReplacementHead(int frame_id) {
    // Initialize the head of the page replacement list
    page_replacement_head = frame_id;
}

void appendToPageReplacementList(int frame_id) {
    int current = page_replacement_head;

    // Traverse to the end of the list
    while (page_replacement_tab[current].frame_next != -1) {
        current = page_replacement_tab[current].frame_next;
    }

    // Append the new frame to the end of the list
    page_replacement_tab[current].frame_next = frame_id;
    page_replacement_tab[frame_id].frame_next = -1;
}

void update_page_replacement(int frame_id) {
    STATWORD ps;
    disable(ps); // Disable interrupts

    // Initialize the page replacement list if it's empty
    if (page_replacement_head == -1) {
        initializePageReplacementHead(frame_id);
        restore(ps);
        return OK;
    } else {
        // Otherwise, append the frame to the replacement list
        appendToPageReplacementList(frame_id);
    }

    restore(ps); // Restore interrupts
    return OK;
}
