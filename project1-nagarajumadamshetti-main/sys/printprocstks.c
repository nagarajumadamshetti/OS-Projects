/* printprocstks.c - printprocstks*/
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long	*esp;

int printprocstks(int priority) {
	kprintf("void printprocstks(int priority)\n");
    int i = 0;
	for (i = 0; i < NPROC; i++) {
		if(proctab[i].pprio > priority && proctab[i].pstate != PRFREE){
			// Print process information
			kprintf("Process [%s]\n", proctab[i].pname);
			kprintf("    pid: %d\n", i);
			kprintf("    priority: %d\n", proctab[i].pprio);
			kprintf("    base: 0x%08x\n", proctab[i].pbase);
			kprintf("    limit: 0x%08x\n", proctab[i].plimit);
			kprintf("    len: %d\n", proctab[i].pstklen);
			if (i == currpid) {
				asm("movl %esp,esp");
				kprintf("    pointer: 0x %08x\n\n", esp);
			}
			else{
				kprintf("    pointer: 0x %08x\n\n", proctab[i].pesp);
			}
		}
    }

    return 0;
}
