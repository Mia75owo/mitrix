int _start() {
    unsigned int ret;
    unsigned int syscall = 0;

    char* foo = "Hello from userspace :D\n";
    unsigned int arg0 = (unsigned int)foo;

    asm volatile("int $0x80" : "=a" (ret) : "a"(syscall), "b"(arg0));

    while (1) {
    }
}
