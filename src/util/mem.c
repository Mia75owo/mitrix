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
u32 strcmp(const char* s1, const char* s2) {
    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}
u32 strncmp(const char* s1, const char* s2, u32 n) {
    register unsigned char u1, u2;
    while (n-- > 0) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2) return u1 - u2;
        if (u1 == '\0') return 0;
    }
    return 0;
}
char* strncpy(char* dest, const char* src, u32 n) {
    if (n != 0) {
        char* d = dest;
        const char* s = src;

        do {
            if ((*d++ = *s++) == 0) {
                while (--n != 0) *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return dest;
}
char* strncat(char* dst, const char* src, u32 n) {
    char* q = strchr(dst, '\0');
    const char* p = src;
    char ch;

    while (n--) {
        *q++ = ch = *p++;
        if (!ch) return dst;
    }
    *q = '\0';

    return dst;
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

char toupper(char c) {
	return (c >= 'a' && c <= 'z') ? (c & ~32) : c;
}

char tolower(char c) {
	return (c >= 'A' && c <= 'Z') ? (c | 32) : c;
}
