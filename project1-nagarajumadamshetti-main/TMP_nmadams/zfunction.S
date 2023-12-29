  /* Start the code section */
  .file	"zfunction.c"
  .text
  .code32
  /* Define zfunction as a global (exported) function. */
  .globl zfunction
  .type zfunction, @function
zfunction:

  /* Subroutine Prologue */
  pushl %ebp      /* Save the old base pointer value. */
  movl %esp, %ebp /* Set the new base pointer value. */

  /* Subroutine Body */
  movl 8(%ebp), %eax   /* Move value of parameter into EAX. */

  andl $0xFFFFF00F, %eax  /* Clear the 20th to 27th bits, counting from the left starting with 0. */
  shl $8, %eax            /* Shift by 8 bits to the left. */
  orl $0xff, %eax         /* Fill the right-most 8 bits with 1s. */

  /* Subroutine Epilogue */
  pop %ebp       /* Restore the caller's base pointer value. */
  ret
