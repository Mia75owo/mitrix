#ifndef SYSCALL_LIST_H_
#define SYSCALL_LIST_H_

#define SYSCALL_STDIN_FILE 0
#define SYSCALL_STDOUT_FILE 1
#define SYSCALL_STDERR_FILE 2

// clang-format off
#define SYSCALL_EXIT              0x0
#define SYSCALL_PRINT             0x1
#define SYSCALL_PRINT_CHAR        0x2
#define SYSCALL_GET_SYSTIME       0x3

#define SYSCALL_CREATE_FB         0x4
#define SYSCALL_DRAW_FB           0x5
#define SYSCALL_REQUEST_SCREEN    0x6

#define SYSCALL_CREATE_EVENTS_BUF 0x7

#define SYSCALL_FILE_OPEN         0x8
#define SYSCALL_FILE_CLOSE        0x9

#define SYSCALL_READ              0xA
#define SYSCALL_WRITE             0xB

#define SYSCALL_GET_HEAP_START    0xC
#define SYSCALL_GET_HEAP_END      0xD
#define SYSCALL_SET_HEAP_SIZE     0xE
// clang-format on

#endif
