
// // syscall_stats.c
// #include <kernel.h>

// // Define a struct to hold statistics for a system call
// struct syscall_stat {
//     char syscall_name[50];
//     int count;
//     unsigned long total_exec_time;
// };

// // Initialize an array of syscall_stat structures
// struct syscall_stat syscall_stats[NUM_SYSCALLS];

// // Function to update statistics for a system call
// void update_syscall_stats(const char *syscall_name, int syscall_num, unsigned long exec_time) {
//     strcpy(syscall_stats[syscall_num].syscall_name, syscall_name);
//     syscall_stats[syscall_num].count++;
//     syscall_stats[syscall_num].total_exec_time += exec_time;
// }
