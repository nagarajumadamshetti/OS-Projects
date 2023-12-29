.text
pferrcode: .long 0               // Define a long integer to store the page fault error code
.globl  pfintr, pferrcode        // Declare the symbols pfintr and pferrcode as global

pfintr:                          // Start of the page fault interrupt handler
    popl pferrcode               // Pop the error code off the stack and store it in pferrcode
    pushfl                       // Push the FLAGS register onto the stack (to save its state)
    cli                          // Clear the interrupt flag (disable interrupts)
    pushal                       // Push all general-purpose registers onto the stack (to save their state)
    call pfint                   // Call the pfint function (C function for handling the page fault)
    popal                        // Pop all general-purpose registers off the stack (to restore their state)
    popfl                        // Pop the FLAGS register off the stack (to restore its state)
    iret                         // Return from the interrupt (also restores the CS and EIP registers)
