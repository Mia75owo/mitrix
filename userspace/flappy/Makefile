OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
EXE = flappy.elf

CFLAGS += -I../libgfx/include
CFLAGS += -O2
LDFLAGS += -L../libgfx -l:libgfx.a

include ../../util.mk

.DEFAULT_GOAL := all
all: $(EXE)

$(EXE): $(OBJ)
	@$(LD) -o $(EXE) $(OBJ) $(LDFLAGS)
	@$(call PRETTYLD,"$<")

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)
	@$(call PRETTYCC,"$<")

gen_cc_json: clean # Genereate compile_commands.json for LSP
	bear -- make all

clean: # Clean generated files
	rm -rf *.o $(EXE)
	@$(call PRETTYRM,"*.o $(EXE)")
