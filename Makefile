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

CC_flags += -I$(SRC)

AS=nasm
AS_flags=-f elf32

$(OUT)/boot.o: $(SRC)/boot.asm
	$(AS) $(AS_flags) $< -o $@
$(OUT)/idt.o: $(SRC)/idt/idt.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/tty.o: $(SRC)/tty/tty.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/fpu.o: $(SRC)/fpu/fpu.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/keyboard.o: $(SRC)/keyboard/keyboard.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/serial.o: $(SRC)/util/serial.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/mem.o: $(SRC)/util/mem.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/debug.o: $(SRC)/util/debug.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/tests.o: $(SRC)/tests/tests.c
	$(CC) -c $< -o $@ $(CC_flags)

$(OUT)/kernel.o: $(SRC)/kernel/kernel.c
	$(CC) -c $< -o $@ $(CC_flags)

OS=OS.flp
$(OUT)/$(OS): $(OUT)/boot.o $(OUT)/idt.o $(OUT)/kernel.o $(OUT)/tty.o $(OUT)/fpu.o $(OUT)/serial.o $(OUT)/mem.o $(OUT)/debug.o $(OUT)/keyboard.o $(OUT)/tests.o
	$(CC) -T $(SRC)/linker.ld -o $@ $^ -ffreestanding -nostdlib -lgcc

gen_cc_json: clean
	bear -- make $(OUT)/$(OS)

clean:
	rm -rf $(OUT)/*

VM=qemu-system-i386
run: $(OUT)/$(OS)
	$(VM) -serial stdio -kernel $<
