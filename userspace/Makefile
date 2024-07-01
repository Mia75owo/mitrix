CC?=~/opt/cross/bin/i686-elf-gcc
LD?=~/opt/cross/bin/i686-elf-gcc
AR?=~/opt/cross/bin/i686-elf-ar
AS?=nasm

CFLAGS =
CFLAGS += -std=gnu11
CFLAGS += -ffreestanding
CFLAGS += -fno-builtin
CFLAGS += -nostdlib
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -I../libmitrix/include
CFLAGS += -I../libc/include

ASFLAGS = -f elf32

LDFLAGS = 
LDFLAGS += -ffreestanding
LDFLAGS += -fno-builtin
LDFLAGS += -nostdlib
LDFLAGS += -L../libmitrix -l:libmitrix.a -L../libc -l:libc.a -static-libgcc -lgcc

export

all:
	mkdir -p bin
	$(MAKE) -C libmitrix
	$(MAKE) -C libc
	$(MAKE) -C libgfx

	$(MAKE) -C utest
	cp utest/*.elf bin/
	$(MAKE) -C fib
	cp fib/*.elf bin/
	$(MAKE) -C flappy
	cp flappy/*.elf bin/
	$(MAKE) -C doomgeneric
	cp doomgeneric/*.elf bin/

gen_cc_json:
	$(MAKE) -C libmitrix gen_cc_json
	$(MAKE) -C libc gen_cc_json
	$(MAKE) -C libgfx gen_cc_json
	$(MAKE) -C utest gen_cc_json
	$(MAKE) -C fib gen_cc_json
	$(MAKE) -C flappy gen_cc_json
	$(MAKE) -C doomgeneric gen_cc_json

clean:
	rm -f bin/*.elf
	$(MAKE) -C libmitrix clean
	$(MAKE) -C libc clean
	$(MAKE) -C libgfx clean
	$(MAKE) -C utest clean
	$(MAKE) -C fib clean
	$(MAKE) -C flappy clean
	$(MAKE) -C doomgeneric clean
