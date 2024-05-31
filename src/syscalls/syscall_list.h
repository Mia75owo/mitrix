#ifndef SYSCALL_LIST_H_
#define SYSCALL_LIST_H_

#define SYSCALL_STDIN_FILE 0
#define SYSCALL_STDOUT_FILE 1
#define SYSCALL_STDERR_FILE 2

// clang-format off
#define SYSCALL_EXIT        0x0
#define SYSCALL_PRINT       0x1
#define SYSCALL_PRINT_CHAR  0x2
#define SYSCALL_GET_SYSTIME 0x3

#define SYSCALL_READ        0xA
#define SYSCALL_WRITE       0xB
// clang-format on

#endif
