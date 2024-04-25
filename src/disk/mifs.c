#include "mifs.h"

#include "util/debug.h"
#include "util/mem.h"

static MIFS fs;

void mifs_init(u8* start, u8* end) {
    assert(start < end);

    fs.start = start;
    fs.end = end;

    MIFS_Header* header = (MIFS_Header*)start;
    assert_msg(header->fname_size == FNAME_SIZE, "MIFS mismatch!");

    fs.files = header->files;
    fs.first_file = (MIFS_File*)(start + sizeof(MIFS_Header));
}

FilePtr mifs_file(char* file_name) {
    u8* ptr = (u8*)fs.first_file;

    while (ptr < (u8*)fs.end) {
        MIFS_File* file = (MIFS_File*)ptr;
        ptr += sizeof(MIFS_File);

        if (strncmp(file_name, file->name, FNAME_SIZE) == 0) {
            return (FilePtr){ptr, file->size};
        }

        ptr += file->size;
    }

    return (FilePtr){NULL, 0};
}