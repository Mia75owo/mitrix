#ifndef MEM_H_
#define MEM_H_

#include "util/types.h"

#define CEIL_DIV(a, b) (((a + b) - 1) / b)

static inline u32 GET_BIT(u32 num, u32 n) { return (num >> n) & 1ul; }
static inline u32 SET_BIT(u32 num, u32 n, bool value) {
    return (num & ~((u32)1 << n)) | ((u32)value << n);
}

// Bitmap
#define BITMAP_GET(bitmap, index) GET_BIT((bitmap)[(index) / 32], (index) % 32)
#define BITMAP_SET(bitmap, index, value) \
    ((bitmap)[(index) / 32] =            \
         SET_BIT((bitmap)[(index) / 32], (index) % 32, value))

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
u32 strcmp(const char* s1, const char* s2);
u32 strncmp(const char* s1, const char* s2, u32 n);
char* strchr(const char* str, char c);
char* strncpy(char* dest, const char* src, u32 n);
char* strncat(char* dest, const char* src, u32 n);

u32 strlen(const char* str);

void memrev(void* dest, u32 len);

void itoa(char* dest, u64 val, u32 len, u16 base);

u8 ctoi(char c);
u64 atoi(char* str, u16 base);

char toupper(char c);
char tolower(char c);

bool cishex(char c);

#endif
