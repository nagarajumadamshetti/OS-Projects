/* printsyscallsummary.c - printsyscallsummary */

#include <kernel.h>
#include <proc.h>
// #include <stdio.h>

int trackEnabled = 0;
// Function to update statistics for a system call
void update_syscall_stats(int pid, const char *syscall_name, int syscall_num, unsigned long exec_time) {
    if(trackEnabled==1){
    // kprintf("\nTrack enabled: %d, PID: %d, syscall: %s",trackEnabled, pid, syscall_name);
        strcpy(proctab[pid].syscall_stats[syscall_num].syscall_name, syscall_name);
        // proctab[pid].syscall_stats[syscall_num].count++;
        proctab[pid].syscall_stats[syscall_num].total_exec_time += exec_time;
    }
    return;
}

void update_syscall_stats_count(int pid, const char *syscall_name, int syscall_num) {
    if(trackEnabled==1){
    // kprintf("\nTrack enabled: %d, PID: %d, syscall: %s",trackEnabled, pid, syscall_name);
        strcpy(proctab[pid].syscall_stats[syscall_num].syscall_name, syscall_name);
        proctab[pid].syscall_stats[syscall_num].count++;
        // proctab[pid].syscall_stats[syscall_num].total_exec_time += exec_time;
    }
    return;
}

void syscallsummary_start() {
    trackEnabled = 1;
    int i = 0,j=0;
    for(j=0;j<NPROC;j++){
        for (i=0; i < NUM_SYSCALLS; i++) {
            proctab[j].syscall_stats[i].count = 0;
            proctab[j].syscall_stats[i].total_exec_time = 0;
        }
    }
    return;
}

void syscallsummary_stop() {
    trackEnabled = 0;
    return;
}

void printsyscallsummary() {
    // kprintf("\nprintsyscallsummary");
    int i = 0,j=0;
    for(j=0;j<NPROC;j++){
        int flag=1;
        for (i=0; i < NUM_SYSCALLS; i++) {
            if (proctab[j].syscall_stats[i].count > 0) {
                if(flag){
                    printf("Process [pid:%d]\n", j);
                    flag=0;
                }
                printf("    Syscall: %s, count: %d, average execution time: %lu (ms)\n",
                    proctab[j].syscall_stats[i].syscall_name, proctab[j].syscall_stats[i].count,
                    proctab[j].syscall_stats[i].total_exec_time / proctab[j].syscall_stats[i].count);
            }
        }
    }
    return;
}

