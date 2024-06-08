#ifndef SYS_H_
#define SYS_H_

#include "util/types.h"
__attribute__((noreturn)) void spin_halt();
void halt();
__attribute__((noreturn)) void abort();
__attribute__((noreturn)) void reboot();
__attribute__((noreturn)) void shutdown();
void sleep(u32 ms);

void cli_push();
void cli_pop();

// clang-format off
typedef enum {
    EF_CARRY               = (1 << 0),
    EF_RESERVED1           = (1 << 1),
    EF_PARITY              = (1 << 2),
    EF_RESERVED2           = (1 << 3),
    EF_AUX_CARRY           = (1 << 4),
    EF_RESERVED3           = (1 << 5),
    EF_ZERO                = (1 << 6),
    EF_SIGN                = (1 << 7),
    EF_TRAP                = (1 << 8),
    EF_INTERRUPTS          = (1 << 9),
    EF_DIRECTION           = (1 << 10),
    EF_OVERFLOW            = (1 << 11),
    EF_IO_PRIVILEGE        = (1 << 12),
    EF_NESTET_TASK         = (1 << 13),
    EF_MODE                = (1 << 14),
    EF_RESUME              = (1 << 15),
    EF_VIRTUAL8086         = (1 << 16),
    EF_ALIGNMENT_CHECK     = (1 << 17),
    EF_VIRTUAL_INT         = (1 << 18),
    EF_VIRTUAL_INT_PENDING = (1 << 19),
    EF_CPUID_INSTR         = (1 << 20),
    EF_RESERVED4           = (1 << 21),
    EF_AES_KEY_SCHEDULE    = (1 << 22),
    EF_ALTERNATE_INSTR_SET = (1 << 23),
    // 23-32 reserved
} EFLAG;
// clang-format on

static inline u32 eflags_get() {
    u32 eflags;
    asm volatile(
        "pushfl \n"
        "popl %0 \n"
        : "=r"(eflags));
    return eflags;
}

static inline bool eflags_check(EFLAG flag) { return eflags_get() & flag; }

#endif
