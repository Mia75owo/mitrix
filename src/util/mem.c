#include "mem.h"

void* memset(void* dest, u8 val, u32 len) {
    u8* ptr = dest;
    for (; len != 0; len--) *ptr++ = val;
    return dest;
}
void* memcpy(void* dest, const void* src, u32 len) {
    const u8* sp = src;
    u8* dp = dest;

    for (; len != 0; len--) *dp++ = *sp++;
    return dest;
}
bool memcmp(void* a, void* b, u32 len) {
    while (len-- > 0) {
        if (*((u8*)a++) != *((u8*)b++)) {
            return false;
        }
    }
    return true;
}
char* strchr(const char* str, char c) {
    while (*str != c) {
        if (!*str) return NULL;
        str++;
    }
    return (char*)str;
}

u32 strlen(const char* str) {
    const char* ss = str;
    while (*ss) ss++;
    return ss - str;
}

void memrev(void* dest, u32 len) {
    u8* lo = dest;
    u8* hi = dest + len - 1;
    u8 swap;
    while (lo < hi) {
        swap = *lo;
        *lo++ = *hi;
        *hi-- = swap;
    }
}

void itoa(char* dest, u64 val, u32 len, u16 base) {
    if (len < 2) {
        if (len == 1) {
            dest[0] = '\0';
        }
        return;
    }

    if (val == 0) {
        dest[0] = '0';
        dest[1] = '\0';
        return;
    }

    u16 i = 0;
    while (val != 0) {
        char num = val % base;

        if (num >= 10) {
            dest[i++] = num - 10 + 65;  // A-...
        } else {
            dest[i++] = num + 48;  // 0-9
        }

        val /= base;

        if (i >= len) {
            i--;
            break;
        }
    }
    dest[i] = '\0';

    memrev(dest, i);
}

u8 ctoi(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;
}

u64 atoi(char* str, u16 base) {
    u64 num = 0;

    while (1) {
        num += ctoi(*str);

        if (*++str == '\0') {
            break;
        }
        num *= base;
    }

    return num;
}

bool cishex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}
