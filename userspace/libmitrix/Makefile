OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CFLAGS =
CFLAGS += -O3
CFLAGS += -ffreestanding
CFLAGS += -fno-builtin
CFLAGS += -nostdlib
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -I include

include ../../util.mk

.DEFAULT_GOAL := all
all: libmitrix.a

libmitrix.a: $(OBJ)
	@$(AR) rcs $@ $^
	@$(call PRETTYAR,"$@")

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)
	@$(call PRETTYCC,"$<")

gen_cc_json: clean # Genereate compile_commands.json for LSP
	bear -- make all

clean: # Clean generated files
	@rm -rf *.o libmitrix.a
	@$(call PRETTYRM,"*.o libmitrix.a")
