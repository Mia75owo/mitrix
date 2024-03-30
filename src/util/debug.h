#ifndef DEBUG_H_
#define DEBUG_H_

#include "util/types.h"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

void klog(char* str);
void klog_num(u64 num, u16 base);

void kpanic_(const char* first, ...);
#define kpanic(...) ( kpanic_(__VA_ARGS__, (char*)0) )

#define assert(x) if (!(x)) { kpanic(__FILE__, ":", STRINGIZE(__LINE__), " assert failed"); }
#define assert_msg(x, msg) if (!(x)) { kpanic(__FILE__, ":", STRINGIZE(__LINE__), " assert failed: ", (msg)); }

void abort();

#endif
