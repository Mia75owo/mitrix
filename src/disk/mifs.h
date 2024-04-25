#ifndef MIFS_H_
#define MIFS_H_

#include "util/types.h"

#define FNAME_SIZE 64

typedef struct {
    u32 files;
    u32 fname_size;
} __attribute__((packed)) MIFS_Header;

typedef struct {
    char name[FNAME_SIZE];
    u32 size;
} __attribute__((packed)) MIFS_File;

typedef struct {
    u8* start;
    u8* end;
    MIFS_File* first_file;
    u32 files;
} MIFS;

typedef struct {
    u8* addr;
    u32 size;
} FilePtr;

void mifs_init(u8* start, u8* end);
FilePtr mifs_file(char* file_name);

#endif
