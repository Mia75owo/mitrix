#ifndef DEBUG_H_
#define DEBUG_H_

#include "util/types.h"

void klog(char* str);
void klog_num(u64 num, u16 base);

void kassert(u32 cond, const char* file, u32 line, const char* err);
#define assert(x) ( kassert((u32)(x), __FILE__, __LINE__, "") )
#define assert_msg(x, msg) ( kassert((u32)(x), __FILE__, __LINE__, (msg)) )

void abort();

#endif
