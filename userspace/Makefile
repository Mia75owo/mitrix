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

include ../util.mk

.DEFAULT_GOAL := all
all:
	@mkdir -p bin
	$(MAKE) --no-print-directory -C libmitrix
	$(MAKE) --no-print-directory -C libc
	$(MAKE) --no-print-directory -C libgfx

	$(MAKE) --no-print-directory -C utest
	@cp utest/*.elf bin/
	$(MAKE) --no-print-directory -C fib
	@cp fib/*.elf bin/
	$(MAKE) --no-print-directory -C flappy
	@cp flappy/*.elf bin/
	$(MAKE) --no-print-directory -C doomgeneric
	@cp doomgeneric/*.elf bin/

gen_cc_json: # Genereate compile_commands.json for LSP
	$(MAKE) --no-print-directory -C libmitrix gen_cc_json
	$(MAKE) --no-print-directory -C libc gen_cc_json
	$(MAKE) --no-print-directory -C libgfx gen_cc_json
	$(MAKE) --no-print-directory -C utest gen_cc_json
	$(MAKE) --no-print-directory -C fib gen_cc_json
	$(MAKE) --no-print-directory -C flappy gen_cc_json
	$(MAKE) --no-print-directory -C doomgeneric gen_cc_json

clean: # Clean generated files
	@rm -rf bin/*.elf
	$(call PRETTYRM,"bin/*.elf")
	$(MAKE) --no-print-directory -C libmitrix clean
	$(MAKE) --no-print-directory -C libc clean
	$(MAKE) --no-print-directory -C libgfx clean
	$(MAKE) --no-print-directory -C utest clean
	$(MAKE) --no-print-directory -C fib clean
	$(MAKE) --no-print-directory -C flappy clean
	$(MAKE) --no-print-directory -C doomgeneric clean
