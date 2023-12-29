/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include<math.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);


void update_preemption(){
	#ifdef RTCLOCK
		// Set the preemption timer to the quantum value
		preempt = QUANTUM; // Quantum is a constant defining the time slice allocated to each process
	#endif
}
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry *optr;	/* pointer to old process entry */
	register struct	pentry *nptr;	/* pointer to new process entry */
	register struct	linux_process *olptr;	
	register struct	linux_process *nlptr;

	if (getschedclass() == EXPDISTSCHED){
        // Check if the current process is ready to run
        if((optr= &proctab[currpid])->pstate == PRCURR){
            // Mark the current process as ready
            optr->pstate = PRREADY;
            // Add the current process to the ready list
            insert(currpid, rdyhead, optr->pprio);
        }

        // Generate a random number
        int random_value = (int) expdev(0.1); 

        // Find the next process to run using the exponential scheduling algorithm
        int next_process = exponential_next_process(random_value);

		currpid = dequeue(next_process); // Dequeue the next process from the ready list

        nptr = &proctab[currpid]; // Get a pointer to the structure containing information about the current process
        nptr->pstate = PRCURR; // Set the state of the current process to "current"
        update_preemption();
	} else if(getschedclass()==LINUXSCHED){ // Check if the current scheduler is the Linux scheduler
	
		// Initialize variables
		optr = &proctab[currpid]; // pointer to the current process table entry
		olptr = &linux_array[currpid]; // pointer to the current process's slot in the linux_array
		int max_goodness=-2; // maximum goodness value found so far
		int max_goodness_index=-2; // index of the process with the maximum goodness value
		int i=0; // loop counter
		
		// Loop through all ready processes
		for ( i = q[rdytail].qprev; i != rdyhead; i = q[i].qprev) {
			// Check if the current process has a higher goodness value than the maximum found so far
			if (linux_array[i].goodness > max_goodness) {
				max_goodness = linux_array[i].goodness;
			}
		}

		for ( i = q[rdytail].qprev; i != rdyhead; i = q[i].qprev) {
			// Check if the current process has a higher goodness value than the maximum found so far
			if (linux_array[i].goodness == max_goodness) {
				max_goodness_index = i;
				break;
			}
		}

		// Update the goodness value of the current process
		linux_array[currpid].goodness -= (linux_array[currpid].counter - preempt);
		
		// Check if the current process should be rescheduled
		if(preempt == 0){
			linux_array[currpid].goodness = 0;
		}

		// Update the counter of the current process
		if(currpid == NULLPROC) {
			linux_array[currpid].goodness = 0;
			linux_array[currpid].counter = 0;
		} else {
			linux_array[currpid].counter = preempt;
		}

		// Return OK if the current process has a positive goodness value and is currently being executed
		if (linux_array[currpid].goodness > 0 && linux_array[currpid].goodness >= max_goodness && optr->pstate == PRCURR) {
			return OK;
		}else if (max_goodness > 0 && (optr->pstate != PRCURR || linux_array[currpid].counter == 0 || linux_array[currpid].goodness < max_goodness)) {
			// If the current process should be rescheduled, move it to the ready queue
			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->pprio);
			}

			// Set the next process to be executed
			nptr = &proctab[max_goodness_index];
			nptr->pstate = PRCURR;
			max_goodness_index = dequeue(max_goodness_index);

			currpid = max_goodness_index;
			preempt = linux_array[currpid].counter;
		}
		else if((optr->pstate != PRCURR || linux_array[currpid].counter == 0) && max_goodness == 0){ // Check if there are any other processes that should be rescheduled
			if (currpid == NULLPROC) {
				return OK;
			} 
			i = 0; // Initialize loop counter to 0

			while ((i < NPROC)) { // Loop until i exceeds NPROC
				if (proctab[i].pstate != PRFREE) { // If the process at index i is not free
					linux_array[i].counter = proctab[i].pprio + (linux_array[i].counter / 2); // Update the counter for this process by adding its priority and dividing the result by 2
					linux_array[i].goodness = proctab[i].pprio + linux_array[i].counter; // Update the goodness value for this process by adding its priority and counter
				}
				i += 1; // Increment the loop counter
			}


			// If no processes have a positive goodness value, return OK
			if (max_goodness == 0) {
				// Move the current process to the ready queue
				if (optr->pstate == PRCURR) {
					optr->pstate = PRREADY;
					insert(currpid, rdyhead, optr->pprio);
				}
				
				// Set the next process to be executed
				nptr = &proctab[NULLPROC];
				nptr->pstate = PRCURR;
				dequeue(NULLPROC);
				currpid = NULLPROC;
				
				// Update the preemption
				update_preemption();
			}
		} 
	}else{
		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		(lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
		
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		update_preemption();
	}

	// kprintf("10\n");

	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	// kprintf("11\n");
	
	/* The OLD process returns here when resumed. */
	return OK;
}
