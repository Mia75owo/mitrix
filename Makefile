SRC=src
OUT=build

CC=i686-elf-gcc
CC_flags =
CC_flags += -ffreestanding
CC_flags += -fno-builtin
CC_flags += -nostdlib
#CC_flags += -nostdinc
CC_flags += -Wall
CC_flags += -Wextra

AS=nasm
AS_flags=-f elf32

$(OUT)/boot.o: $(SRC)/boot.asm
	$(AS) $(AS_flags) $< -o $@
$(OUT)/kernel.o: $(SRC)/kernel/kernel.c
	$(CC) -c $< -o $@ $(CC_flags)

OS=OS.flp
$(OUT)/$(OS): $(OUT)/boot.o $(OUT)/kernel.o
	$(CC) -T $(SRC)/linker.ld -o $@ $^ -ffreestanding -nostdlib

clean:
	rm -rf $(OUT)/*

VM=qemu-system-i386
run: $(OUT)/$(OS)
	$(VM) -kernel $<
