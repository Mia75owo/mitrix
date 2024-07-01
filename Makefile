SHELL = /bin/sh

SRC=src
OUT=build

export CC=~/opt/cross/bin/i686-elf-gcc
export LD=~/opt/cross/bin/i686-elf-gcc
export AR=~/opt/cross/bin/i686-elf-ar
export AS=nasm

CFLAGS=
CFLAGS += -O3
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -ffreestanding
CFLAGS += -fno-builtin
CFLAGS += -nostdlib
CFLAGS += -I$(SRC)
CFLAGS += -g

ASFLAGS=
ASFLAGS += -f elf32
ASFLAGS += -g

NATIVE_CC=gcc

include util.mk

.PHONY: always clean userspace
always:
	mkdir -p $(OUT)

all: $(OUT)/mitrix.iso

clean: # Clean generated files
	@rm -rf $(OUT)/*
	@$(call PRETTYRM,"$(OUT)/*")
	@rm -rf mitrix/boot/kernel
	@$(call PRETTYRM,"mitrix/boot/kernel")
	@rm -rf mitrix/boot/$(RAMDISK)
	@$(call PRETTYRM,"mitrix/boot/$(RAMDISK)")
	$(MAKE) --no-print-directory -C userspace clean


################
# Source files #
################

# C files
CFILES = $(wildcard $(SRC)/**/*.c)
COBJ = $(CFILES:$(SRC)/%.c=$(OUT)/%.o)
CDEPS = $(COBJ:%.o=%.d)

$(COBJ): Makefile
-include $(CDEPS)

$(OUT)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@
	@$(call PRETTYCC,"$<")

# ASM files
ASMFILES = $(SRC)/boot.asm
ASMOBJ = $(ASMFILES:$(SRC)/%.asm=$(OUT)/%.o)
ASMDEPS = $(ASMOBJ:%.o=%.d)

$(ASMOBJ): Makefile
-include $(ASMDEPS)

$(OUT)/%.o: $(SRC)/%.asm
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) -MD $(@:%.o=%.d) $< -o $@
	@$(call PRETTYAS,"$<")

###########
# Ramdisk #
###########

RAMDISK=ramdisk
$(OUT)/$(RAMDISK): $(OUT)/tool_mifs userspace ramdisk/
	cd ramdisk && ../$(OUT)/tool_mifs ../$@ ./*

#############
# OS-floppy #
#############

OS=OS.flp
$(OUT)/$(OS): $(COBJ) $(ASMOBJ)
	@$(CC) -T $(SRC)/linker.ld -o $@ $^ -ffreestanding -nostdlib -lgcc
	@$(call PRETTYLD,"$<")

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
	bear -- $(MAKE) $(OUT)/$(OS)
	$(MAKE) --no-print-directory -C userspace gen_cc_json

#########
# tools #
#########

$(OUT)/tool_mifs: tools/mifs.c
	$(NATIVE_CC) -Wall -Wextra -O2 $< -o $@

##################
# userspace code #
##################

userspace: # Compile userspace programs
	$(MAKE) --no-print-directory -C userspace
	cp userspace/bin/* ramdisk/

###################
# run in emulator #
###################

VM=qemu-system-i386
run: $(OUT)/mitrix.iso # Compile and start the OS in QEMU
	$(VM) -enable-kvm -serial stdio -cdrom $< -boot d



# Error if toolchain was not found
TOOLCHAIN_NOT_FOUND := $(shell $(CC) --version 2>/dev/null)
$(if $(TOOLCHAIN_NOT_FOUND),,$(error Toolchain not found! Use 'toolchain/build_tools.sh' to compile the toolchain))
