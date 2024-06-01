#ifndef DEBUG_H_
#define DEBUG_H_

#include "util/types.h"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

void klog(char* format, ...);
void kpanic(const char* format, ...);

#define assert(x)                                                     \
    if (!(x)) {                                                       \
        kpanic("%s:%s assert failed", __FILE__, STRINGIZE(__LINE__)); \
    }
#define assert_msg(x, msg)                          \
    if (!(x)) {                                     \
        kpanic("%s:%s assert failed: %s", __FILE__, \
               STRINGIZE(__LINE__), (msg));         \
    }

#endif
