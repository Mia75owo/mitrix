#include "stdio.h"
#include "syscalls.h"

// To get libc to compile

size_t write(FILE* instance, const char *bp, size_t n) {
    return syscall_write(instance->file_id, (u8*)bp, n);
}
size_t read(FILE* instance, char *bp, size_t n) {
    return syscall_read(instance->file_id, (u8*)bp, n);
}

static struct File_methods fm = {
    .write = write,
    .read = read,
};

static FILE _stdin = { &fm, 0 };
static FILE _stdout = { &fm, 1 };
static FILE _stderr = { &fm, 2 };

FILE* const stdin = &_stdin;
FILE* const stdout = &_stdout;
FILE* const stderr = &_stderr;
