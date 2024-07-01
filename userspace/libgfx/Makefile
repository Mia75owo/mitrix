OBJ = $(patsubst %.c, %.o, $(wildcard src/*.c))

CFLAGS += -O3
CFLAGS += -I include

include ../../util.mk

.DEFAULT_GOAL := all
all: libgfx.a

libgfx.a: $(OBJ)
	@$(AR) rcs $@ $^
	@$(call PRETTYAR,"$@")

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)
	@$(call PRETTYCC,"$<")

gen_cc_json: clean # Genereate compile_commands.json for LSP
	bear -- make all

clean: # Clean generated files
	@rm -rf *.o libgfx.a
	@$(call PRETTYRM,"*.o libgfx.a")
