#ifndef SERIAL_H_
#define SERIAL_H_

#include "util/types.h"

void serial_init();
void serial_write(char c);
void serial_puts(char* str);
void serial_put_num(u64 num, u16 base);

#endif
