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

static FILE files[16];

FILE *fopen(const char *path, const char *mode) {
    i32 file_slot = -1;
    for (u32 i = 0; i < 16; i++) {
        if (files[i].file_id == 0) {
            file_slot = i;
            break;
        }
    }
    if (file_slot == -1) {
        syscall_print("MAX FILES EXEEDED!\n");
        syscall_exit();
        return 0;
    }

    u32 file_id = syscall_file_open((char*)path);
    if (file_id == 0) {
        printf("FOPEN file_id == 0!\n");
        return 0;
    }
    files[file_slot].file_id = file_id;
    files[file_slot].vmt = &fm;

    printf("FOPEN File Slot: %i\n", file_slot);
    printf("FOPEN FileID: %i\n", file_id);
    return &files[file_slot];
}

int fclose(FILE *stream) {
    printf("FCLOSE FileID: %i\n", stream->file_id);

    syscall_file_close(stream->file_id);
    stream->file_id = 0;

    return 0;
}

int fseek(FILE *stream, long offset, int whence) {
    if (whence == SEEK_SET) {
        syscall_set_file_offset(stream->file_id, offset);
        return 0;
    }

    if (whence == SEEK_CUR) {
        i32 cur_offset = (i32)syscall_get_file_offset(stream->file_id);

        syscall_set_file_offset(stream->file_id, cur_offset + offset);
        return 0;
    }

    if (whence == SEEK_END) {
        i32 size = (i32)syscall_get_file_size(stream->file_id);

        syscall_set_file_offset(stream->file_id, size + offset);
        return 0;
    }

    return -1;
}
long ftell(FILE *stream) {
    return syscall_get_file_offset(stream->file_id);
}

int mkdir(const char *path, uint32_t mode) {
    syscall_print("MKDIR\n");
}
int remove(const char *pathname) {
    syscall_print("REMOVE\n");
}
int rename(const char *oldpath, const char *newpath) {
    syscall_print("RENAME\n");
}
int fflush(FILE *stream) {
    syscall_print("FFLUSH\n");
}
int system(const char *command) {
    printf("SYSTEM: %s\n", command);
    return -1;
}

void exit(int status) {
    (void)status;
    syscall_exit();
}

#define isdigit(c) (c >= '0' && c <= '9')

double atof(const char *s) {
    // This function stolen from either Rolf Neugebauer or Andrew Tolmach.
    // Probably Rolf.
    double a = 0.0;
    int e = 0;
    int c;
    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a * 10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a * 10.0 + (c - '0');
            e = e - 1;
        }
    }
    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i * 10 + (c - '0');
            c = *s++;
        }
        e += i * sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return a;
}
