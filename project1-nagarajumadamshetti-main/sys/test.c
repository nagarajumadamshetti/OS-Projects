#include <limits.h>
#include <stdio.h>
//function to clear nth-bit
long clearNthBit(long data,unsigned int pos)
{
    return (data & (~(1 << pos)));
}


long ztask1(long param){
    // for 32 bit: start from 4 as it is from right
    // similarly for 64 bit : start from 32
    int bit= (int)(CHAR_BIT * sizeof(void *));
    int n = 4;
    long one =1;
    // long inpud = 0xfffff00fffffffff;
    // if(bit==64){
    //     n=36;
    // }else{
    //     n=4;
    // }
    int i=0;
    for(i=0;i<8;i++){
        param &= ~(one << n+i);
    }
    
    return param;
}

long ztask2(long param){
    param <<= 8;
    return param;
}

long ztask3(long param){
    param |= 0xFF;
    return param;
}

// long zfunction(long param) {
//     long tresult1 = ztask1(param);
//     long tresult2 = ztask2(tresult1);
//     long tresult3 = ztask3(tresult2);
//     return tresult3;
// }
long zfunction(long param){
    param &= 0xFFFFF00F;
    printf("%lx\n",param);
    param <<= 8;
    printf("%lx\n",param);
    param |= 0x000000FF;
    printf("%lx\n",param);
    return param;
}

int main() {
    // long input = 0xaaaabBBbccccdddd;
    // long inpud = 0xfffff00fffffffff;
    
    // printf("bitwise and: %lx\n",input & inpud);;
    
    long input = 0xaabbccdd;
    // printf("\n stage:1: %\n",input);

    printf("Input:  0x%x\n", input);
    
    long tresult1 = ztask1(input);
    printf("task1:  0x%x\n", tresult1);;
    
    long tresult2 = ztask2(tresult1);
    printf("task2:  0x%x\n", tresult2);;
    
    long tresult3 = ztask3(tresult2);
    printf("task3:  0x%x\n", tresult3);;
    printf("\n\nHello CSC 501\n\n");
	int bit= (int)(8 * sizeof(long *));
	printf("\n\n Size: %d\n",bit);
    return 0;
}


// // #include <limits.h>
// // #include <stdio.h>

// // int main()
// // {
// //      printf("%d bits\n", (int)(CHAR_BIT * sizeof(void *)));
// //      return 0;
// // }
// #include <limits.h>
// #include <stdio.h>

// //function to clear nth-bit
//  typedef struct {
//     int pid;
//     char* name;
// } process;

// // process* p = malloc(20 * sizeof *p);
// process* p = malloc(20 * sizeof(process));
// p[0].pid = 1;
// sprintf(p[0].name,"process1");

// // p[1]->pid = 2;
// // sprintf(p[1]->name,"process2");

// long clearNthBit(long data,unsigned int pos)
// {
//     return (data & (~(1 << pos)));
// }


// long ztask1(long param){
//     // for 32 bit: start from 4 as it is from right
//     // similarly for 64 bit : start from 32
//     int bit= (int)(CHAR_BIT * sizeof(void *));
//     int n = 4;
//     long one =1;
//     // long inpud = 0xfffff00fffffffff;
//     // if(bit==64){
//     //     n=36;
//     // }else{
//     //     n=4;
//     // }
//     for(int i=0;i<8;i++){
//         param &= ~(one << n+i);
//     }
    
//     return param;
// }

// long ztask2(long param){
//     param <<= 8;
//     return param;
// }

// long ztask3(long param){
//     param |= 0xFF;
//     return param;
// }

// long zfunction(long param) {
//     param = ztask1(param);
//     param = ztask2(param);
//     param = ztask3(param);
//     return param;
// }

// int main() {
//     long input = 0xaabbccdd;
//     // long inpud = 0xfffff00fffffffff;
    
//     // printf("bitwise and: %lx\n",input & inpud);;
    
//     // long input = 0xaabbccdd;
//     // printf("\n stage:1: %\n",input);

//     printf("Input:  0x%lx\n", input);
    
//     long tresult1 = ztask1(input);
//     printf("task1:  0x%x\n", tresult1);;
    
//     long tresult2 = ztask2(tresult1);
//     printf("task2:  0x%x\n", tresult2);;
    
//     long tresult3 = ztask3(tresult2);
//     printf("task3:  0x%x\n", tresult3);;
//     return 0;
// }
