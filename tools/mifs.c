#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define FNAME_SIZE 64

typedef struct {
    uint32_t files;
    uint32_t fname_size;
} __attribute__((packed)) MIFS_Header;

typedef struct {
    char name[FNAME_SIZE];
    uint32_t size;
} __attribute__((packed)) MIFS_File;

int main(int argc, char **argv) {
    if (argc <= 2) {
        printf("Not enough arguments! Exiting!\n");
        exit(1);
    }

    // Skip the program
    argc--;
    argv++;

    // Output file
    const char* outf = *argv;
    argc--;
    argv++;

    // Check if all files exist
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
        if (access(argv[i], F_OK) != 0) {
            printf("Does not exist! Exiting!\n");
            exit(1);
        }
    }

    MIFS_Header mifs_header = {0};
    mifs_header.files = argc;
    mifs_header.fname_size = FNAME_SIZE;

    FILE* out_file = fopen(outf, "w+");

    // Write header
    fwrite(&mifs_header, sizeof(mifs_header), 1, out_file);

    // Write file headers
    for (int i = 0; i < argc; i++) {
        FILE* file = fopen(argv[i], "r");
        MIFS_File file_header;

        // Get file size
        fseek(file, 0L, SEEK_END);
        file_header.size = ftell(file);

        // Get file name
        char* file_name = argv[i];
        if (strncmp(argv[i], "./", 2) == 0) {
            file_name += 2;
        }
        if (strlen(file_name) > FNAME_SIZE) {
            printf("File: '%s' exeeds the file name size: %i", file_name, FNAME_SIZE);
            exit(1);
        }
        memset(file_header.name, 0, FNAME_SIZE);
        strncpy(file_header.name, file_name, FNAME_SIZE - 1);

        // Close file and write header
        fclose(file);
        fwrite(&file_header, sizeof(file_header), 1, out_file);

        // Write file content
        file = fopen(argv[i], "r");
        int c;
        while ((c = getc(file)) != EOF) {
            putc(c, out_file);
        }
        fclose(file);
    }
    // 
    // // Write file contents
    // for (int i = 0; i < argc; i++) {
    //     FILE* file = fopen(argv[i], "r");
    //     int c;
    //     while ((c = getc(file)) != EOF) {
    //         putc(c, out_file);
    //     }
    // }

    fclose(out_file);

    return 0;
}
