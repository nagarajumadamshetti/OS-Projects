/* lab0.h */

#ifndef _LAB0_H_
#define _LAB0_H_

#define SYS_FREEMEM 0

extern unsigned long ctr1000;


extern int printprocstks(int priority);


extern void syscallsummary_stop();
extern void printsyscallsummary();
extern void syscallsummary_start();
extern void update_syscall_stats(int pid, const char *syscall_name, int syscall_num, unsigned long exec_time);
extern void update_syscall_stats_count(int pid, const char *syscall_name, int syscall_num);

extern long zfunction(long parameter);

#endif
