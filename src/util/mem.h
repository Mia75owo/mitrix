#ifndef MEM_H_
#define MEM_H_

#include "util/types.h"

void* memset(void* dest, u8 val, u32 len);
void* memcpy(void* dest, const void* src, u32 len);

u32 strlen(const char* str);

void memrev(void* dest, u32 len);

void itoa(char* dest, u64 val, u32 len, u16 base);

u8 ctoi(char c);
u64 atoi(char* str, u16 base);

#endif
