SRC=src
OUT=build

PROVIDED_CC?=i686-elf-gcc
PROVIDED_AS?=nasm

CC=$(PROVIDED_CC)
CC_flags =
CC_flags += -ffreestanding
CC_flags += -fno-builtin
CC_flags += -nostdlib
#CC_flags += -nostdinc
CC_flags += -Wall
CC_flags += -Wextra

CC_flags += -I$(SRC)

AS=$(PROVIDED_AS)
AS_flags=-f elf32

$(OUT)/boot.o: $(SRC)/boot.asm
	$(AS) $(AS_flags) $< -o $@
$(OUT)/idt.o: $(SRC)/idt/idt.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/serial.o: $(SRC)/serial/serial.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/fpu.o: $(SRC)/fpu/fpu.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/keyboard.o: $(SRC)/keyboard/keyboard.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/pit.o: $(SRC)/pit/pit.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/memory.o: $(SRC)/memory/memory.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/pmm.o: $(SRC)/memory/pmm.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/gfx.o: $(SRC)/gfx/gfx.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/vtty.o: $(SRC)/gfx/vtty.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/gui.o: $(SRC)/gfx/gui.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/tty.o: $(SRC)/gfx/tty.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/mem.o: $(SRC)/util/mem.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/debug.o: $(SRC)/util/debug.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/sys.o: $(SRC)/util/sys.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/tests.o: $(SRC)/tests/tests.c
	$(CC) -c $< -o $@ $(CC_flags)

$(OUT)/kernel.o: $(SRC)/kernel/kernel.c
	$(CC) -c $< -o $@ $(CC_flags)

OS=OS.flp
$(OUT)/$(OS): $(OUT)/boot.o $(OUT)/idt.o $(OUT)/kernel.o $(OUT)/fpu.o $(OUT)/serial.o $(OUT)/mem.o $(OUT)/debug.o $(OUT)/keyboard.o $(OUT)/tests.o $(OUT)/pit.o $(OUT)/sys.o $(OUT)/memory.o $(OUT)/pmm.o $(OUT)/gfx.o $(OUT)/vtty.o $(OUT)/gui.o $(OUT)/tty.o
	$(CC) -T $(SRC)/linker.ld -o $@ $^ -ffreestanding -nostdlib -lgcc

gen_cc_json: clean
	bear -- make $(OUT)/$(OS)

clean:
	rm -rf $(OUT)/*

VM=qemu-system-i386
# run: $(OUT)/$(OS)
# 	$(VM) -serial stdio -kernel $<
run: $(OUT)/mitrix.iso
	$(VM) -serial stdio $<

$(OUT)/mitrix.iso: $(OUT)/$(OS)
	cp $(OUT)/$(OS) mitrix/boot/kernel
	grub-mkrescue -o $@ mitrix

bochs: $(OUT)/mitrix.iso
	bochs -f bochsrc.txt
