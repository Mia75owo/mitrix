SRC=src
OUT=build

PROVIDED_CC?=i686-elf-gcc
PROVIDED_AS?=nasm
PROVIDED_AR?=i686-elf-ar

export PROVIDED_CC
export PROVIDED_AS
export PROVIDED_AR

CC=$(PROVIDED_CC)
CC_flags =
CC_flags += -ffreestanding
CC_flags += -fno-builtin
CC_flags += -nostdlib
CC_flags += -Wall
CC_flags += -Wextra
CC_flags += -I$(SRC)
CC_flags += -g

AS=$(PROVIDED_AS)
AS_flags=-f elf32 -g

NATIVE_CC=gcc

.PHONY: always
always:

################
# Source files #
################

$(OUT)/boot.o: $(SRC)/boot.asm
	$(AS) $(AS_flags) $< -o $@

$(OUT)/gdt.o: $(SRC)/gdt/gdt.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/idt.o: $(SRC)/idt/idt.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/handlers.o: $(SRC)/idt/handlers.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/tss.o: $(SRC)/tasks/tss.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/tasks.o: $(SRC)/tasks/tasks.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/task_manager.o: $(SRC)/tasks/task_manager.c
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
$(OUT)/kmalloc.o: $(SRC)/memory/kmalloc.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/shmem.o: $(SRC)/memory/shmem.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/gfx.o: $(SRC)/gfx/gfx.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/vtty.o: $(SRC)/gfx/vtty.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/gui.o: $(SRC)/gfx/gui.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/tty.o: $(SRC)/gfx/tty.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/disk.o: $(SRC)/disk/disk.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/mifs.o: $(SRC)/disk/mifs.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/shell.o: $(SRC)/shell/shell.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/mem.o: $(SRC)/util/mem.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/debug.o: $(SRC)/util/debug.c
	$(CC) $(CC_flags) -c $< -o $@
$(OUT)/sys.o: $(SRC)/util/sys.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/elf.o: $(SRC)/elf/elf.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/syscalls.o: $(SRC)/syscalls/syscalls.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/tests.o: $(SRC)/tests/tests.c
	$(CC) $(CC_flags) -c $< -o $@

$(OUT)/kernel.o: $(SRC)/kernel/kernel.c
	$(CC) $(CC_flags) -c $< -o $@

###########
# RAMDISK #
###########

RAMDISK=ramdisk
$(OUT)/$(RAMDISK): $(OUT)/tool_mifs userspace ramdisk/
	cd ramdisk && ../build/tool_mifs ../$@ ./*
	# $(OUT)/tool_mifs $@ ./ramdisk/*
	# dd if=/dev/zero of=$@ bs=10M count=1
	# mformat -i $@ -F ::
	# mcopy -i $@ -s ramdisk/* ::

#############
# OS-floppy #
#############

OS=OS.flp
$(OUT)/$(OS): $(OUT)/boot.o $(OUT)/gdt.o $(OUT)/idt.o $(OUT)/handlers.o $(OUT)/kernel.o $(OUT)/fpu.o $(OUT)/serial.o $(OUT)/mem.o $(OUT)/debug.o $(OUT)/keyboard.o $(OUT)/tests.o $(OUT)/pit.o $(OUT)/sys.o $(OUT)/memory.o $(OUT)/pmm.o $(OUT)/kmalloc.o $(OUT)/shmem.o $(OUT)/gfx.o $(OUT)/vtty.o $(OUT)/gui.o $(OUT)/tty.o $(OUT)/disk.o $(OUT)/mifs.o $(OUT)/tss.o $(OUT)/tasks.o $(OUT)/task_manager.o $(OUT)/shell.o $(OUT)/elf.o $(OUT)/syscalls.o
	$(CC) -T $(SRC)/linker.ld -o $@ $^ -ffreestanding -nostdlib -lgcc

############
# ISO file #
############

$(OUT)/mitrix.iso: $(OUT)/$(OS) $(OUT)/$(RAMDISK)
	cp $(OUT)/$(OS) mitrix/boot/kernel
	cp $(OUT)/$(RAMDISK) mitrix/boot/ramdisk
	grub-mkrescue -o $@ mitrix

#########################
# compile_commands.json #
#########################

gen_cc_json: clean
	bear -- make $(OUT)/$(OS)

#########################
# clean compile outputs #
#########################

clean:
	rm -rf $(OUT)/*
	rm -rf mitrix/boot/kernel
	rm -rf mitrix/boot/$(RAMDISK)
	make -C userspace clean

#########
# tools #
#########

$(OUT)/tool_mifs: tools/mifs.c
	$(NATIVE_CC) -Wall -Wextra -O2 $< -o $@

##################
# userspace code #
##################

userspace: always
	make -C userspace
	cp userspace/bin/* ramdisk/

###################
# run in emulator #
###################

VM=qemu-system-i386
run: $(OUT)/mitrix.iso
	$(VM) -serial stdio $<

bochs: $(OUT)/mitrix.iso
	bochs -f bochsrc.txt
