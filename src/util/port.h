#ifndef PORT_H_
#define PORT_H_

#include "types.h"

static inline void outb(u16 port, u8 value) {
    asm volatile("outb %1, %0" ::"dN"(port), "a"(value));
}
static inline void outw(u16 port, u16 value) {
    asm volatile("outw %1, %0" ::"dN"(port), "a"(value));
}

static inline u8 inb(u16 port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}
static inline u16 inw(u16 port) {
    u16 ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

#endif
