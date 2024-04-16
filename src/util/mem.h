#ifndef MEM_H_
#define MEM_H_

#include "util/types.h"

#define CEIL_DIV(a, b) (((a + b) - 1) / b)

typedef union {
    struct {
        u8 lower;
        u8 higher;
    };
    u16 val;
} Word;
typedef union {
    struct {
        Word lower;
        Word higher;
    };
    u32 val;
} DWord;

void* memset(void* dest, u8 val, u32 len);
void* memcpy(void* dest, const void* src, u32 len);
bool memcmp(void* a, void* b, u32 len);
char* strchr(const char* str, char c);

u32 strlen(const char* str);

void memrev(void* dest, u32 len);

void itoa(char* dest, u64 val, u32 len, u16 base);

u8 ctoi(char c);
u64 atoi(char* str, u16 base);

bool cishex(char c);

#endif
