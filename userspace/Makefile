PROVIDED_CC?=i686-elf-gcc
PROVIDED_AS?=nasm
PROVIDED_AR?=i686-elf-ar

CC=$(PROVIDED_CC)
AS=$(PROVIDED_AS)
LD=$(PROVIDED_CC)
AR=$(PROVIDED_AR)

CC_flags =
CC_flags += -ffreestanding
CC_flags += -fno-builtin
CC_flags += -nostdlib
CC_flags += -Wall
CC_flags += -Wextra
CC_flags += -I../libmitrix/include
CC_flags += -I../libc/include

AS_flags=-f elf32

LD_flags = 
LD_flags += -ffreestanding
LD_flags += -fno-builtin
LD_flags += -nostdlib
LD_flags += -L../libmitrix -l:libmitrix.a -L../libc -l:libc.a -static-libgcc -lgcc

export

all:
	mkdir -p bin
	$(MAKE) -C libmitrix
	$(MAKE) -C libc

	$(MAKE) -C utest
	cp utest/*.exe bin/

gen_cc_json:
	$(MAKE) -C libmitrix gen_cc_json
	$(MAKE) -C utest gen_cc_json

clean:
	$(MAKE) -C utest clean
	rm -f bin/*.exe
