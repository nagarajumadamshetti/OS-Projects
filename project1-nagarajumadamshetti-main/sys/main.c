#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

int prX;
void halt();

/*------------------------------------------------------------------------
*  main  --  user main program
*------------------------------------------------------------------------
*/
prch(c)
char c;
{
    int i;
    sleep(5);	
}
int main()
{
	long param=0xaabbccdd;
	long ans=zfunction(param);
	kprintf("\n\nHello CSC 501:\n\nzfunction(%x) => %lx\n\n", param, ans);
	// int bit= (int)(8 * sizeof(long *));
	// kprintf("\n\n Size: %d",bit);
	printprocstks(-1);
    kprintf("\nvoid printsyscallsummary()\n");
    syscallsummary_start();        
    resume(prX = create(prch,2000,20,"proc X",1,'A'));
    sleep(10);
    syscallsummary_stop();
    printsyscallsummary();
    return 0;
}

// /*------------------------------------------------------------------------
//  *  main  --  user main program
//  *------------------------------------------------------------------------
//  */

// int main()
// {
// 	syscallsummary_start();
// 	long param=0xaabbcdd;
// 	long ans=zfunction(param);
// 	kprintf("\n\nHello CSC 501:\n\n %lx\n\n",ans);
// 	int bit= (int)(8 * sizeof(long *));
// 	kprintf("\n\n Size: %d",bit);
// 	kprintf("\n\nProcessess");
// 	printprocstks(19);
// 	sleep(10);
// 	syscallsummary_stop();
// 	printsyscallsummary();
// 	return 0;
// }
