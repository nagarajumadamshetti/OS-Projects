
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <sched.h>
#include <math.h>



int sched_class_global = 0;

void setschedclass (int sched_class){
    sched_class_global = sched_class;
}

int getschedclass(){
    return sched_class_global;
}

int exponential_next_process(int random) {
    // Initialize first_pointer to the tail of the ready queue
    int first_pointer = q[rdytail].qprev;
    // kprintf(" FIRST : %d\n",first_pointer);

    // Initialize second_pointer to the same value as first_pointer
    int second_pointer =  q[rdytail].qprev;
    // random = 40;

    // Loop until we find a process with a key less than the random number OR
    // we reach the beginning of the ready queue
    while (first_pointer < NPROC && (random < q[first_pointer].qkey)) {
        // Check if the key of the current process is different from the key of the previous process
        if (q[first_pointer].qkey != q[second_pointer].qkey) {
            // If the keys are different, update second_pointer to point to the current process
            second_pointer = first_pointer;
        }

        // Move to the previous process in the ready queue
        first_pointer = q[first_pointer].qprev;
    }

    // Return the process ID of the process pointed to by second_pointer
    return second_pointer;
    
}
