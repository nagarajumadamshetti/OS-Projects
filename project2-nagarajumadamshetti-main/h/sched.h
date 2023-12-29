/* sched.h - task2 */

#ifndef _SCHED_H_
#define _SCHED_H_


#define EXPDISTSCHED 1
#define LINUXSCHED 2

struct linux_process {
    int counter;
	int goodness;
};

extern struct linux_process linux_array[];
extern int sched_class_global;

extern void setschedclass (int sched_class);
extern int getschedclass();
extern double expdev(double lambda);
extern int exponential_next_process(int random);
#endif 
