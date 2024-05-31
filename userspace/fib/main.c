#include "stdio.h"
#include "syscalls.h"
#include "types.h"

int _start() {
    u64 t1 = 0;
    u64 t2 = 1;
    u64 nextTerm = 0;
    u64 n;

    printf("Generate the first N fib numbers: ");

    // Read user input
    char buf[64];
    fgets(buf, 64, stdin);
    sscanf(buf, "%d", &n);

    // displays the first two terms which is always 0 and 1
    printf("\nFibonacci Series: %d, %d, ", t1, t2);
    nextTerm = t1 + t2;

    for (u32 i = 0; i < n; i++) {
        printf("%u, ", nextTerm);
        t1 = t2;
        t2 = nextTerm;
        nextTerm = t1 + t2;
    }

    syscall_exit();
    return 0;
}
