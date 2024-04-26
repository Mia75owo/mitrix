#ifndef SYS_H_
#define SYS_H_

#include "util/types.h"
void spin_halt();
void halt();
void abort();
void reboot();
void shutdown();
void sleep(u32 ms);

#endif
