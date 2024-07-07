#ifndef SYSCALL_LIST_H_
#define SYSCALL_LIST_H_

#define SYSCALL_STDIN_FILE 0
#define SYSCALL_STDOUT_FILE 1
#define SYSCALL_STDERR_FILE 2

// clang-format off
#define SYSCALL_EXIT               0
#define SYSCALL_PRINT              1
#define SYSCALL_PRINT_CHAR         2
#define SYSCALL_GET_SYSTIME        3
#define SYSCALL_READ               4
#define SYSCALL_WRITE              5
#define SYSCALL_EXEC               6
#define SYSCALL_EXEC_BLOCKING      7
#define SYSCALL_EXEC_DETACHED      8
#define SYSCALL_OWNER_TASK_DEPTH   9

#define SYSCALL_CREATE_FB         10
#define SYSCALL_DRAW_FB           11
#define SYSCALL_GET_SCREEN_SIZE_X 12
#define SYSCALL_GET_SCREEN_SIZE_Y 13

#define SYSCALL_FILE_OPEN         20
#define SYSCALL_FILE_CLOSE        21
#define SYSCALL_GET_FILE_OFFSET   22
#define SYSCALL_SET_FILE_OFFSET   23
#define SYSCALL_GET_FILE_SIZE     24
#define SYSCALL_GET_FILE_COUNT    25
#define SYSCALL_GET_FILE_NAME     26
#define SYSCALL_FILE_OPEN_INDEX   27

#define SYSCALL_GET_HEAP_START    30
#define SYSCALL_GET_HEAP_END      31
#define SYSCALL_SET_HEAP_SIZE     32

#define SYSCALL_SCHEDULER_NEXT    40
#define SYSCALL_SLEEP             41
#define SYSCALL_WAIT_FOR_EVENT    42

#define SYSCALL_CREATE_EVENTS_BUF 50
// clang-format on

#endif
